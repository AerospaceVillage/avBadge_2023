# Programming the Badges

The badge programming pinout was arbitrarily designed before purchasing and becoming
aware of the [ESP-Prog Board](https://media.digikey.com/pdf/Data%20Sheets/Espressif%20PDFs/Intro_to_the_ESP-Prog_Brd_Web.pdf)

In hindsight the badge pinout would have been changed to match that of the ESP-Prog, but there is no
time-machine to fix that. That that end a custom wire harness has to be built to remap the pins
of the ESP-Prog to that of the boards (fortunately the Wright Flyer and Wright Stuff use the same incorrect pinout).

## Make a custom cable

On the ESP-Prog board the pins are as follows (pin #3 is closest to the keyed portion)

```
ESP-Prog Side
  -----     1. EN
| 1  2 |    2. VBus
  3  4 |    3. TX
| 5  6 |    4. GND
  -----     5. RX
            6. Boot
```

```
AV Board Side - putting the key towards the outbound edge
  -----     1. TX
| 1  2 |    2. RX
  3  4 |    3. GND
| 5  6 |    4. VBus
  -----     5. Boot
            6. EN
```

It is then necessary to cut the 6 ribbon wire provided with the ESP-Prog board and then re-solder the 6 wires together so that they are mapped correctly from the ESP-Prog to the board being programmed.

* Take note to label one or both sides of the modified cable ribbon as it will be important to connect the respective end to the correct side (i.e. ESP-Prog side of the cable to the ESP-Prog)


## Pogo programming Custom Cable

If you are using the [Sparkfun ISP Pogo Adaptor](https://www.sparkfun.com/products/11591) to program the boards a similar mapping of wires will need to be accomplished. Consider using the smaller pitch size ribbon cable to modify such that one end goes to the ESP-Prog and the other is connected to the Pogo Adaptor with the following color mapping. When done this way the boards can be programmed in the orientation of the cables going **away** from the board being programmed.

```
Pogo Adaptor - with the provided colored adaptor

Yellow - TX
Black  - RX
Green  - GND
Red    - VBus
Blue   - Boot
Orange - EN
```
