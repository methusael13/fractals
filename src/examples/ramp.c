#include <math.h>
#include <stdio.h>

#include <gfx/gfx.h>
#include <lib/palette.h>

#define WIN_WIDTH 512
#define WIN_HEIGHT 100

int main(int argc, const char* argv[]) {
    const char* title = "Color Ramp";
    gfx_open(WIN_WIDTH, WIN_HEIGHT, title, 0);
    
    // Color palette data
    int color_stops[5][3] = {
        { 0, 7, 100 }, { 32, 107, 203 }, { 237, 255, 255 },
        { 255, 170, 0 }, { 0, 2, 0 }
    };
    float stop_pos[5] = { 0.0f, 0.16f, 0.42f, 0.6425f, 0.8575f };
    int palette[WIN_WIDTH][3];

    build_color_palette(color_stops, stop_pos, 5, palette, WIN_WIDTH);
    int i, j;

    for (i = 0; i < WIN_WIDTH; i++) {
        gfx_color(palette[i][0], palette[i][1], palette[i][2]);
        for (j = 0; j < WIN_HEIGHT; j++)
            gfx_point(i, j);
    }

    while (TRUE) {
        if (gfx_wait() == 'q') break;
    }

    return 0;
}
