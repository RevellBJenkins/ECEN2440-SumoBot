
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include <stdint.h>
#include <string.h>

// OLED configuration
constexpr uint OLED_SDA = 6;
constexpr uint OLED_SCL = 7;
constexpr uint8_t OLED_ADDR = 0x3C;


constexpr int OLED_WIDTH = 128;
constexpr int OLED_HEIGHT = 32;

// Display buffer: 128 * 32 / 8 = 512 bytes
uint8_t buffer[OLED_WIDTH * OLED_HEIGHT / 8] = {0};

// Send a command to the OLED
void oled_command(uint8_t command) {
    uint8_t data[] = {0x00, command};
    i2c_write_blocking(i2c1, OLED_ADDR, data, 2, false);
}

// Initialize OLED
void oled_init() {
    i2c_init(i2c1, 400000);

    gpio_set_function(OLED_SDA, GPIO_FUNC_I2C);
    gpio_set_function(OLED_SCL, GPIO_FUNC_I2C);

    gpio_pull_up(OLED_SDA);
    gpio_pull_up(OLED_SCL);

    sleep_ms(100);

    oled_command(0xAE); // Display OFF
    oled_command(0xD5); // Clock divide
    oled_command(0x80);
    oled_command(0xA8); // Multiplex ratio
    oled_command(0x1F); // 32 rows
    oled_command(0xD3); // Display offset
    oled_command(0x00);
    oled_command(0x40); // Start line
    oled_command(0x8D); // Charge pump
    oled_command(0x14);
    oled_command(0x20); // Memory addressing mode
    oled_command(0x00); // Horizontal addressing
    oled_command(0xA1); // Segment remap
    oled_command(0xC8); // COM scan direction
    oled_command(0xDA); // COM pins
    oled_command(0x02);
    oled_command(0x81); // Contrast
    oled_command(0x8F);
    oled_command(0xD9); // Precharge
    oled_command(0xF1);
    oled_command(0xDB); // VCOMH
    oled_command(0x40);
    oled_command(0xA4); // Display follows RAM
    oled_command(0xA6); // Normal display
    oled_command(0xA4); // Display pixels from memory
    oled_command(0xAF); // Display ON
}

// Send the display buffer to the OLED
void oled_update() {
    oled_command(0x21);
    oled_command(0);
    oled_command(127);

    oled_command(0x22);
    oled_command(0);
    oled_command(3);

    // Send 16 display bytes at a time
    for (int i = 0; i < sizeof(buffer); i += 16) {
        uint8_t data[17];
        data[0] = 0x40;

        memcpy(&data[1], &buffer[i], 16);

        i2c_write_blocking(i2c1, OLED_ADDR, data, 17, false);
    }
}

// Draw a pixel
void oled_pixel(int x, int y) {
    if (x < 0 || x >= OLED_WIDTH ||
        y < 0 || y >= OLED_HEIGHT) {
        return;
    }

    buffer[x + (y / 8) * OLED_WIDTH] |= (1 << (y % 8));
}

// 5x7 font for the characters in "SUMO BOT"
const uint8_t font[][5] = {
    {0x46, 0x49, 0x49, 0x49, 0x31}, // S
    {0x3F, 0x40, 0x40, 0x40, 0x3F}, // U
    {0x7F, 0x02, 0x0C, 0x02, 0x7F}, // M
    {0x3E, 0x41, 0x41, 0x41, 0x3E}, // O
    {0x7F, 0x49, 0x49, 0x49, 0x36}, // B
    {0x01, 0x01, 0x7F, 0x01, 0x01}  // T
};

// Draw one character
void oled_char(char c, int x, int y) {
    int index = -1;

    switch (c) {
        case 'S': index = 0; break;
        case 'U': index = 1; break;
        case 'M': index = 2; break;
        case 'O': index = 3; break;
        case 'B': index = 4; break;
        case 'T': index = 5; break;
        case ' ': return;
        default: return;
    }

    for (int col = 0; col < 5; col++) {
        for (int row = 0; row < 7; row++) {
            if (font[index][col] & (1 << row)) {
                oled_pixel(x + col, y + row);
            }
        }
    }
}

// Draw a string
void oled_text(const char* text, int x, int y) {
    while (*text) {
        oled_char(*text, x, y);
        x += 6;
        text++;
    }
}

int main() {
    oled_init();

    memset(buffer, 0, sizeof(buffer));

    // Display text
    oled_text("SUMO BOT", 40, 12);

    oled_update();

    while (true) {
        sleep_ms(1000);
    }

    return 0;
}
