# ESP32-S3 PDA — Demo Firmware

Minimal Arduino demo for the custom **ESP32-S3 PDA** board. Drives the ST7789 display, reads the MAX17048 battery gauge, lights the WS2812 status LED, and responds to the three navigation buttons.

## Hardware

| Silkscreen | GPIO | Function               |
|-----------|------|------------------------|
| SCL       | 2    | SPI SCLK (TFT clock)   |
| SDA       | 1    | SPI MOSI (TFT data)    |
| CS        | 3    | TFT chip select        |
| DC        | 4    | TFT data/command       |
| RST       | 5    | TFT reset              |
| BL        | 6    | TFT backlight PWM      |
| —         | 7    | WS2812 status LED      |
| —         | 8    | I2C SCL (MAX17048)     |
| —         | 9    | I2C SDA (MAX17048)     |
| —         | 10   | Button UP              |
| —         | 11   | Button SELECT          |
| —         | 12   | Button DOWN            |

**Display:** ST7789 240×320 TFT, 1.54″ or 1.69″  
**Fuel gauge:** MAX17048 (I²C addr 0x36)  
**LED:** WS2812/NeoPixel (single)

## Project Structure

```
test/
├── platformio.ini    # PlatformIO build config
├── src/
│   └── main.cpp      # Demo sketch
└── README.md
```

## Build & Flash

### Prerequisites

1. Install [PlatformIO](https://platformio.org/install) (VS Code extension or CLI).
2. Connect the ESP32-S3 board via USB.

```bash
# Clone / enter the project
cd test

# Build
pio run

# Flash & open serial monitor
pio run --target upload --target monitor
```

### Settings

| Setting          | Value      |
|-----------------|------------|
| Board           | ESP32-S3 DevKitC-1 |
| Framework       | Arduino    |
| Monitor baud    | 115200     |

## What the Demo Does

1. **Boot splash** — "ESP32-S3 PDA / Boot OK" on the TFT, WS2812 glows blue.
2. **Battery readout** — displays cell voltage and state-of-charge (updates ~6×/sec).
3. **Button indicators** — shows which button is pressed on the TFT.
4. **LED feedback** — WS2812 changes colour:
   - Purple = idle
   - Green = SELECT
   - Red = UP
   - Blue = DOWN
5. **Serial debug** — prints voltage, SOC, and button states at 115200 baud.

## Libraries Used

| Library                 | Version |
|------------------------|---------|
| Adafruit GFX           | ≥1.11   |
| Adafruit ST7735/ST7789 | ≥1.10   |
| Adafruit MAX1704X      | ≥1.0    |
| FastLED                | ≥3.7    |

All are fetched automatically by PlatformIO (see `platformio.ini` → `lib_deps`).

## Customisation

- **Backlight level** — call `backlightWrite(0..255)` in `loop()`, e.g. to dim after inactivity.
- **Rotation** — change `tft.setRotation(1)` (0–3) if your panel is mounted differently.
- **I²C pins** — edit `I2C_SDA` / `I2C_SCL` at the top of `main.cpp`.
