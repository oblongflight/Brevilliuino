## Intro
Gaggiuino started as an idea to improve the Gaggia Classic and Classic Pro while keeping the machine appearance and button functionality as close as possible to the original. The Brevilliuino project adapts this codebase to the Breville Barista Express, with modifications to account for the differences in solenoid behavior, thermoblock characteristics, and steaming. This system works with Gaggiuino V2 only, the last release for which source code is available. The current codebase is specific to [GaggiaBoard_V3](https://github.com/banoz/CoffeeHat/tree/a1cdccbc4df707b2967d31b3c6e985c20c3fda71/Hardware/GaggiaBoard_V3) but could be adapted to a LEGO build or another PCB.

## Differences from Gaggiuino
- The Breville Barista Express uses a pair of solenoids (one three-way valve and one two-way valve) and a manual switch to direct flow to the steam wand, hot water outlet, grouphead, or drip tray. It has a thermoblock for heating instead of a boiler.
- Because of the location of the pressure transducer (in place of the stock analog pressure gauge), no pressure measurement is available except when flow is directed to the grouphead. 
- Steam and hot water modes are activated with a manual knob on the side of the unit which actuates a physical valve and two reed switches. Turning the knob to Steam directs water flow to the steam wand and activates Steam mode (pin PC15), and turning the knob to Hot Water directs water flow to the hot water outlet and activates Hot Water mode (pin PB15). Steaming is accomplished by pulsing the pump slowly (at a rate determined by the value of b.Div) and heating the thermoblock to 125-145C. Steam heating power is determined by the value of m.Div. These values are set in the Settings page of the Nextion screen. 
- The steam wand can be flushed by pressing the Brew button while steam mode is activated. 
- The Brew button (pin PC14) is now handled by the EasyButton library (because I'm not a real programmer); in the original code it was activated by a toggle switch but the Breville control panel switches are momentary. 
- During brewing, the 3WV (three-way valve solenoid) is energized to allow flow to the grouphead; the grouphead pressure is released after brewing via the 2WV (two-way valve solenoid), via a relay actuated by pin PA8.

![plumbing](/Plumbing.jpg)

## Other Notes
The entire system is powered by a relay controlled by an ESP32-S3 which also handles the grinder motor, control panel lights, and the grind scale. It interfaces with Google Home via SinricPro so that I can preheat the thermoblock and (once I integrate it) grind my coffee from bed. The code for this is available [here](https://github.com/oblongflight/Brevilliuino-Grinder).

## Unaddressed Issues
The temperature control has not been fully tuned yet and we are probably over- or undershooting the desired temperature in different flow states due to the location of the thermocouple and the ham-handed way I'm currently handling heating. The endgame is probably a flow-adjusted heating profile but I haven't gotten around to it yet and the coffee tastes good so far.
