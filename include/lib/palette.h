#ifndef FRACTAL_PALETTE_H
#define FRACTAL_PALETTE_H

#define TRUE 1
#define FALSE 0

void build_color_palette(int (*stops)[3], float *stop_pos, int stop_size,
                         int (*palette)[3], int palette_size);

#endif
