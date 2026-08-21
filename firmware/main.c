#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/clocks.h"

// ---------------- funkyboi pin map ----------------
#define PIN_LCD_MOSI 40
#define PIN_LCD_SCK  41
#define PIN_LCD_CS   42
#define PIN_LCD_DC   43
#define PIN_LCD_RST  44
#define PIN_LCD_BL   45

#define PIN_UP       0
#define PIN_LEFT     1
#define PIN_RIGHT    2
#define PIN_DOWN     3
#define PIN_B        35
#define PIN_A        37
#define PIN_SELECT   18
#define PIN_START    24

#define LCD_W 320
#define LCD_H 172

static inline void lcd_cs(bool high)  { gpio_put(PIN_LCD_CS, high); }
static inline void lcd_dc(bool high)  { gpio_put(PIN_LCD_DC, high); }
static inline void lcd_sck(bool high) { gpio_put(PIN_LCD_SCK, high); }
static inline void lcd_mosi(bool high){ gpio_put(PIN_LCD_MOSI, high); }

// Slow-but-simple SPI bitbang for first board bring-up.
static void spi_write8(uint8_t v) {
    for (int i = 7; i >= 0; --i) {
        lcd_sck(false);
        lcd_mosi((v >> i) & 1);
        __asm volatile("nop\n nop\n");
        lcd_sck(true);
        __asm volatile("nop\n nop\n");
    }
    lcd_sck(false);
}

static void lcd_cmd(uint8_t c) {
    lcd_cs(false);
    lcd_dc(false);
    spi_write8(c);
    lcd_cs(true);
}

static void lcd_data8(uint8_t d) {
    lcd_cs(false);
    lcd_dc(true);
    spi_write8(d);
    lcd_cs(true);
}

static void lcd_data(const uint8_t *p, size_t n) {
    lcd_cs(false);
    lcd_dc(true);
    while (n--) spi_write8(*p++);
    lcd_cs(true);
}

static void lcd_reset(void) {
    gpio_put(PIN_LCD_RST, 1);
    sleep_ms(10);
    gpio_put(PIN_LCD_RST, 0);
    sleep_ms(20);
    gpio_put(PIN_LCD_RST, 1);
    sleep_ms(120);
}

static void lcd_init(void) {
    gpio_put(PIN_LCD_BL, 0);
    lcd_reset();

    lcd_cmd(0x01); // SWRESET
    sleep_ms(150);
    lcd_cmd(0x11); // SLPOUT
    sleep_ms(120);

    // 16-bit RGB565
    lcd_cmd(0x3A);
    lcd_data8(0x55);

    // Landscape orientation. If mirrored/upside-down, try 0xA0 / 0x60 / 0xC0.
    lcd_cmd(0x36);
    lcd_data8(0x60);

    // Normal display on, inversion on is common for ST7789 panels.
    lcd_cmd(0x21);
    lcd_cmd(0x13);
    lcd_cmd(0x29);
    sleep_ms(20);

    gpio_put(PIN_LCD_BL, 1);
}

static void lcd_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    // For 172x320 glass in landscape, controller RAM is offset by 34 rows.
    const uint16_t yoff = 34;
    y0 += yoff;
    y1 += yoff;

    uint8_t b[4];
    lcd_cmd(0x2A); // CASET
    b[0] = x0 >> 8; b[1] = x0; b[2] = x1 >> 8; b[3] = x1;
    lcd_data(b, 4);

    lcd_cmd(0x2B); // RASET
    b[0] = y0 >> 8; b[1] = y0; b[2] = y1 >> 8; b[3] = y1;
    lcd_data(b, 4);

    lcd_cmd(0x2C); // RAMWR
}

static void lcd_fill(uint16_t color) {
    lcd_window(0, 0, LCD_W - 1, LCD_H - 1);
    lcd_cs(false);
    lcd_dc(true);
    uint8_t hi = color >> 8, lo = color & 0xff;
    for (int i = 0; i < LCD_W * LCD_H; ++i) {
        spi_write8(hi);
        spi_write8(lo);
    }
    lcd_cs(true);
}

static void lcd_color_bars(void) {
    const uint16_t colors[] = {
        0xF800, // red
        0x07E0, // green
        0x001F, // blue
        0xFFE0, // yellow
        0x07FF, // cyan
        0xF81F, // magenta
        0xFFFF, // white
        0x0000  // black
    };
    const int n = (int)(sizeof(colors)/sizeof(colors[0]));
    const int bar_h = LCD_H / n;

    for (int bar = 0; bar < n; ++bar) {
        int y0 = bar * bar_h;
        int y1 = (bar == n - 1) ? (LCD_H - 1) : (y0 + bar_h - 1);
        lcd_window(0, y0, LCD_W - 1, y1);
        lcd_cs(false);
        lcd_dc(true);
        uint8_t hi = colors[bar] >> 8, lo = colors[bar] & 0xff;
        for (int i = 0; i < LCD_W * (y1 - y0 + 1); ++i) {
            spi_write8(hi);
            spi_write8(lo);
        }
        lcd_cs(true);
    }
}

static void button_init(int pin) {
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_IN);
    gpio_pull_up(pin);
}

static bool pressed(int pin) {
    return gpio_get(pin) == 0;
}

static uint16_t color_from_buttons(void) {
    if (pressed(PIN_A))      return 0xF800; // red
    if (pressed(PIN_B))      return 0x001F; // blue
    if (pressed(PIN_UP))     return 0x07E0; // green
    if (pressed(PIN_DOWN))   return 0xFFE0; // yellow
    if (pressed(PIN_LEFT))   return 0x07FF; // cyan
    if (pressed(PIN_RIGHT))  return 0xF81F; // magenta
    if (pressed(PIN_SELECT)) return 0xFFFF; // white
    if (pressed(PIN_START))  return 0x8410; // gray
    return 0x0000;
}

int main(void) {
    stdio_init_all();
    sleep_ms(500);

    // LCD GPIO
    const int outs[] = {PIN_LCD_MOSI, PIN_LCD_SCK, PIN_LCD_CS, PIN_LCD_DC, PIN_LCD_RST, PIN_LCD_BL};
    for (unsigned i = 0; i < sizeof(outs)/sizeof(outs[0]); ++i) {
        gpio_init(outs[i]);
        gpio_set_dir(outs[i], GPIO_OUT);
        gpio_put(outs[i], 0);
    }
    gpio_put(PIN_LCD_CS, 1);
    gpio_put(PIN_LCD_RST, 1);

    // Buttons
    button_init(PIN_UP);
    button_init(PIN_LEFT);
    button_init(PIN_RIGHT);
    button_init(PIN_DOWN);
    button_init(PIN_B);
    button_init(PIN_A);
    button_init(PIN_SELECT);
    button_init(PIN_START);

    lcd_init();
    lcd_color_bars();

    printf("funkyboi bring-up started. sys_clk=%u Hz\n", clock_get_hz(clk_sys));
    printf("Press buttons: screen color changes and USB serial prints state.\n");

    uint16_t last = 0xFFFF;
    uint32_t last_mask = 0xFFFFFFFFu;

    while (true) {
        uint32_t mask = 0;
        mask |= pressed(PIN_UP)     << 0;
        mask |= pressed(PIN_LEFT)   << 1;
        mask |= pressed(PIN_RIGHT)  << 2;
        mask |= pressed(PIN_DOWN)   << 3;
        mask |= pressed(PIN_B)      << 4;
        mask |= pressed(PIN_A)      << 5;
        mask |= pressed(PIN_SELECT) << 6;
        mask |= pressed(PIN_START)  << 7;

        uint16_t c = color_from_buttons();
        if (c != last) {
            lcd_fill(c);
            last = c;
        }

        if (mask != last_mask) {
            printf("buttons: U%d L%d R%d D%d B%d A%d SEL%d START%d\n",
                !!(mask & 1), !!(mask & 2), !!(mask & 4), !!(mask & 8),
                !!(mask & 16), !!(mask & 32), !!(mask & 64), !!(mask & 128));
            last_mask = mask;
        }

        sleep_ms(20);
    }
}
