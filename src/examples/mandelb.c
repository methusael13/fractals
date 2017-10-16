#include <omp.h>
#include <stdio.h>

#include <gfx/gfx.h>
#include <lib/palette.h>

#define kIterCount 150
#define WIN_WIDTH 800
#define WIN_HEIGHT 540
#define PALETTE_SZ 2048

const float kScale = 3.5f / WIN_WIDTH;
const float kXShift = WIN_WIDTH * 0.75f;
const float kYShift = WIN_HEIGHT * 0.5f;

float iterate_pixel(float x, float y) {
    int i = 0;
    float cx = (x - kXShift) * kScale;
    float cy = (y - kYShift) * kScale;
    float zx = 0, zy = 0, nx;

    while (i < kIterCount && zx*zx + zy*zy < 4.0f) {
        nx = zx*zx - zy*zy + cx;
        zy = 2*zx*zy + cy;
        zx = nx; i++;
    }

    return i == kIterCount ? 0 : (float) i / kIterCount;
}

void render_mandelbrot(int (*palette)[3]) {
    int p, cidx;
    const int pixels = WIN_WIDTH * WIN_HEIGHT;

    #pragma omp parallel private(p, cidx, pixels)
    {
        #pragma omp for
        for (p = 0; p < pixels; p++) {
            int _x = p % WIN_WIDTH, _y = p / WIN_WIDTH;
            cidx = (int)(iterate_pixel(_x, _y) * (PALETTE_SZ - 1));
            gfx_color(palette[cidx][0], palette[cidx][1], palette[cidx][2]);
            gfx_point(_x, _y);
        }
        
        /* If being executed in the master thread */
        if (omp_get_thread_num() == 0) {
            printf("OpenMP thread count: %d\n", omp_get_num_threads());
        }
    }
}

int main(int argc, const char* argv[]) {
    const char* title = "Mandelbrot Set";
    gfx_open(WIN_WIDTH, WIN_HEIGHT, title, 0);

    // Color palette data
    int color_stops[5][3] = {
        { 0, 7, 100 }, { 32, 107, 203 }, { 237, 255, 255 },
        { 255, 170, 0 }, { 0, 2, 0 }
    };
    float stop_pos[5] = { 0.0f, 0.16f, 0.42f, 0.6425f, 0.8575f };
    int palette[PALETTE_SZ][3];

    build_color_palette(color_stops, stop_pos, 5, palette, PALETTE_SZ);
    render_mandelbrot(palette);

    while (TRUE) {
        if (gfx_wait() == 'q') break;
    }
    
    return 0;
}
