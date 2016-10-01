# esp_lightingnado

> I watched Sharknado 4 times yesterday so, for some reason, lightingnado came to mind for a project name.

## Pins

I'm using an ESP12E DEVKIT V2 (ESP + USB programmer board) so my pins correspond to that:

* **RESET (D7)** - Momentarily grounding this pin within the first couple seconds of startup resets the configuration and enables the captive configuration portal
* **LET DATA (RXA)** - This pin is used for the data signal for the WS2812 LED strip.
