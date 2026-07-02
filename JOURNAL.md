# 6/29/26 - 8 Hours

## Day 1 of Designing, Part Choice, Wiring Diagram, and Rough CAD, 6/29/26
BTW I'm finalizing this journal a day late because I got sidetracked and forgot to commit it yesterday. Oops!

So before starting this project I had a very lofty idea on what I wanted to build and to be honest didn't understand the X tier project idea pool (sorry Alex), so I dreamed up my
lofty goal of a smart electric skateboard and sent it in slack with my hopes and dreams. Without accounting for the fact that... I actually need to design it before getting almost 1K in funding! I'm still not quite sure on how I'm going to finance this thing especially because I don't feel like it qualifies for X tier, S at most I feel... Regardless we move forward.

So, in order to punch my ticket to Outpost in approximately 9 days..... let's go


### Part Choices

I wanted to do this project because I had a spare RC car motor and ESC lying around that I figured out how to use with a NiMH battery pack. I thought that if I tweaked the gear ratio enough I could get enough power out of a measly 9.6V battery to fully flesh out an electric skateboard... Sadly I was wrong.

In order to make a smart electric skateboard you first have to have an electric skateboard (duh). So I started out with the deck. I realized that I actually wanted to be able to bring this thing places so I chose a relatively small base deck which can just be upgraded to a better one need be. A $40 27 x 8 longboard deck. Nothing terribly remarkable but should have enough space for all my electronics. 

Next up, was the drive train kit, which is essentially the belt and pulley system that mounts a motor to the axle of the skateboard. I found a simple one on diyelectricskateboard.org (a little bit on the nose, you think?) It comes with wheels, trucks, etc. etc. The assembled kit is pictured below.

<img src = "https://cdn.hackclub.com/019f16e0-972d-77a1-973d-29985746de4f/mechanical%20kit%20pic.png" width = "300">


As much as I'd love to buy the motor from the website I couldn't find a good ESC-Motor combination for cheap, so I looked on flipsky and ended up finding a good motor ESC combo with the "Mini FSESC4.2 50A" VESC and the "BLDC Belt Motor 6354 190KV" motor (I'll include links to all of the parts I'm planning to buy below). From there finding most of the other parts was was easy because the features I wanted came quite easily once you add an ESP32 and Arduino Nano to an electric skateboard architecture.

The features I wanted to add are:

- Bluetooth control (ESP32)
- An app that connects to the board and lets you post your rides on **Strava** (the most important feature for me)
- GPS Datalogging
- Headlights
- Hall effect throttle
- Spark key that unlocks the entire system by completing the circuit
- OLED Screen 
- Gyroscope

So I chose all of the parts relatively quickly as a lot of them are just standard ESP32/Arduino components.

This was stuff like the ESP32 kit, NEO6M GPS module, MPU-6050, SSD1306 OLED, on off switch, MicroSD module, buzzer, etc. etc.


However, something that became a bit of a pain was wiring all of these parts together, in particularly the hall effect thumb throttle I found on amazon and the LED light bar.

Since the battery I chose is a 6S 22.2V 5000mAH lipo I had to use a voltage step down module and added a UBEC to make sure the voltage is properly regulated as it flows to the ESP32, Arduino Nano, and the headlights.

These headlights ended up also being a pain. I found a 7 inch set on Amazon that is usually used for cars, I thought wiring it would be easy turns out it isn't because there isn't a signal wire on it and only the voltage and ground.

This is where I introduce my excellently formatted wiring diagram...
<img src = "https://cdn.hackclub.com/019f1b16-1831-7590-9dcf-12fc6555674d/Electric%20Skateboard%20Wiring.png" width = 600>

As you see here I added a MOFSET gate pin to ensure that I could turn off and on the headlights. Additionally, I had to put a lot of effort in making sure everything shares a common ground and that the Arduino Nano doesn't fry the ESP32; the ESP32 accepts only 3.3V input from signal pins and the Nano outputs 5V. Same thing occured with the hall effect thumb trigger.

A neat thing that I discovered while designign this wiring chart was the usage of a "spark key". When I was looking into all the different RC lipo battery connectors (which there's a LOT of), I decided to use exclusively the XT90-S, the S is for the extra resistor in the female housing that prevents current overflow, so when I was looking at documentation I stumbled upon an old forum post on how to set up a XT90 "loop key" Which is essentially an XT90 connector that completes the circuuit of a board as the on and off switch (I'm actually considering removing the toggle switch begause of the existence of this solution).

Below is an image of a loop key and a link to the tutorial.



https://forum.esk8.news/t/how-to-make-a-xt90-loop-key/17664 

<img src = "https://cdn.hackclub.com/019f1b1b-e77f-78c1-8277-7c5a9475fb69/fa4c533230fcef8eb53440110562e0ba560b5190_2_690x326.jpeg">

Tommorow, I plan on finalizing this wiring diagram in a software that isn't google drawings so I can actually design the mounting board that will go on the skateboard.

Oh I also did make a rough CAD of the case but I decided against using this specific design because of A) how annoying it'd be to print and B) I realized that I can just mount the electronics board to the skateboard with some really strong velcro adhesive.

Here's a picture of my old design which uses a bunch of 10-32 bolts and holes drilled into the skateboard itself.

<img src = "https://cdn.hackclub.com/019f1b1d-b07a-77c2-90d6-f0178e3e750e/Screenshot%202026-06-30%20203904.png">

You can see the slot for the XT90 spark key and the three holes for the motor wires that run from the ESC to the motor as well as the slot for the OLED display and a weird hole I thought I would use for the battery?!

The battery solution was another reason I switched to using a velcro mounted fixture because I wanted to switch the battery easily without it being too much of a pain. I realized that I could use velcro when I was thinking of how my FRC team (6423 represent!) mounted the acrylic panel that shielded the brain pan and how strong that velcro was. I don't have a photo but trust me it was really strong and none of our electrical system was damaged during the season. :))


Anyways thats all I did today, it took a long time....

Here's my BOM too:

## Drivetrain

| Item | Link | Price |
|---|---|---|
| Drive Train Kit | [Link](https://diyelectricskateboard.com/products/single-motor-mechanical-kit) | $250 |
| Motor — 6355 190KV Brushless Outrunner | [Link](https://flipsky.net/products/6354-motor?variant=39584732020923) | $71 |
| ESC — Flipsky Mini FSESC4.20 50A | [Link](https://flipsky.net/products/mini-fsesc4-20-50a-with-anodized-heat-sink) | $56 |
| Battery — 6S 5000mAh 30C LiPo | [Link](https://www.amazon.com/OVONIC-Battery-5000mAh-Airplane-Helicopter/dp/B0CP5KHH46) | $95 |
| Charger — IMAX B6 LiPo Balance Charger | [Link](https://www.amazon.com/Battery-Charger-Balance-Discharger-Chargers/dp/B0G64JZ7JV) | $35 |
| Deck & Enclosure — Blank Longboard Deck | [Link](https://www.amazon.com/Blank-Graphic-Longboard-Deck-CRUISER/dp/B004H2SP5Y) | $35 |

## Electronics

| Item | Link | Price |
|---|---|---|
| Switch | — | — |
| ESP32 DevKit V1 | [Link](https://www.amazon.com/ESP-WROOM-32-Development-Microcontroller-Integrated-Compatible/dp/B08D5ZD528) | $17 |
| Arduino Nano | *already have* | — |
| NEO-6M GPS Module | [Link](https://www.amazon.com/YELUFT-GY-NEO6MV2-Control-Support-Raspberry/dp/B0F2DKWJ4J) | $11 |
| MPU-6050 IMU | [Link](https://www.amazon.com/dp/B00LP25V1A) | $12 |
| SSD1306 0.96" OLED | [Link](https://www.amazon.com/UCTRONICS-SSD1306-Self-Luminous-Display-Raspberry/dp/B072Q2X2LL) | $14 |
| On/Off Rocker Switch | [Link](https://www.amazon.com/DIYables-Rocker-Arduino-ESP8266-Raspberry/dp/B0CN8VRSRF) | — |
| MicroSD Card Module + Card | [Link](https://www.amazon.com/WWZMDiB-Adater-Module-Support-Arduino/dp/B0B779R5TZ) | $6 |
| IRLZ44N MOSFET (headlight) | [Link](https://www.amazon.com/ALLECIN-IRLZ44N-Transistors-IRLZ44NPBF-Mosfets/dp/B0CBKH4XGL) | $10 |
| 3.3V Passive Buzzer | [Link](https://www.amazon.com/Passive-Buzzer-Piezoelectric-Arduino-Raspberry/dp/B0DHGP95K4) | $7 |
| 4x4 Membrane Keypad | [Link](https://www.amazon.com/DIYmalls-Keypad-Membrane-Keyboard-Arduino/dp/B086Z1ZXNJ) | $8 |
| ZUDKSUY Hall-Effect Thumb Throttle | [Link](https://www.amazon.com/ZUDKSUY-Electric-Throttle-LeftRight-Replacement/dp/B0D6WB5JRM) | $10 |
| Assorted Resistors | [Link](https://www.amazon.com/Avelis-Resistors-Assortment-Precision-Electronic/dp/B0FNRD8B3X) | $5 |
| 12V LED Light Bar (headlight) | [Link](https://www.amazon.com/Lightboss-LED-Light-Off-Road-Waterproof/dp/B09YSWDXSY) | $9 |
| Hookup Wire Assortment | [Link](https://www.amazon.com/s?k=breadboard+jumper+wire+assortment) | $7 |
| Heat Shrink Tubing | [Link](https://www.amazon.com/Ginsco-580-pcs-Assorted-Sleeving/dp/B01MFA3OFA) | $8 |
| Solder | [Link](https://www.amazon.com/MAIYUM-63-37-Solder-Electrical-Soldering/dp/B075WB98FJ) | $9 |
| Grip Tape | [Link](https://www.amazon.com/Black-Diamond-Longboard-Skateboard-Sheet/dp/B0013T1NZS) | $12 |
| Step Down Buck/Boost Converter | [Link](https://www.amazon.com/Seloky-Converter-Adjustable-Electronic-Stabilizer/dp/B0DBVYP91F) | $6 |
| XT90-S Connectors *(need wired versions)* | [Link](https://www.amazon.com/FLY-RC-Connector-Anti-Spark-Adapter/dp/B0893CGLP7) | $13 |
| 12 AWG Wire (black & red) | — | — |
| UBEC | [Link](https://www.amazon.com/ShareGoo-Converter-Module-Quadcopter-Holder/dp/B07DYXTX9H) | $19 |
| Charging Lead | [Link](https://www.amazon.com/FLY-RC-Connector-11-8inch-Silicone/dp/B07C2DGM8D) | — |
| Dupont Connectors + Crimpers | — | — |

**Estimated subtotal (listed prices):** ~$729 


I'm probably going to cut the keypad and the toggle switch btw, they seemed kind of useless ICL.

Logging off!

# 6/30/26 - 3 Hours

## Wiring Reformatting

Ok so... I may have messed up the resistor values but I managed to fix them by looking at a lot of datasheets for MOFSET gates and documentation on how I should connnect an Arduino Nano to an ESP32 (you have to use them because arduino outputs 5v and esp32 only takes 3.3)

However, I fixed them and here is my final rewire, nothing too different from the old sheet but I'm quite happy about how clean this one looks :)

<img src = "https://cdn.hackclub.com/019f1ba8-6494-78ec-aa2a-8b537365bdf5/Smart%20Skateboard%20Wiring%20Diagram.drawio(1).png">

A little embarrassed how long this took cause its my first time using Draw.io (I was also very distracted by the World Cup my family was watching in the background) but hopefully tommorow I'll be making the circuit board. Peace!

## 7/1/26 - 4 Hours

Alrighty, today I turned my wiring diagrams into an actual, real schematic!

Here's the final product before I got into the process that went into it and what I had to change and stuff :)

<img src = "https://cdn.hackclub.com/019f1fa6-cb4b-7150-bc92-e3b2824ae3d5/schematic.png">

Sorry if the image is super low res I dont know how to properly export it from KiCAD.

Anyways, most of my time was basically spent rethinking the way that I'm connecting all these modules to the arduino to finalize the wiring.

I also did a very aggressive recheck of each place where resistors are needed and all of their nodes.


When designing this board I had to think about how I would integrate this circuitboard with the physical XT90 connector system that you can see in the ohter wiring diagram. I realized that I could basically take the wires of the ESC, UBEC, step down, and headlights, and screw them into wire terminals on the board which made things extremely easy to work with.

From there it was just a lot a lot of checking and reviewing part diagrams for modules (I'm so scared of accidently using a component that can't handle 5V, but like 99% of arduino modules take 3.3V-5V so I really shouldn't be worrying)
I also made my own Arduino Nano and ESP32 schematics from two 15x1 connector schematics so I could ensure each pin was correctly labeled. 

The next part was turning it into a PCB but I think I'm going to call it a day because I already chose all of the footprints (they're mostly through hole and screw terminals so assembly is extremely easy) so I'm going to delay it to tommorow. I am tired!!!!

I'm also going to do a final wiring/compatibility check before finalizing the PCB and figuring out the layout.

Then I need to worry about the case ;-; I'm really tied but I know I can put the work into finishing this design soon :3

Oh yeah, I used used screw terminals for the GPS and SD logger because I want to ziptie them to a specific part of the case (so I can easily take out the SD and put the GPS antenna in an optimal position) instead of having them tied to the PCB.

I hope to finish this PCB or a rough form of it tmmrw and get the case done as well so I can send in my S/X tier pitch. Then I'm going to order all of the parts and do a test wire with each module individually to ensure they all function and that I know how to program them.

From there I'll finalize my case design and tweak the PCB again to get my desired build which hopefully will be quick :))))))

I think I should also do a quick macropad to make sure I can at least bring *something* to Outpost if I can't finish in time (I think that's how it works?)

## 7/2/26 - 5 Hours

Currently contemplating my life choices because my motor won't get here by the time I have to submit...

Welp I finished my PCB and CAD at least


Heres what my mounting solution is going to be for the pcb

<img src="https://cdn.hackclub.com/019f2504-a272-7190-97ed-579479bf9502/Screenshot%202026-07-02%20184754.png">

Essentially I'm going to countersink M5 holes into the skateboard deck and 3D print spacers so I can put my PCB on it while velcroing a bunch of other components down to the main body of the deck (basically everything that isn't on the PCB that I don't want to be locked in placed)


To protect this I also modeld a shield which will be velcroed onto the main deck. Seen here:

<img src = "https://cdn.hackclub.com/019f2507-65c2-7137-9ee7-673963ea3eee/Screenshot%202026-07-02%20185056.png">

I cut out holes to glue on the XT90S spark key hole which I found a free 3D model for on thingiverse: 
https://www.thingiverse.com/thing:2323422/comments 

basically you can just slot it into the holes and superglue down the edges which is dandy :)


<img src = "https://cdn.hackclub.com/019f2508-aadb-7b29-8780-0e3aca4f722f/Screenshot%202026-07-02%20185056.png">

Additionally I finally figured out how to finalize my PCB

<img src = "https://cdn.hackclub.com/019f2509-905f-7cde-beeb-4838f69e2123/Screenshot%202026-07-02%20185322.png">

Isn't that a beauty. You can find the gerber zip in the KiCad folder of this repo. :)


That's pretty much the project design finished. Thanks for your time and I hope to update this journal againwhen I finish building the super duper awesome board.


I don't know if my motor will come in time for the 7/7 deadline so I might not be done by then and wow PCB shipping is expensive ;-; so I hope to at least have a working development board for this project before the deadline.


