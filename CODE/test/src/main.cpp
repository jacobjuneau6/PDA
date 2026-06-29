/**
 * ESP32-S3 PDA — Demo Sketch
 *
 * Hardware:
 *   - ST7789 240×320 TFT (software SPI)
 *   - MAX17048 LiPo fuel gauge (I2C)
 *   - WS2812 status LED
 *   - 3 navigation buttons (UP / SELECT / DOWN)
 *
 * Pin mapping (matches custom PDA board):
 *   GPIO1  = SDA  → SPI MOSI
 *   GPIO2  = SCL  → SPI SCLK
 *   GPIO3  = CS   → Chip select
 *   GPIO4  = DC   → Data/command
 *   GPIO5  = RST  → Reset
 *   GPIO6  = BL   → Backlight PWM
 *   GPIO7  = WS2812 data
 *   GPIO8  = I2C SCL
 *   GPIO9  = I2C SDA
 *   GPIO10 = Button UP
 *   GPIO11 = Button SELECT
 *   GPIO12 = Button DOWN
 */

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <Adafruit_MAX1704X.h>
#include <FastLED.h>

// ---------------------------------------------------------------------------
// Display — software SPI (matches user pin table)
// ---------------------------------------------------------------------------
#define TFT_MOSI   1    // SDA on silkscreen
#define TFT_SCLK   2    // SCL on silkscreen
#define TFT_CS     3
#define TFT_DC     4
#define TFT_RST    5
#define TFT_BL     6    // Backlight PWM

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

// ---------------------------------------------------------------------------
// I2C — MAX17048 fuel gauge
// ---------------------------------------------------------------------------
#define I2C_SDA    9
#define I2C_SCL    8

Adafruit_MAX17048 max17048;

// ---------------------------------------------------------------------------
// WS2812 status LED
// ---------------------------------------------------------------------------
#define LED_PIN    7
#define NUM_LEDS   1
CRGB leds[NUM_LEDS];

// ---------------------------------------------------------------------------
// Navigation buttons (active-low with pull-ups)
// ---------------------------------------------------------------------------
#define BTN_UP      10
#define BTN_SELECT  11
#define BTN_DOWN    12

// ---------------------------------------------------------------------------
// Backlight PWM
// ---------------------------------------------------------------------------
#define BL_CHANNEL  0       // LEDC channel
#define BL_FREQ     5000    // 5 kHz
#define BL_RES      8       // 8-bit resolution (0–255)

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

void backlightWrite(uint8_t duty) {
  ledcWrite(BL_CHANNEL, duty);
}

// ---------------------------------------------------------------------------
// SETUP
// ---------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  delay(500);  // let USB serial enumerate

  Serial.println(F("\n=== ESP32-S3 PDA — Demo ===\n"));

  // ---- Buttons ----
  pinMode(BTN_UP,     INPUT_PULLUP);
  pinMode(BTN_SELECT, INPUT_PULLUP);
  pinMode(BTN_DOWN,   INPUT_PULLUP);

  // ---- WS2812 ----
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
  leds[0] = CRGB::Blue;
  FastLED.show();

  // ---- Backlight PWM ----
  ledcSetup(BL_CHANNEL, BL_FREQ, BL_RES);
  ledcAttachPin(TFT_BL, BL_CHANNEL);
  backlightWrite(128);  // 50 % brightness at boot

  // ---- I2C + MAX17048 ----
  Wire.begin(I2C_SDA, I2C_SCL);
  if (!max17048.begin()) {
    Serial.println(F("⚠ MAX17048 not detected — check I2C wiring"));
  } else {
    Serial.println(F("✓ MAX17048 OK"));
  }

  // ---- Display ----
  tft.init(240, 320);                 // 240 wide × 320 tall
  tft.setRotation(1);                 // landscape (USB port right)
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);
  tft.setTextWrap(true);

  tft.setCursor(10, 10);
  tft.println("ESP32-S3 PDA");
  tft.println("Boot OK");

  // Hardware summary
  tft.setTextSize(1);
  tft.setCursor(10, 70);
  tft.println("ST7789 240x320 TFT");
  tft.setCursor(10, 82);
  tft.println("MAX17048 fuel gauge");
  tft.setCursor(10, 94);
  tft.println("WS2812 status LED");
  tft.setCursor(10, 106);
  tft.println("GPIO1-6  display bus");
  tft.setCursor(10, 118);
  tft.println("GPIO8-9  I2C bus");

  backlightWrite(255);  // full brightness
  delay(1500);
}

// ---------------------------------------------------------------------------
// LOOP
// ---------------------------------------------------------------------------
void loop() {
  // ---- Read buttons (active-low → invert) ----
  bool up   = !digitalRead(BTN_UP);
  bool sel  = !digitalRead(BTN_SELECT);
  bool down = !digitalRead(BTN_DOWN);

  // ---- Battery ----
  float voltage = 0.0f;
  float soc     = 0.0f;
  bool  gaugeOk = max17048.begin();  // true if already init'd

  if (gaugeOk) {
    voltage = max17048.cellVoltage();
    soc     = max17048.cellPercent();
  }

  // ---- Clear info area ----
  tft.fillRect(0, 55, 240, 100, ST77XX_BLACK);

  // ---- Voltage & SOC ----
  tft.setTextSize(2);
  tft.setCursor(10, 60);
  if (gaugeOk) {
    tft.print("V:  ");
    tft.print(voltage, 2);
    tft.print(" V");

    tft.setCursor(10, 85);
    tft.print("SOC: ");
    tft.print(soc, 1);
    tft.print(" %");
  } else {
    tft.print("MAX17048 N/C");
  }

  // ---- Button state ----
  tft.setCursor(10, 115);
  tft.print("BTN: ");
  if (up)   tft.print("UP ");
  if (sel)  tft.print("SEL ");
  if (down) tft.print("DN ");
  if (!up && !sel && !down) tft.print("—");

  // ---- WS2812 colour by button ----
  if (sel) {
    leds[0] = CRGB::Green;
  } else if (up) {
    leds[0] = CRGB::Red;
  } else if (down) {
    leds[0] = CRGB::Blue;
  } else {
    leds[0] = CRGB::Purple;
  }
  FastLED.show();

  // ---- Serial debug ----
  Serial.printf("V=%.2f  SOC=%.1f%%  UP=%d SEL=%d DN=%d\n",
                voltage, soc, up, sel, down);

  delay(150);
}
