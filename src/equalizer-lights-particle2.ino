#include <neopixel.h>
#include <Adafruit_SSD1306_RK.h>
#include <Adafruit_GFX_RK.h>

#define NUM_STRIPS 7
#define numberLEDs 130
#define maxVolume 2000
#define minVolume 300
#define oledBarWidth 16
#define LEDType WS2812B
#define msgReset A1
#define msgStrobe A2
#define msgRead A0
#define middleLED 65

const int ledPins[NUM_STRIPS] = {D8, D6, D5, D4, D3, D2, A3};

// Updated config string with new variables (indices 20 and 21)
String config = "5,5,5,5,5,5,5,"  // Lower bounds 0-6
                "50,50,50,50,50,50,50,"  // Upper bounds 7-13
                "1,254,1,"  // RGB 14-16
                "50,"  // Brightness 17
                "1,1,"  // Color mode 18, Shape mode 19
                "30000,"  // Noise floor duration in ms (20)
                "5";  // Noise floor offset percent (21)

//5,5,5,5,5,5,5,50,50,50,50,50,50,50,1,254,1,50,2,1,30000,5";

// Configuration variables
float lowerBounds[NUM_STRIPS];
float upperBounds[NUM_STRIPS];
int red;
int green;
int blue;
int brightness;
int colorMode;
int shapeMode;

// Updated configArray to float to accommodate floating-point numbers
float configArray[22];  // Indices 0 to 21

// Arrays to store the minimum signal values per band
float minSignal[NUM_STRIPS];

// Time tracking variables for noise floor
unsigned long minSignalUpdateTime = 0;
unsigned long minSignalInterval = 30000;  // Default 30 seconds, can be updated via config

float noiseFloorOffsetPercent = 5.0;  // Default 5%, can be updated via config

// Arrays to store the light values and net signals
float lightValues[NUM_STRIPS];
float netSignals[NUM_STRIPS];
float percentFill[NUM_STRIPS];

// Fire mode heat map
uint8_t fireHeat[NUM_STRIPS][numberLEDs];

// Strobe mode state
unsigned long lastFlashTime = 0;
bool isFlashing = false;

//for rainbow color modes
uint16_t hue = 0;
uint16_t stripHues[NUM_STRIPS] = {0, 40, 80, 120, 160, 200, 240}; // For 7 strips, closely spaced hues

int loopNum = 0;

Adafruit_NeoPixel strips[NUM_STRIPS] = {
    Adafruit_NeoPixel(numberLEDs, ledPins[0], LEDType),
    Adafruit_NeoPixel(numberLEDs, ledPins[1], LEDType),
    Adafruit_NeoPixel(numberLEDs, ledPins[2], LEDType),
    Adafruit_NeoPixel(numberLEDs, ledPins[3], LEDType),
    Adafruit_NeoPixel(numberLEDs, ledPins[4], LEDType),
    Adafruit_NeoPixel(numberLEDs, ledPins[5], LEDType),
    Adafruit_NeoPixel(numberLEDs, ledPins[6], LEDType)
};

Adafruit_SSD1306 display(128,32, &Wire, -1);

int turnOnLED(String args) {
    digitalWrite(D7, args.toInt());
    if (digitalRead(D7) == HIGH) {
        return 1;
    } else if (digitalRead(D7) == LOW) {
        return 0;
    } else {
        return 0;
    }
}

int updateConfigVars(String args) {
    Serial.println(args);

    int configVar = 0;
    int startIndex = 0;
    int commaIndex = args.indexOf(',');

    // Parse the config string into configArray
    while (commaIndex != -1 && configVar < 22) {
        String part = args.substring(startIndex, commaIndex);
        configArray[configVar] = part.toFloat();
        configVar++;
        startIndex = commaIndex + 1;
        commaIndex = args.indexOf(',', startIndex);
    }
    // Get the last value after the last comma
    if (configVar < 22) {
        String part = args.substring(startIndex);
        configArray[configVar] = part.toFloat();
        configVar++;
    }

    // Assign configuration variables
    for (int i = 0; i < NUM_STRIPS; i++) {
        lowerBounds[i] = configArray[i];
        upperBounds[i] = configArray[i + 7];
    }

    red = (int)configArray[14];
    green = (int)configArray[15];
    blue = (int)configArray[16];
    brightness = (int)configArray[17];
    colorMode = (int)configArray[18];
    shapeMode = (int)configArray[19];

    // Assign new configuration variables
    minSignalInterval = (unsigned long)configArray[20];  // Duration in milliseconds
    noiseFloorOffsetPercent = configArray[21];  // Noise floor offset percentage

    // Rebuild config string to reflect current active values
    config = "";
    for (int i = 0; i < 22; i++) {
        config += String((int)configArray[i]);
        if (i < 21) config += ",";
    }

    return 1;
}

void setup() {
    for (int i = 0; i < NUM_STRIPS; i++) {
        pinMode(ledPins[i], OUTPUT);
    }

    pinMode(msgReset, OUTPUT);
    pinMode(msgStrobe, OUTPUT);

    updateConfigVars(config);
    Serial.begin(9600);

    Particle.function("LEDState1", turnOnLED);
    Particle.function("UpdateConfig", updateConfigVars);
    Particle.variable("config", config);

    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {  // Address 0x3C for 128x32
        Serial.println(F("SSD1306 allocation failed"));
    } else {
        Serial.println("Screen initialized successfully");
    }
    delay(500);

    // Clear the buffer
    display.clearDisplay();

    // Draw a single pixel in white
    display.drawPixel(10, 10, 1);

    // Show the display buffer on the screen
    display.display();

    for (int i = 0; i < NUM_STRIPS; i++) {
        strips[i].begin();
        strips[i].show();
    }

    // Initialize minSignal[] to maximum ADC value
    for (int i = 0; i < NUM_STRIPS; i++) {
        minSignal[i] = 4095.0;
    }

    minSignalUpdateTime = millis();  
}
void loop() {
    digitalWrite(msgReset, HIGH);
    delay(1);
    digitalWrite(msgReset, LOW);

    display.clearDisplay();

    // Read the MSGEQ7 values into lightValues array
    for (int x = 0; x < 7; x++) {
        digitalWrite(msgStrobe, LOW);
        delayMicroseconds(35);
        int currentVal = analogRead(msgRead);
        lightValues[x] = currentVal;

        // Band 2 (index 1) has a hardware fault — override with average of adjacent bands
        if (x == 6) {
            lightValues[1] = (lightValues[0] + lightValues[2]) / 2.0;
        }

        digitalWrite(msgStrobe, HIGH);
        delayMicroseconds(35);
    }

    // Update the minimum signal values
    for (int i = 0; i < NUM_STRIPS; i++) {
        if (lightValues[i] < minSignal[i]) {
            minSignal[i] = lightValues[i];
        }
    }

    // Reset the minimum signal values every minSignalInterval milliseconds
    if (millis() - minSignalUpdateTime >= minSignalInterval) {
        for (int i = 0; i < NUM_STRIPS; i++) {
            minSignal[i] = 4095.0;  // Reset to maximum ADC value
        }
        minSignalUpdateTime = millis();
    }

    // Calculate noise floor as minSignal[i] plus noiseFloorOffsetPercent
    float noiseFloor[NUM_STRIPS];
    for (int i = 0; i < NUM_STRIPS; i++) {
        noiseFloor[i] = minSignal[i] * (1 + noiseFloorOffsetPercent / 100.0);
        if (noiseFloor[i] > 4095.0) {
            noiseFloor[i] = 4095.0;
        }
    }

    // Calculate net signals
    for (int i = 0; i < NUM_STRIPS; i++) {
        netSignals[i] = lightValues[i] - noiseFloor[i];
        if (netSignals[i] < 0) {
            netSignals[i] = 0;
        }
    }

    // Clear the LED strips
    for (int i = 0; i < NUM_STRIPS; i++) {
        strips[i].clear();
    }

    // Scale lower and upper bounds, calculate percent fill
    float lowerScaled[NUM_STRIPS];
    float upperScaled[NUM_STRIPS];
    for (int i = 0; i < NUM_STRIPS; i++) {
        lowerScaled[i] = (lowerBounds[i] * 4095.0) / 100.0;
        upperScaled[i] = (upperBounds[i] * 4095.0) / 100.0;
        percentFill[i] = 100.0 * (netSignals[i] - lowerScaled[i]) / (upperScaled[i] - lowerScaled[i]);
        percentFill[i] = constrain(percentFill[i], 0.0, 100.0);
    }

    updateStripColors();
    if (shapeMode == 2) {
        for (int j = 0; j < numberLEDs; j++) {
            float threshold = (100.0 * j) / numberLEDs;
            for (int i = 0; i < NUM_STRIPS; i++) {
                if (percentFill[i] > threshold) {
                    if (colorMode == 1) {
                        strips[i].setColorDimmed(j, red, green, blue, brightness);
                    }
                    // Rainbow modes: color already set by updateStripColors(), leave it
                } else {
                    strips[i].setPixelColor(j, 0);  // Black out inactive LEDs
                }
            }
        }

    } else if (shapeMode == 1) {
        for (int j = 0; j < numberLEDs; j++) {
            for (int i = 0; i < NUM_STRIPS; i++) {
                if (abs(j - 55) < (numberLEDs * (percentFill[i] / 2.0) / 100.0) || j == 55) {
                    if (colorMode == 1) {
                        strips[i].setColorDimmed(j, red, green, blue, brightness);
                    }
                } else {
                    strips[i].setPixelColor(j, 0);
                }
            }
        }
        int middleIndex = 55;
        for (int i = 0; i < NUM_STRIPS; i++) {
            if (colorMode == 1) {
                strips[i].setColorDimmed(middleIndex, red, green, blue, brightness);
            }
        }
    }

    for (int i = 0; i < NUM_STRIPS; i++) {
        strips[i].show();
    }
}

void updateStripColors() {
    // Turn all LEDs off by default
    uint32_t offColor = strips[0].Color(0, 0, 0);
    for (int i = 0; i < numberLEDs; i++) {
        for (int s = 0; s < NUM_STRIPS; s++) {
            strips[s].setPixelColor(i, offColor);
        }
    }

    switch (colorMode) {
        case 1:
            // Solid color mode: Don't set a baseline here.
            // The main loop logic will light specific LEDs as needed.
            break;

        case 2:
            // Rainbow cycle (all strips same color)
            // Increment hue for the rainbow effect and set a baseline rainbow color
            hue++;
            {
                uint32_t rainbowColor = applyBrightness(hsvToRgb((uint8_t)(hue / 4), 255, 255));
                for (int i = 0; i < numberLEDs; i++) {
                    for (int s = 0; s < NUM_STRIPS; s++) {
                        strips[s].setPixelColor(i, rainbowColor);
                    }
                }
            }
            break;

        case 3:
            // Rainbow cycle (each strip different color)
            // Apply a per-strip rainbow
            for (int i = 0; i < numberLEDs; i++) {
                for (int s = 0; s < NUM_STRIPS; s++) {
                    strips[s].setPixelColor(i, applyBrightness(hsvToRgb((uint8_t)(stripHues[s] / 4), 255, 255)));
                }
            }
            for (int i = 0; i < NUM_STRIPS; i++) {
                stripHues[i]++;
            }
            break;

        case 4:
            // Fire/lava mode — tinted by configured RGB color
            for (int s = 0; s < NUM_STRIPS; s++) {
                // Cool down every cell slightly
                for (int j = 0; j < numberLEDs; j++) {
                    uint8_t cooldown = random(0, 3);
                    fireHeat[s][j] = (fireHeat[s][j] > cooldown) ? fireHeat[s][j] - cooldown : 0;
                }

                // Drift heat upward
                for (int j = numberLEDs - 1; j >= 2; j--) {
                    fireHeat[s][j] = (fireHeat[s][j - 1] + fireHeat[s][j - 2]) / 2;
                }

                // Ignite new sparks near the bottom, intensity based on audio
                int numSparks = (int)(percentFill[s] / 20.0) + 1;
                for (int k = 0; k < numSparks; k++) {
                    int pos = random(0, 8);
                    int sparkHeat = (int)(percentFill[s] * 2.55);  // 0-255 based on fill
                    fireHeat[s][pos] = constrain(fireHeat[s][pos] + sparkHeat, 0, 255);
                }

                // Map heat to color: blend from black -> base RGB -> white/yellow
                for (int j = 0; j < numberLEDs; j++) {
                    uint8_t heat = fireHeat[s][j];
                    uint8_t r, g, b;
                    if (heat < 128) {
                        // Low heat: scale base RGB by heat
                        float scale = heat / 128.0;
                        r = (uint8_t)(red * scale);
                        g = (uint8_t)(green * scale);
                        b = (uint8_t)(blue * scale);
                    } else {
                        // High heat: blend from base RGB toward yellow/white
                        float blend = (heat - 128) / 127.0;
                        r = red + (uint8_t)((255 - red) * blend);
                        g = green + (uint8_t)((255 - green) * blend);
                        b = blue + (uint8_t)((min(blue + 30, 255) - blue) * blend);
                    }
                    r = (r * brightness) / 255;
                    g = (g * brightness) / 255;
                    b = (b * brightness) / 255;
                    strips[s].setPixelColor(j, strips[0].Color(r, g, b));
                }
            }
            break;

        case 5:
            // Strobe/flash mode — white flash when any band peaks
            {
                // Check if any band is near peak
                bool peakDetected = false;
                for (int i = 0; i < NUM_STRIPS; i++) {
                    if (percentFill[i] >= 90.0) {
                        peakDetected = true;
                        break;
                    }
                }

                if (peakDetected) {
                    isFlashing = true;
                    lastFlashTime = millis();
                }

                if (isFlashing && (millis() - lastFlashTime < 50)) {
                    // White flash at full brightness
                    uint32_t flashColor = strips[0].Color(brightness, brightness, brightness);
                    for (int i = 0; i < numberLEDs; i++) {
                        for (int s = 0; s < NUM_STRIPS; s++) {
                            strips[s].setPixelColor(i, flashColor);
                        }
                    }
                } else {
                    isFlashing = false;
                    // Base color with brightness applied
                    uint8_t br = (red * brightness) / 255;
                    uint8_t bg = (green * brightness) / 255;
                    uint8_t bb = (blue * brightness) / 255;
                    uint32_t baseColor = strips[0].Color(br, bg, bb);
                    for (int i = 0; i < numberLEDs; i++) {
                        for (int s = 0; s < NUM_STRIPS; s++) {
                            strips[s].setPixelColor(i, baseColor);
                        }
                    }
                }
            }
            break;

        case 6:
            // Gradient along strip — full rainbow mapped across strip length
            hue++;
            for (int j = 0; j < numberLEDs; j++) {
                uint8_t pixelHue = (uint8_t)((hue / 4) + (j * 256 / numberLEDs));
                uint32_t color = applyBrightness(hsvToRgb(pixelHue, 255, 255));
                for (int s = 0; s < NUM_STRIPS; s++) {
                    strips[s].setPixelColor(j, color);
                }
            }
            break;

        default:
            // Default mode: leave LEDs off
            break;
    }
}



uint32_t applyBrightness(uint32_t color) {
  if (color == 0) return 0; // Ensure off pixels stay off

  uint8_t r = (color >> 16) & 0xFF;
  uint8_t g = (color >> 8) & 0xFF;
  uint8_t b = color & 0xFF;
  r = (r * brightness) / 255;
  g = (g * brightness) / 255;
  b = (b * brightness) / 255;
  return strips[0].Color(r, g, b);
}

uint32_t hsvToRgb(uint8_t h, uint8_t s, uint8_t v) {
  uint8_t r, g, b;

  uint8_t region = h / 43;
  uint8_t remainder = (h - (region * 43)) * 6;

  uint8_t p = (v * (255 - s)) >> 8;
  uint8_t q = (v * (255 - ((s * remainder) >> 8))) >> 8;
  uint8_t t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;

  switch (region) {
    case 0: r = v; g = t; b = p; break;
    case 1: r = q; g = v; b = p; break;
    case 2: r = p; g = v; b = t; break;
    case 3: r = p; g = q; b = v; break;
    case 4: r = t; g = p; b = v; break;
    default: r = v; g = p; b = q; break;
  }

  return strips[0].Color(r, g, b);
}
