# Minimal bring-up firmware

quick and dirty test for the *funky* boi:
- ST7789V3 172x320 display on GPIO40..45
- 8 active-low buttons with pull-ups
- USB serial logging
- No PSRAM, flash filesystem, Doom, or audio yet

## Expected behavior
1. Boot -> LCD shows 8 color bars.
2. Then the loop goes black when no button is pressed.
3. Pressing each button fills the display with a unique color and prints button state over USB CDC serial.

## Pin map
LCD: MOSI 40, SCK 41, CS 42, DC 43, RST 44, BL 45
Buttons: UP 0, LEFT 1, RIGHT 2, DOWN 3, B 35, A 37, SELECT 18, START 24

## Build
Use a recent Raspberry Pi Pico SDK with RP2350 support.

```bash
mkdir build && cd build
cmake .. -DPICO_BOARD=pico2
make -j
```

Copy `microboy_bringup.uf2` to the RP2350 BOOTSEL drive.

## If the LCD is shifted / mirrored
This project assumes landscape 320x172 with a 34-pixel controller RAM offset and MADCTL=0x60.
If your panel orientation differs, adjust the MADCTL byte in `lcd_init()` and/or `yoff` in `lcd_window()`.
