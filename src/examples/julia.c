#include <math.h>
#include <stdio.h>
#include <time.h>

#include <gfx/gfx.h>
#include <lib/palette.h>

#define kIterCount 100
#define WIN_WIDTH 800
#define WIN_HEIGHT 520
#define PALETTE_SZ 2048

#ifndef M_PI
const double M_PI = 3.14159265358979323846;
#endif
const double M_TWO_PI = 2 * M_PI;

const float kScale = 5.5f / WIN_WIDTH;
const float kXShift = WIN_WIDTH * 0.5f;
const float kYShift = WIN_HEIGHT * 0.5f;

float iterate_pixel(float x, float y, float julia_rc, float julia_ic) {
    int i = 0;
    float cx = (x - kXShift) * kScale;
    float cy = (y - kYShift) * kScale;
    float zx = cx, zy = cy, nx;

    while (i < kIterCount && zx*zx + zy*zy < 4.0f) {
        nx = zx*zx - zy*zy + julia_rc;
        zy = 2*zx*zy + julia_ic;
        zx = nx; i++;
    }

    return i == kIterCount ? 0 : (float) i / kIterCount;
}

void render_julia(float real, float imag, int (*palette)[3]) {
    int x, y, cidx;

    for (y = 0; y < WIN_HEIGHT; y++) {
        for (x = 0; x < WIN_WIDTH; x++) {
            cidx = (int) (iterate_pixel(x, y, real, imag) * (PALETTE_SZ - 1));
            gfx_color(palette[cidx][0], palette[cidx][1], palette[cidx][2]);
            gfx_point(WIN_WIDTH - x - 1, y);
        }
    }
}

int main(int argc, const char* argv[]) {
    const char* title = "Julia Set";
    gfx_open(WIN_WIDTH, WIN_HEIGHT, title, 0);
    
    // Color palette data
    int color_stops[5][3] = {
        { 0, 7, 100 }, { 32, 107, 203 }, { 237, 255, 255 },
        { 255, 170, 0 }, { 0, 2, 0 }
    };
    float stop_pos[5] = { 0.0f, 0.16f, 0.42f, 0.6425f, 0.8575f };
    int palette[PALETTE_SZ][3];

    build_color_palette(color_stops, stop_pos, 5, palette, PALETTE_SZ);

    double duration;
    struct timespec cur_t, prev_t;
    clock_gettime(CLOCK_MONOTONIC, &cur_t);
    prev_t = cur_t;

    double render_vel = M_PI / 4.0;  // PI radians in 4 second
    double r = 0.7885, a = 0;

    while (TRUE) {
        clock_gettime(CLOCK_MONOTONIC, &cur_t);
        duration = (cur_t.tv_sec - prev_t.tv_sec) + (double)(cur_t.tv_nsec - prev_t.tv_nsec) / 1e9;
        prev_t = cur_t;

        a = fmod(a + duration * render_vel, M_TWO_PI);
        render_julia(r * cos(a), r * sin(a), palette);
    }
    
    return 0;
}
