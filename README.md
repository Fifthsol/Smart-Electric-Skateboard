# Smart-Electric-Skateboard

This is my PCB and CAD design for a smart electric skateboard, I designed it for the most possible modularity and to be as easily assembled as possible

Key features:
XT90-S spark key: this acts as a physical lock, completing the circuit needed for the board to power on

Gyroscope (MPU-6050): motion and orientation sensing

OLED display: onboard status readout

Buzzer

Wi-Fi + Bluetooth (via ESP32)

GPS logging (via Arduino Nano to avoid clogging ESP32)— tracks rides for eventual 
export to Strava

Regenerative braking: logged by the ESP32

Hall-effect thumb throttle: smooth, analog accel/decel control

Headlights: I used a MOFSET gate to control headlights using the ESP32, the headlights are 12V and plenty bright :)

The PCB is designed for easy assembly and repairability: the ESP32 and Arduino Nano mount use through-hole headers, and most other component connects through screw terminals so you don't have to resolder anything if you want to upgrade a module. A handful of parts (like the XT90 connectors) sit off-board so that wiring is easy to manage. Additionally this board is compatible with basically ever single commercially available ESC, motor, and thumb throttle costing significantly less than other commercially available "smart" skateboards with more configurability.)

You can mount the PCB using countersunk M5 bolts drilled into the main deck of your board.
