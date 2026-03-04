# Equalizer Lights

A hobby project that turns music into a real-time LED light show using a Particle Argon.

Seven NeoPixel LED strips each represent a frequency band from an MSGEQ7 graphic equalizer IC. Bass lights up one strip, treble lights up another, and everything in between gets its own channel.

## Hardware

- Particle Argon
- MSGEQ7 7-band graphic equalizer IC
- 7x WS2812B NeoPixel LED strips (130 LEDs each)
- SSD1306 128x32 OLED display (I2C)

## Features

- **3 color modes**: solid RGB, synchronized rainbow, per-strip rainbow
- **2 shape modes**: center-out expansion or end-to-end fill
- **Adaptive noise floor**: automatically adjusts to ambient noise levels
- **Remote configuration**: all parameters (sensitivity, color, brightness, mode) adjustable via Particle cloud functions

## Cloud API

| Function | Description |
|---|---|
| `LEDState1` | Toggle the onboard D7 LED (0 or 1) |
| `UpdateConfig` | Update all parameters via a 22-value comma-separated string |

### Config string format

```
lowerBounds[7], upperBounds[7], R, G, B, brightness, colorMode, shapeMode, noiseFloorDuration, noiseFloorOffset
```

Example: `5,5,5,5,5,5,5,50,50,50,50,50,50,50,1,254,1,50,1,1,30000,5`

## Building

This is a Particle project. Flash using the [Particle CLI](https://docs.particle.io/getting-started/developer-tools/cli/) or [Particle Workbench](https://www.particle.io/workbench/):

```
particle flash <device-name> .
```
