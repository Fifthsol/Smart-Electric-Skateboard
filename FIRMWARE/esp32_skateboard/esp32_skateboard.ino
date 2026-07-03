#include <Wire.h>
#include <ESP32Servo.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_MPU6050.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// ------------------------- Pins -------------------------
constexpr int PIN_ESC       = 25;   // PPM out to Mini FSESC4.20
constexpr int PIN_THROTTLE  = 34;   // ADC1_CH6, input-only
constexpr int PIN_HEADLIGHT = 4;    // MOSFET gate
constexpr int PIN_SDA       = 21;
constexpr int PIN_SCL       = 22;
constexpr int PIN_NANO_RX   = 16;   // Serial2 RX  <- Nano TX
constexpr int PIN_NANO_TX   = 17;   // Serial2 TX  -> Nano RX

// ------------------------- Tuning -------------------------
// Raw ADC counts from the hall throttle AFTER the 10K/20K divider.
// 0.8V rest / 4.2V full on the sensor becomes ~0.53V / ~2.8V at the pin.
// Run once with DEBUG_THROTTLE=true, note your real min/max, put them here.
constexpr bool  DEBUG_THROTTLE   = false;
constexpr int   THR_RAW_MIN      = 700;    // raw ADC at rest      (CALIBRATE!)
constexpr int   THR_RAW_MAX      = 3300;   // raw ADC at full pull (CALIBRATE!)
constexpr int   THR_DEADBAND     = 60;     // counts of slop above rest

constexpr int   ESC_MIN_US       = 1500;   // neutral (VESC "current, no reverse")
constexpr int   ESC_MAX_US       = 2000;   // full throttle
constexpr int   ESC_NEUTRAL_US   = 1500;
// Soft-start: max microseconds of throttle change per 20ms control tick.
constexpr int   ESC_SLEW_US      = 12;

// Safety
constexpr float TILT_CUTOFF_DEG    = 65.0;  // board this far over = you fell off
constexpr uint32_t THROTTLE_TIMEOUT_MS = 250; // bad ADC / wire pull -> neutral

// ------------------------- BLE UUIDs -------------------------
#define SVC_UUID   "6e400001-b5a3-f393-e0a9-e50e24dcca9e" // NUS-style
#define RX_UUID    "6e400002-b5a3-f393-e0a9-e50e24dcca9e" // app -> board
#define TX_UUID    "6e400003-b5a3-f393-e0a9-e50e24dcca9e" // board -> app (notify)

// ------------------------- Globals -------------------------
Servo esc;
Adafruit_SSD1306 oled(128, 64, &Wire, -1);
Adafruit_MPU6050 mpu;
bool oledOK = false, mpuOK = false;

BLECharacteristic* bleTx = nullptr;
volatile bool bleConnected = false;

// State
bool  armed      = false;   // must be armed (app or auto) before ESC responds
bool  headlight  = false;
bool  logging    = false;   // Nano SD logging state (echoed back by Nano)
int   escNowUs   = ESC_NEUTRAL_US;
float tiltDeg    = 0;
uint32_t lastControlTick = 0;
uint32_t lastTelemetry   = 0;
uint32_t lastOled        = 0;

// GPS data streamed up from the Nano:  $FIX,lat,lon,kmh,alt,sats,hhmmss
struct GpsData {
  bool   valid = false;
  double lat = 0, lon = 0;
  float  kmh = 0, alt = 0;
  int    sats = 0;
  char   timeStr[10] = "--:--:--";
  uint32_t lastRx = 0;
} gps;

char nanoLine[96];
size_t nanoLen = 0;

// ------------------------- BLE callbacks -------------------------
class ServerCB : public BLEServerCallbacks {
  void onConnect(BLEServer*) override    { bleConnected = true; }
  void onDisconnect(BLEServer* s) override {
    bleConnected = false;
    // Failsafe: rider's phone dropped mid-command? Coast to neutral.
    s->getAdvertising()->start();
  }
};

void handleCommand(const String& cmd);

class RxCB : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* c) override {
    String v = String(c->getValue().c_str());
    v.trim();
    if (v.length()) handleCommand(v);
  }
};

// ------------------------- Setup -------------------------
void setup() {
  Serial.begin(115200);                                  // USB debug
  Serial2.begin(9600, SERIAL_8N1, PIN_NANO_RX, PIN_NANO_TX); // Nano link

  pinMode(PIN_HEADLIGHT, OUTPUT);
  digitalWrite(PIN_HEADLIGHT, LOW);

  analogReadResolution(12);
  analogSetPinAttenuation(PIN_THROTTLE, ADC_11db);       // full 0-3.3V range

  // ESC: standard 50Hz servo signal. VESC must be configured for PPM
  // ("App Settings -> PPM -> Current No Reverse") in VESC Tool.
  esc.setPeriodHertz(50);
  esc.attach(PIN_ESC, 1000, 2000);
  esc.writeMicroseconds(ESC_NEUTRAL_US);

  Wire.begin(PIN_SDA, PIN_SCL);
  oledOK = oled.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  if (oledOK) {
    oled.clearDisplay();
    oled.setTextColor(SSD1306_WHITE);
    oled.setTextSize(2);
    oled.setCursor(10, 24);
    oled.print("SK8 BOOT");
    oled.display();
  }
  mpuOK = mpu.begin(0x68);
  if (mpuOK) {
    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  }

  // BLE
  BLEDevice::init("SmartSk8");
  BLEServer* srv = BLEDevice::createServer();
  srv->setCallbacks(new ServerCB());
  BLEService* svc = srv->createService(SVC_UUID);
  bleTx = svc->createCharacteristic(TX_UUID, BLECharacteristic::PROPERTY_NOTIFY);
  bleTx->addDescriptor(new BLE2902());
  BLECharacteristic* rx =
      svc->createCharacteristic(RX_UUID, BLECharacteristic::PROPERTY_WRITE);
  rx->setCallbacks(new RxCB());
  svc->start();
  BLEAdvertising* adv = BLEDevice::getAdvertising();
  adv->addServiceUUID(SVC_UUID);
  adv->setScanResponse(true);
  adv->start();

  Serial.println(F("[ESP32] Smart skateboard up. Waiting for ARM."));
}

// ------------------------- Command handler -------------------------
// App -> board commands (also usable from the Arduino Serial monitor
// via USB for bench testing — type them into Serial):
//   ARM / DISARM        enable-disable the ESC output
//   LIGHT ON|OFF|TOGGLE headlight MOSFET
//   LOG START|STOP      SD ride logging on the Nano (GPX for Strava)
//   PING                replies PONG
void handleCommand(const String& cmd) {
  String c = cmd; c.toUpperCase();
  if      (c == "ARM")          { armed = true;  beepAck(); }
  else if (c == "DISARM")       { armed = false; escNowUs = ESC_NEUTRAL_US; }
  else if (c == "LIGHT ON")     { headlight = true; }
  else if (c == "LIGHT OFF")    { headlight = false; }
  else if (c == "LIGHT TOGGLE") { headlight = !headlight; }
  else if (c == "LOG START")    { Serial2.println(F("$LOG,1")); }
  else if (c == "LOG STOP")     { Serial2.println(F("$LOG,0")); }
  else if (c == "PING")         { bleNotify("PONG"); }
  digitalWrite(PIN_HEADLIGHT, headlight ? HIGH : LOW);
  Serial.print(F("[CMD] ")); Serial.println(c);
}

void bleNotify(const String& s) {
  if (bleConnected && bleTx) {
    bleTx->setValue(s.c_str());
    bleTx->notify();
  }
}

// NOTE: the buzzer on this PCB rev is wired straight across 3V3 and GND
// (BZ1 pads: 3V3 / GND — no GPIO net), so firmware can't actually drive it.
// This is a stub so the call sites are ready when you bodge it to a GPIO.
void beepAck() { /* wire BZ1 to a free GPIO (e.g. 26/27) to enable */ }

// ------------------------- Nano serial parsing -------------------------
void pollNano() {
  while (Serial2.available()) {
    char ch = (char)Serial2.read();
    if (ch == '\n' || ch == '\r') {
      if (nanoLen) { nanoLine[nanoLen] = 0; parseNanoLine(nanoLine); nanoLen = 0; }
    } else if (nanoLen < sizeof(nanoLine) - 1) {
      nanoLine[nanoLen++] = ch;
    } else nanoLen = 0; // overflow, resync
  }
}

void parseNanoLine(char* line) {
  // $FIX,lat,lon,kmh,alt,sats,hh:mm:ss   |   $NOFIX,sats   |   $LOGACK,0|1
  if (!strncmp(line, "$FIX,", 5)) {
    char* p = line + 5;
    char* tok[6]; int n = 0;
    for (char* t = strtok(p, ","); t && n < 6; t = strtok(nullptr, ",")) tok[n++] = t;
    if (n == 6) {
      gps.lat = atof(tok[0]); gps.lon = atof(tok[1]);
      gps.kmh = atof(tok[2]); gps.alt = atof(tok[3]);
      gps.sats = atoi(tok[4]);
      strncpy(gps.timeStr, tok[5], sizeof(gps.timeStr) - 1);
      gps.valid = true; gps.lastRx = millis();
    }
  } else if (!strncmp(line, "$NOFIX", 6)) {
    gps.valid = false; gps.lastRx = millis();
    char* c = strchr(line, ','); if (c) gps.sats = atoi(c + 1);
  } else if (!strncmp(line, "$LOGACK,", 8)) {
    logging = (line[8] == '1');
    bleNotify(logging ? "LOGGING ON" : "LOGGING OFF");
  }
}

// ------------------------- Control loop pieces -------------------------
int readThrottleUs() {
  // Median-of-3 to knock down ADC noise
  int a = analogRead(PIN_THROTTLE);
  int b = analogRead(PIN_THROTTLE);
  int c = analogRead(PIN_THROTTLE);
  int raw = max(min(a, b), min(max(a, b), c));

  if (DEBUG_THROTTLE) { Serial.print(F("thr raw=")); Serial.println(raw); }

  raw = constrain(raw, THR_RAW_MIN, THR_RAW_MAX);
  if (raw < THR_RAW_MIN + THR_DEADBAND) return ESC_NEUTRAL_US;
  return map(raw, THR_RAW_MIN + THR_DEADBAND, THR_RAW_MAX, ESC_MIN_US, ESC_MAX_US);
}

void updateTilt() {
  if (!mpuOK) return;
  sensors_event_t a, g, t;
  mpu.getEvent(&a, &g, &t);
  // Roll relative to gravity — big number means board is on its side/upside-down
  float ax = a.acceleration.x, ay = a.acceleration.y, az = a.acceleration.z;
  tiltDeg = degrees(acos(constrain(az / sqrt(ax*ax + ay*ay + az*az), -1.0f, 1.0f)));
}

void controlTick() {
  updateTilt();

  int target = ESC_NEUTRAL_US;
  bool tiltFault = mpuOK && (tiltDeg > TILT_CUTOFF_DEG);

  if (armed && !tiltFault) target = readThrottleUs();
  if (tiltFault && armed) { armed = false; bleNotify("TILT CUTOFF"); }

  // Slew limit for a soft launch (down = instant, up = ramped)
  if (target > escNowUs)      escNowUs = min(target, escNowUs + ESC_SLEW_US);
  else                        escNowUs = target;

  esc.writeMicroseconds(escNowUs);
}

// ------------------------- OLED + telemetry -------------------------
void drawOled() {
  if (!oledOK) return;
  oled.clearDisplay();
  oled.setTextSize(3);
  oled.setCursor(0, 0);
  if (gps.valid) { oled.print((int)gps.kmh); oled.setTextSize(2); oled.print(F("km/h")); }
  else           { oled.setTextSize(2); oled.print(F("NO GPS")); }

  oled.setTextSize(1);
  oled.setCursor(0, 32);
  oled.print(F("SAT:")); oled.print(gps.sats);
  oled.print(F("  ")); oled.print(gps.timeStr);

  oled.setCursor(0, 44);
  oled.print(armed ? F("[ARMED]") : F("[SAFE ]"));
  oled.print(headlight ? F(" LIGHT") : F("      "));
  oled.print(logging ? F(" REC")   : F("    "));

  oled.setCursor(0, 56);
  oled.print(F("BLE:")); oled.print(bleConnected ? F("OK ") : F("-- "));
  oled.print(F("THR:")); oled.print(escNowUs);
  oled.display();
}

void sendTelemetry() {
  // Board -> app, one CSV line per second:
  // T,<armed>,<light>,<log>,<kmh>,<lat>,<lon>,<sats>,<tilt>,<escUs>
  char buf[120];
  snprintf(buf, sizeof(buf), "T,%d,%d,%d,%.1f,%.6f,%.6f,%d,%.0f,%d",
           armed, headlight, logging, gps.kmh, gps.lat, gps.lon,
           gps.sats, tiltDeg, escNowUs);
  bleNotify(buf);
}

// ------------------------- Main loop -------------------------
void loop() {
  uint32_t now = millis();

  pollNano();

  // Allow bench commands over USB serial too
  if (Serial.available()) {
    String s = Serial.readStringUntil('\n');
    s.trim(); if (s.length()) handleCommand(s);
  }

  if (now - lastControlTick >= 20) {        // 50 Hz control loop
    lastControlTick = now;
    controlTick();
  }
  if (now - lastOled >= 250) { lastOled = now; drawOled(); }
  if (now - lastTelemetry >= 1000) { lastTelemetry = now; sendTelemetry(); }

  // GPS link watchdog
  if (gps.valid && now - gps.lastRx > 5000) gps.valid = false;
}
