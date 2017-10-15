#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <lib/palette.h>

/* Cubic hermite basis functions */
static float h00(float t) { return (1 + 2 * t) * (1 - t) * (1 - t); }
static float h10(float t) { return t * (1 - t) * (1 - t); }
static float h01(float t) { return t * t * (3 - 2 * t); }
static float h11(float t) { return t * t * (t - 1); }

#define ABS(x) ((x) < 0 ? -(x) : (x))

void build_color_palette(int (*stops)[3], float *stop_pos, int stop_size,
                         int (*palette)[3], int palette_size) {
    int i, c;

    // Build required data store
    float **delta = (float**) malloc(stop_size * sizeof(float*));
    float **m = (float**) malloc(stop_size * sizeof(float*));

    for (i = 0; i < stop_size; i++) {
        delta[i] = (float*) malloc(3 * sizeof(float));
        m[i] = (float*) malloc(3 * sizeof(float));
    }

    // Handle negative indexes as well
    #define CYCLE(x) ((x + stop_size) % stop_size)

    // For each color channel
    for (c = 0; c < 3; c++) {
        // Compute delta
        for (i = 0; i < stop_size - 1; i++)
            delta[i][c] = (stops[i+1][c] - stops[i][c]) / (stop_pos[i+1] - stop_pos[i]);
        delta[i][c] = (stops[CYCLE(i+1)][c] - stops[i][c]) / (1.0 - stop_pos[i]);

        // Compute m
        for (i = 0; i < stop_size; i++) {
            if (delta[CYCLE(i-1)][c] * delta[i][c] < 0)
                m[i][c] = 0;
            else
                m[i][c] = (delta[CYCLE(i-1)][c] + delta[i][c]) / 2.0f;
        }

        // Enforce monotonicity
        float alpha, beta, tau;
        for (i = 0; i < stop_size; i++) {
            if (delta[i][c] == 0)
                m[i][c] = m[CYCLE(i+1)][c] = 0;
            else {
                alpha = m[i][c] / delta[i][c]; beta = m[CYCLE(i+1)][c] / delta[i][c];
                if (alpha * alpha + beta * beta > 9) {
                    tau = 3.0f / sqrtf(alpha * alpha + beta * beta);
                    m[i][c] = tau * alpha * delta[i][c];
                    m[CYCLE(i+1)][c] = tau * beta * delta[i][c];
                }
            }
        }
    }

    // Build the palette
    register int stop_idx;
    register float t, yl, yu, ml, mu, f, h, xu;

    for (c = 0; c < 3; c++) {
        stop_idx = 0;
        for (i = 0; i < palette_size; i++) {
            f = (float) i / palette_size;
            // Special case for xupper when stop_idx is already at the last color stop
            xu = stop_idx >= stop_size - 1 ? 1.0f : stop_pos[stop_idx+1];
            h = xu - stop_pos[stop_idx]; t = (f - stop_pos[stop_idx]) / h;

            yl = stops[stop_idx][c], yu = stops[CYCLE(stop_idx+1)][c];
            ml = m[stop_idx][c], mu = m[CYCLE(stop_idx+1)][c];
            palette[i][c] = yl * h00(t) + h * ml * h10(t) + yu * h01(t) + h * mu * h11(t);

            if (stop_idx < stop_size - 1 && f >= stop_pos[stop_idx+1])
                stop_idx++;
        }
    }

    // Cleanup
    for (i = 0; i < stop_size; i++) { free(m[i]); free(delta[i]); }
    free(m); free(delta);

    #undef CYCLE
}
