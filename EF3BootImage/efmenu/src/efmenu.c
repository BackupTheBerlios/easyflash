/*
 * (c) 2010 Thomas Giesel
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * Thomas Giesel skoe@directbox.com
 */

#include <stdint.h>
#include <string.h>
#include <conio.h>
#include <c64.h>

#include "text_plot.h"
#include "memcfg.h"

// from gfx.s
extern const uint8_t* bitmap;
extern const uint8_t* colmap;
extern const uint8_t* attrib;
extern uint8_t background;

// from efmenu_asm.s
void __fastcall__ setBankChangeMode(uint8_t bank, uint8_t mode);
void waitForNoKey(void);

#define MODE_EF     0
#define MODE_FC3    1
#define MODE_GEORAM 2
#define MODE_KERNAL 3

typedef struct mode_map_s
{
    uint8_t     key;
    uint8_t     bank;
    uint8_t     mode;
    const char* label;
    const char* comment;
} mode_map_t;

static const mode_map_t mode_map[] =
{
        { CH_F1,    0x00,   MODE_EF,        "F1", "EasyFlash" },
        { CH_F3,    0x70,   MODE_FC3,       "F3", "Final Cartridge III" },
        { CH_F5,    0x00,   MODE_GEORAM,    "F5", "GeoRAM" },
        { CH_F7,    0x74,   MODE_KERNAL,    "F7", "Exos V3 Kernal" },
        { 0, 0, NULL, NULL }
};


static void showMenu(void)
{
    uint8_t y;
    const mode_map_t* entry;

    y = 13 * 8 + 2;

    entry = mode_map;
    while (entry->key)
    {
        text_plot_puts(22 * 8 + 2, y, entry->label);
        text_plot_puts(24 * 8 + 4, y, entry->comment);
        y += 8;
        ++entry;
    }
}


static void waitForKey(void)
{
    uint8_t key;
    const mode_map_t* entry;

    do
    {
        // scan the keyboard
        // we don't do this in IRQ, because of the sprite multiplexer
        //__asm__("jsr $ff9f");

        if (kbhit())
        {
            key = cgetc();

            entry = mode_map;
            while (entry->key)
            {
                if (entry->key == key)
                {
                	VIC.bordercolor = COLOR_WHITE;
                    // Wait until the key is released
                    waitForNoKey();
                    // PONR
                    setBankChangeMode(entry->bank, entry->mode);
                }
                ++entry;
            }
        }
    }
    while (1);
}

static void prepare_background(void)
{
	uint8_t y, yy;

	for (y = 0; y < 10; ++y)
	{
		yy = y + 2;
		memset(P_GFX_COLOR + 40 * yy + 2,
			   COLOR_WHITE << 4, 16);
		memset(P_GFX_BITMAP + 320 * yy + 2 * 8, 0, 16 * 8);

		yy = y + 13;
		memset(P_GFX_COLOR + 40 * yy + 22,
			   COLOR_BLACK << 4 | COLOR_GRAY3, 16);
		memset(P_GFX_BITMAP + 320 * yy + 22 * 8, 0, 16 * 8);
	}
}


int main(void)
{
    // copy bitmap at $A000 from ROM to RAM => VIC can see it
    memcpy(P_GFX_BITMAP, bitmap, 8000);

    // copy colors to $8400
    memcpy(P_GFX_COLOR, colmap, 1000);

//    memset(P_GFX_COLOR, 15 << 4, 1000);
  //  memset(P_GFX_BITMAP, 0, 8000);

    // copy attribs to $d800
    //memcpy((char*)0xd800, attrib, 1000);

    VIC.bordercolor = COLOR_BLACK;
    VIC.bgcolor0 = background;

    /* set VIC base address to $4000 */
    CIA2.pra = 0x14 + 2;

    /* DDR => Output */
    CIA2.ddra = 0x3f;

    /* video offset $2000, bitmap offset = $0000 */
    VIC.addr = 0x80;

    /* Bitmap mode */
    VIC.ctrl1 = 0xbb;

    /* MultiColor */
    /* VIC.ctrl2 = 0x18; */

    prepare_background();

    showMenu();
    waitForKey();

    return 0;
}
