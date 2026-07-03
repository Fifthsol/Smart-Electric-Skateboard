# Smart Electric Skateboard

> **FIRMWARE IS HUMAN REVIEWED, CLAUDE WRITTEN**
> WILL REWRITE BEFORE CREATING FINAL VERSION AND TESTING WITH REAL HARDWARE, DON'T USE FIRMWARE YET.

This is my PCB and CAD design for a smart electric skateboard, I designed it for the most possible modularity and to be as easily assembled as possible

## Key Features

- **XT90-S spark key**: this acts as a physical lock, completing the circuit needed for the board to power on
- **Gyroscope (MPU-6050)**: motion and orientation sensing
- **OLED display**: onboard status readout
- **Buzzer**
- **Wi-Fi + Bluetooth** (via ESP32)
- **GPS logging** (via Arduino Nano to avoid clogging ESP32) — tracks rides for eventual export to Strava
- **Regenerative braking**: logged by the ESP32
- **Hall-effect thumb throttle**: smooth, analog accel/decel control
- **Headlights**: I used a MOFSET gate to control headlights using the ESP32, the headlights are 12V and plenty bright :)

## Design Notes

The PCB is designed for easy assembly and repairability: the ESP32 and Arduino Nano mount use through-hole headers, and most other component connects through screw terminals so you don't have to resolder anything if you want to upgrade a module. A handful of parts (like the XT90 connectors) sit off-board so that wiring is easy to manage. Additionally this board is compatible with basically ever single commercially available ESC, motor, and thumb throttle costing significantly less than other commercially available "smart" skateboards with more configurability.)

You can mount the PCB using countersunk M5 bolts drilled into the main deck of your board. Reference the CAD for the positioning of these holes.

The cover that goes over can be printed and you can use velcro to create a removable cover for all electronics :)

As some of the wiring sits off board, reference my wiring diagram below :) I believe it's pretty self explanatory, use wire splice connectors where there is break points, lay everything out before hand so you cut the appropriate amount of 10 AWG wire.


## Images

<img src = "https://cdn.hackclub.com/019f25f9-34dd-704a-a63d-0e10b274460a/Final%20PCB.png">

here's my final two layer PCB :) you can find the KiCAD file in this repo, when ordering a PCB please only download the final gerber zip (I think I deleted the old one but still be careful)

<img src="https://cdn.hackclub.com/019f1ba8-6494-78ec-aa2a-8b537365bdf5/Smart%20Skateboard%20Wiring%20Diagram.drawio(1).png">


<img src="https://cdn.hackclub.com/019f2508-aadb-7b29-8780-0e3aca4f722f/Screenshot%202026-07-02%20185056.png">

<img src="https://cdn.hackclub.com/019f2504-a272-7190-97ed-579479bf9502/Screenshot%202026-07-02%20184754.png">

See full assembly in the Onshape link below!

<img src="https://cdn.hackclub.com/019f1fa6-cb4b-7150-bc92-e3b2824ae3d5/schematic.png">

## CAD

[https://cad.onshape.com/documents/cc961e82541e14a55eb3a3aa/w/93f68d3bf484e10ac8913aa9/e/999f5a267f0d529136fe4578?renderMode=0&uiState=6a472d69a55f8813fe6f1d74 ](https://cad.onshape.com/documents/cc961e82541e14a55eb3a3aa/w/93f68d3bf484e10ac8913aa9/e/999f5a267f0d529136fe4578?renderMode=0&uiState=6a472d69a55f8813fe6f1d74) - SOURCE FILES, YOU CAN EXPORT, FOR SOME REASON ONSHAPE DOESN'T LET ME LET PUBLIC MAKE COPIES, LMK IF U NEED MORE FILES

I also added the files for the XT90S holder and key I got from thingiverse but you can also get them from the creators' thingiverse links:

https://www.thingiverse.com/thing:3755061 - DSC101
 https://www.thingiverse.com/thing:2323422 - MMANER

Additionally I'd like to credit Linny for the wonderful spark key tutorial:

https://forum.esk8.news/t/how-to-make-a-xt90-loop-key/17664 
