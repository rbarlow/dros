#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <vga.h>

/*
 * The VGA framebuffer is capable of displaying text.
 *
 * The framebuffer starts at 0xB8000 and consists of 80 columns
 * and 25 rows of 16 bit entries. Each entry consists of an ASCII
 * character, the foreground (text) color, and the background color.
 *
 * Bit:     | 15 14 13 12 11 10 9 8 | 7 6 5 4 | 3 2 1 0 |
 * Content: | ASCII                 | FG      | BG      |
 */

/* Defines the current terminal cursor location */
size_t fb_cursor_row;
size_t fb_cursor_column;

uint16_t *framebuffer;

/* Initialize the terminal */
void fb_init(void)
{
	fb_cursor_row = 0;
	fb_cursor_column = 0;
	framebuffer = (uint16_t *) VGA_MEMORY_LOCATION;
	for (size_t row = 0; row < VGA_HEIGHT; row++) {
		for (size_t column = 0; column < VGA_WIDTH; column++) {
			const size_t index = row * VGA_WIDTH + column;
			framebuffer[index] =
			    make_vga_entry(' ', COLOR_LIGHT_GREY, COLOR_BLACK);
		}
	}

    /* Put the cursor back at the beginning */
	fb_cursor_row = 0;
	fb_cursor_column = 0;
}

void fb_write_cell(char c, enum vga_color foreground,
        enum vga_color background, size_t row, size_t column)
{
    const size_t index = row * VGA_WIDTH + column;
    framebuffer[index] = make_vga_entry(c, foreground, background);
}


/* Write a single character to the terminal */
void fb_write_char(char c)
{
    if (c == '\t') {
        if (fb_cursor_column > 72) {
            c = '\n';
        } else {
            fb_cursor_column += 8 - (fb_cursor_column % 8);
            return;
        }
    }

    /* Handle newline characters */
	if (c == '\n') {
		fb_cursor_column = 0;
		fb_cursor_row++;
		return;
	}

    /* Wrap text by bumping the row when the column hits maximum width */
	if (fb_cursor_column == VGA_WIDTH) {
		fb_cursor_column = 0;
		fb_cursor_row++;
	}

	if (fb_cursor_row == VGA_HEIGHT) {
		// Shift everyone up to make a new row available
		for (size_t row = 1; row < VGA_HEIGHT; row++) {
			for (size_t column = 0; column < VGA_WIDTH; column++) {
				const size_t entry = row * VGA_WIDTH + column;
				framebuffer[entry - VGA_WIDTH] = framebuffer[entry];
				framebuffer[entry] = make_vga_entry(' ', COLOR_LIGHT_GREY, COLOR_BLACK);
			}
		}

		fb_cursor_column = 0;
		fb_cursor_row--;
    }

	fb_write_cell(c, COLOR_LIGHT_GREY, COLOR_BLACK, fb_cursor_row, fb_cursor_column);
    fb_cursor_column++;
}

/* Write text to the terminal */
void fb_write(const char *data, size_t length)
{
    for(size_t i = 0; i < length; i++)
    {
        fb_write_char(data[i]);
    }
}

