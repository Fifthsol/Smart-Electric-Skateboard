#include <SoftwareSerial.h>
#include <TinyGPSPlus.h>
#include <SPI.h>
#include <SD.h>

constexpr uint8_t PIN_GPS_RX = 4;   // Nano listens here (GPS TX)
constexpr uint8_t PIN_GPS_TX = 3;   // Nano talks here   (GPS RX)
constexpr uint8_t PIN_SD_CS  = 10;

constexpr uint32_t GPS_BAUD  = 9600;   // NEO-6M default
constexpr uint32_t LINK_BAUD = 9600;   // to ESP32 (keep modest for SoftwareSerial coexistence)
constexpr uint32_t FIX_PERIOD_MS = 1000;

SoftwareSerial gpsSerial(PIN_GPS_RX, PIN_GPS_TX);
TinyGPSPlus gps;

bool sdOK = false;
bool logging = false;
File gpxFile;
char gpxName[13];          // 8.3 filename e.g. RIDE07.GPX
uint32_t lastFixSent = 0;
uint32_t lastLogged  = 0;

char cmdBuf[16];
uint8_t cmdLen = 0;

// ---------------------------------------------------------
void setup() {
  Serial.begin(LINK_BAUD);       // hardware UART -> ESP32
  gpsSerial.begin(GPS_BAUD);

  sdOK = SD.begin(PIN_SD_CS);
  Serial.println(sdOK ? F("$SD,1") : F("$SD,0"));
}

// ---------------------------------------------------------
void loop() {
  // Feed the NMEA parser
  while (gpsSerial.available()) gps.encode(gpsSerial.read());

  // Commands from the ESP32: $LOG,1  /  $LOG,0
  while (Serial.available()) {
    char ch = (char)Serial.read();
    if (ch == '\n' || ch == '\r') {
      if (cmdLen) { cmdBuf[cmdLen] = 0; handleCmd(); cmdLen = 0; }
    } else if (cmdLen < sizeof(cmdBuf) - 1) {
      cmdBuf[cmdLen++] = ch;
    } else cmdLen = 0;
  }

  uint32_t now = millis();
  if (now - lastFixSent >= FIX_PERIOD_MS) {
    lastFixSent = now;
    sendFix();
    if (logging && gps.location.isValid()) logTrackpoint();
  }
}

// ---------------------------------------------------------
void handleCmd() {
  if (!strncmp(cmdBuf, "$LOG,1", 6)) startLog();
  else if (!strncmp(cmdBuf, "$LOG,0", 6)) stopLog();
}

// ---------------------------------------------------------
void sendFix() {
  if (gps.location.isValid() && gps.location.age() < 3000) {
    // $FIX,lat,lon,kmh,alt,sats,hh:mm:ss
    Serial.print(F("$FIX,"));
    Serial.print(gps.location.lat(), 6); Serial.print(',');
    Serial.print(gps.location.lng(), 6); Serial.print(',');
    Serial.print(gps.speed.kmph(), 1);   Serial.print(',');
    Serial.print(gps.altitude.meters(), 1); Serial.print(',');
    Serial.print(gps.satellites.value()); Serial.print(',');
    printTime2(gps.time.hour());   Serial.print(':');
    printTime2(gps.time.minute()); Serial.print(':');
    printTime2(gps.time.second());
    Serial.println();
  } else {
    Serial.print(F("$NOFIX,"));
    Serial.println(gps.satellites.value());
  }
}

void printTime2(uint8_t v) {
  if (v < 10) Serial.print('0');
  Serial.print(v);
}

// ---------------------------------------------------------
void startLog() {
  if (logging || !sdOK) { ack(); return; }
  // Find a free RIDE##.GPX slot
  for (uint8_t i = 0; i < 100; i++) {
    snprintf(gpxName, sizeof(gpxName), "RIDE%02u.GPX", i);
    if (!SD.exists(gpxName)) break;
  }
  gpxFile = SD.open(gpxName, FILE_WRITE);
  if (gpxFile) {
    gpxFile.println(F("<?xml version=\"1.0\" encoding=\"UTF-8\"?>"));
    gpxFile.println(F("<gpx version=\"1.1\" creator=\"SmartSk8\" xmlns=\"http://www.topografix.com/GPX/1/1\">"));
    gpxFile.println(F("<trk><name>Skateboard Ride</name><trkseg>"));
    gpxFile.flush();
    logging = true;
  }
  ack();
}

void stopLog() {
  if (logging && gpxFile) {
    gpxFile.println(F("</trkseg></trk></gpx>"));
    gpxFile.close();
  }
  logging = false;
  ack();
}

void ack() {
  Serial.print(F("$LOGACK,"));
  Serial.println(logging ? '1' : '0');
}

// ---------------------------------------------------------
void logTrackpoint() {
  if (!gpxFile) return;
  // <trkpt lat=".." lon=".."><ele>..</ele><time>YYYY-MM-DDTHH:MM:SSZ</time></trkpt>
  gpxFile.print(F("<trkpt lat=\""));
  gpxFile.print(gps.location.lat(), 6);
  gpxFile.print(F("\" lon=\""));
  gpxFile.print(gps.location.lng(), 6);
  gpxFile.print(F("\"><ele>"));
  gpxFile.print(gps.altitude.meters(), 1);
  gpxFile.print(F("</ele>"));

  if (gps.date.isValid() && gps.time.isValid()) {
    char ts[24];
    snprintf(ts, sizeof(ts), "<time>%04u-%02u-%02uT%02u:%02u:%02uZ</time>",
             gps.date.year(), gps.date.month(), gps.date.day(),
             gps.time.hour(), gps.time.minute(), gps.time.second());
    gpxFile.print(ts);
  }
  gpxFile.println(F("</trkpt>"));

  // Flush every ~5s so a battery pull doesn't eat the ride
  if (millis() - lastLogged > 5000) { gpxFile.flush(); lastLogged = millis(); }
}
