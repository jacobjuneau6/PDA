# ESP32-S3 PDA

A simple ESP32-S3 based phone companion and pocket computer. This open-source hardware project includes a custom PCB, 3D-printable case, and Arduino firmware — everything you need to build a portable device with a color TFT display, LiPo battery management, and physical navigation buttons.

## Features

- **ESP32-S3-WROOM-1-N16R8** module (16 MB flash, 8 MB PSRAM)
- **ST7789 240×320 TFT** color display with PWM backlight control
- **MAX17048** LiPo fuel gauge for accurate battery state-of-charge
- **MCP73831** Li-Ion/Li-Po battery charger (charges via USB)
- **WS2812/NeoPixel** RGB status LED
- **3 navigation buttons** (UP, SELECT, DOWN) with active-low pull-ups
- **USB micro-B** connector for power and data
- **2-layer PCB** with Gerber files ready for fabrication
- **3D-printable case** designed in FreeCAD

## Pinout

| Silkscreen | GPIO | Function |
|-----------|------|----------|
| SCL | 2 | SPI SCLK (TFT clock) |
| SDA | 1 | SPI MOSI (TFT data) |
| CS | 3 | TFT chip select |
| DC | 4 | TFT data/command |
| RST | 5 | TFT reset |
| BL | 6 | TFT backlight PWM |
| — | 7 | WS2812 status LED (NeoPixel) |
| — | 8 | I2C SCL (MAX17048 fuel gauge) |
| — | 9 | I2C SDA (MAX17048 fuel gauge) |
| — | 10 | Button UP |
| — | 11 | Button SELECT |
| — | 12 | Button DOWN |

**Display connector:** 1×08 pin header (GND, VCC, SCL, SDA, RST, DC, CS, BL) for ST7789 240×320 TFT, 1.54″ or 1.69″ diagonal.

**I²C address:** MAX17048 at `0x36`.

## Bill of Materials

| Reference            | Qty | Part                    | DigiKey P/N              | Description                                                | Price  |
|---------------------|-----|-------------------------|--------------------------|------------------------------------------------------------|--------|
| J1                  | 1   | RAHMB20AUETR            | 137-RAHMB20AUETRCT-ND    | USB micro-B receptacle, right-angle, SMT (Switchcraft)     |        |
| LED1                | 1   | COM-16347               | 568-16347CT-ND           | WS2812/NeoPixel RGB LED                                    |        |
| R1, R2              | 2   | RC1206FR-132KL          | 13-RC1206FR-132KLCT-ND   | 2 kΩ 1206 thick-film resistor (Yageo)                      |        |
| SW1, SW2, SW3, SW4  | 4   | B3F-1002                | SW401-ND                 | Tactile switch (Omron)                                      |        |
| U1                  | 1   | ESP32-S3-WROOM-1-N16R8  | 5407-ESP32-S3-WROOM-1-N16R8CT-ND | ESP32-S3 module, 16 MB flash, 8 MB PSRAM        |        |
| U2                  | 1   | MCP73831T-2DCI/MC       | MCP73831T-2DCI/MCCT-ND   | Li-Ion/Li-Po battery charger IC (Microchip)               |        |
| U3                  | 1   | TPS74533PCQWDRVRQ1      | 296-TPS74533PCQWDRVRQ1CT-ND | 3.3 V LDO voltage regulator, 500 mA (TI)              |        |
| U4                  | 1   | MAX17048G+T10           | MAX17048G+T10CT-ND       | LiPo fuel gauge IC (Maxim Integrated)                      |        |
| U5                  | 1   | AO3400A                 | 785-1000-1-ND            | N-channel MOSFET, SOT-23 (battery protection)              |        |
| —                   | 1   | Custom PCB & stencil     | JLCPCB                   | Fabrication                                                | $20.00 |
| —                   | 1   | DigiKey components      |                          | Components & shipping                                      | $30.00 |
| —                   | 1   | 1Ah battery             |                          | https://www.amazon.com/dp/B0DPZVBKMY?th=1                  | $8.00  |
| —                   | 1   | Display                 |                          | https://www.amazon.com/gp/product/B0DFWLD38D/ref=ox_sc_act_title_2?smid=A2UYBS1CH53PM5 | $16.00 |


## Power Architecture

```
USB VBUS (5 V)
  ├── MCP73831 charger (U2) ──→ VBAT (battery voltage)
  │     ├── AO3400A MOSFET (U5) — battery protection
  │     └── MAX17048 fuel gauge (U4) — I²C monitoring
  │
  └── TPS74533 LDO (U3) ──→ 3.3 V rail
        ├── ESP32-S3 module (U1)
        ├── Display backlight (PWM on GPIO6)
        └── Peripherals (WS2812, buttons, I²C pull-ups)
```

- The device charges a single-cell LiPo battery via USB at 4.2 V termination.
- Battery voltage and state-of-charge are monitored by the MAX17048 over I²C.
- The 3.3 V LDO powers the MCU and peripherals from either USB or battery.

## Repository Structure

```
PDA/
├── README.md              # This file
├── BOM.csv                # Bill of Materials with DigiKey part numbers
├── Journal.md             # Design journal — case design notes & images
├── LICENSE                # MIT License
│
├── board/                 # KiCad PCB design
│   ├── board.kicad_sch    # Schematic
│   ├── board.kicad_pcb    # PCB layout
│   ├── board.kicad_pro    # Project settings
│   ├── gbrs/              # Gerber files (ready for fabrication)
│   └── staging/           # Vendor symbols, footprints, and 3D models
│
├── CAD/                   # FreeCAD mechanical design
│   ├── Assembly.FCStd     # Full assembly (PCB + case + battery + display)
│   ├── case.FCStd         # 3D-printable enclosure
│   ├── battery.FCStd      # Dummy battery model (fit check)
│   ├── display.FCStd      # Dummy display model (fit check)
│   └── PCB.step           # PCB STEP export
│
└── CODE/                  # Firmware
    └── test/
        ├── platformio.ini # PlatformIO build configuration
        └── src/
            └── main.cpp   # Arduino demo sketch
```

## Firmware

The demo firmware exercises all major subsystems: display, battery gauge, status LED, and buttons.

### Build & Flash

**Prerequisites:** [PlatformIO](https://platformio.org/install) (VS Code extension or CLI).

```bash
cd CODE/test

# Build
pio run

# Flash and open serial monitor (115200 baud)
pio run --target upload --target monitor
```

### What the Demo Does

1. **Boot splash** — "ESP32-S3 PDA / Boot OK" on the TFT, WS2812 glows blue
2. **Battery readout** — cell voltage and state-of-charge, updates ~6× per second
3. **Button indicators** — shows pressed button (UP/SELECT/DOWN) on the TFT
4. **LED feedback** — WS2812 colour changes:
   - Purple = idle
   - Green = SELECT
   - Red = UP
   - Blue = DOWN
5. **Serial debug** — prints voltage, SOC, and button state at 115200 baud

### Libraries

| Library | Version |
|---------|---------|
| Adafruit GFX | ≥1.11 |
| Adafruit ST7735/ST7789 | ≥1.10 |
| Adafruit MAX1704X | ≥1.0 |
| FastLED | ≥3.7 |

All are fetched automatically by PlatformIO via `lib_deps`.

### Board Configuration

| Setting | Value |
|---------|-------|
| Board | ESP32-S3 DevKitC-1 |
| Framework | Arduino |
| Monitor baud | 115200 |

### Customisation

- **Backlight level** — call `backlightWrite(0..255)` in `loop()` (e.g., dim after inactivity)
- **Rotation** — change `tft.setRotation(1)` (0–3) if your panel is mounted differently
- **I²C pins** — edit `I2C_SDA` and `I2C_SCL` at the top of `main.cpp`

## PCB

- **Layers:** 2-layer board
- **Software:** KiCad 8
- **Gerber files:** Ready for fabrication in [`board/gbrs/`](board/gbrs/)

The PCB uses all surface-mount components. Most ICs are in small-outline packages (SOT-23, TDFN, WSON). The display connects via a standard 1×08 2.54 mm pitch pin header.

## Case

Designed in FreeCAD with a boxy, sharp-edged aesthetic. The case holds:
- The custom PCB
- A single-cell LiPo battery
- The ST7789 TFT display (1.54″ or 1.69″)

Dummy battery and display models are included for fit checking in the assembly.

## License

MIT License — see [LICENSE](LICENSE) for details.

Copyright (c) 2026 jacobjuneau6
