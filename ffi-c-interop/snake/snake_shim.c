#include "raylib.h"

void shim_clear_background(int r, int g, int b, int a) {
    Color c = { (unsigned char)r, (unsigned char)g, (unsigned char)b, (unsigned char)a };
    ClearBackground(c);
}

void shim_draw_rectangle(int x, int y, int w, int h, int r, int g, int b, int a) {
    Color c = { (unsigned char)r, (unsigned char)g, (unsigned char)b, (unsigned char)a };
    DrawRectangle(x, y, w, h, c);
}

void shim_draw_text(const char *text, int x, int y, int size, int r, int g, int b, int a) {
    Color c = { (unsigned char)r, (unsigned char)g, (unsigned char)b, (unsigned char)a };
    DrawText(text, x, y, size, c);
}

int shim_get_random_value(int min, int max) {
    return GetRandomValue(min, max);
}
