# Smart-Electric-Skateboard


FIRMWARE IS HUMAN REVIEWED, CLAUDE WRITTEN 
WILL REWRITE BEFORE CREATING FINAL VERSION AND TESTING WITH REAL HARDWARE, DON'T USE FIRMWARE YET.

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


<img src = "https://cdn.hackclub.com/019f2509-905f-7cde-beeb-4838f69e2123/Screenshot%202026-07-02%20185322.png">

<img src = "https://cdn.hackclub.com/019f1ba8-6494-78ec-aa2a-8b537365bdf5/Smart%20Skateboard%20Wiring%20Diagram.drawio(1).png">

<img src = "https://cdn.hackclub.com/019f2508-aadb-7b29-8780-0e3aca4f722f/Screenshot%202026-07-02%20185056.png">



<img src="https://cdn.hackclub.com/019f2504-a272-7190-97ed-579479bf9502/Screenshot%202026-07-02%20184754.png">

<img src = "https://cdn.hackclub.com/019f1fa6-cb4b-7150-bc92-e3b2824ae3d5/schematic.png">

https://cad.onshape.com/documents/cc961e82541e14a55eb3a3aa/w/93f68d3bf484e10ac8913aa9/e/0b85ebaf63d54804817c31e9?renderMode=0&uiState=6a4718eb525d8b8d88a0e782