#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct Pixmap
{
    uint32_t width, height;
    uint32_t length;
    uint32_t size;
    uint32_t *data;
} Pixmap;

Pixmap pixmap_create(uint32_t width, uint32_t height)
{
    Pixmap map = {
        .width = width,
        .height = height,
    };
    map.length = width * height;
    map.size = width * height * sizeof(uint32_t);
    map.data = (uint32_t *)malloc(map.size);
    memset(map.data, 0, map.size);
    return map;
}

void pixmap_delete(Pixmap *pixmap)
{
    if (pixmap != NULL)
    {
        pixmap->width = 0;
        pixmap->height = 0;
        pixmap->length = 0;
        pixmap->size = 0;
        if (pixmap->data != NULL)
        {
            free(pixmap->data);
            pixmap->data = NULL;
        }
    }
}

void pixmap_clear_color(Pixmap *pixmap, uint32_t color)
{
    for (int i = 0; i < pixmap->length; i++)
        pixmap->data[i] = color;
}

void pixmap_dump(const Pixmap *pixmap, const char *file_name)
{
    FILE *f = fopen(file_name, "wb");
    fwrite(pixmap->data, sizeof(uint32_t), pixmap->width * pixmap->height, f);
    fclose(f);
}

void pixel_set(Pixmap *pixmap, int32_t x, int32_t y, uint32_t color)
{
    pixmap->data[x + pixmap->width * y] = color;
}

/**
 * A naive approach line drawing algorithm using the math equations for slope and line function.
 * 
 * Pros:
 *     - Easy to implement
 *     - Straigtforward
 * 
 * Cons:
 *     - Floating point calculations (like the slope or the y point)
 *     - Rounding up the y every iteration
 *     - Not supporting all types of slopes
 *     - Does not handle divison by 0 in case dx equals to 0
 */
void line_draw_naive(Pixmap *pixmap, int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint32_t color)
{
    float m = (y1 - y0) / (x1 - x0);
    float b = y0 - m * x0;
    for (int32_t x = x0; x <= x1; x++)
    {
        pixel_set(pixmap, x, y0, color);
        y0 = (int32_t)roundf(m * x + b);
    }
}

/**
 * A naive approach line drawing algorithm using the math equations for slope and line function.
 * 
 * Pros:
 *     - Easy to implement
 *     - Straigtforward
 *     - Supports all types of slopes
 *     - Handles division by 0 cases
 * 
 * Cons:
 *     - Floating point calculations (like the slope or the y | x point)
 *     - Rounding up the y | x every iteration
 */
void line_draw_improved(Pixmap *pixmap, int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint32_t color)
{
    int32_t dx = x1 - x0;
    int32_t dy = y1 - y0;

    if (dx > dy) // Slope < 1
    {
        float m = (float)dy / dx;
        float b = y0 - m * x0;
        dx = (dx < 0) ? -1 : 1;
        for (int32_t x = x0; x != x1; x += dx)
        {
            pixel_set(pixmap, x, y0, color);
            y0 = (int32_t)roundf(m * x + b);
        }
    }
    else
    {
        if (dy != 0) // Slope >= 1
        {
            float m = (float)dx / dy;
            float b = x0 - m * y0;
            dy = (dy < 0) ? -1 : 1;
            for (int32_t y = y0; y != y1; y += dy)
            {
                pixel_set(pixmap, x0, y, color);
                x0 = (int32_t)roundf(m * y + b);
            }
        }
    }
}

int main(void)
{
    Pixmap map = pixmap_create(800, 600);
    pixmap_clear_color(&map, 0xFF000000);
    line_draw_improved(&map, 10, 10, 100, 100, 0xFFFFFFFF);
    line_draw_improved(&map, 10, 100, 100, 10, 0xFFFFFFFF);
    line_draw_improved(&map, 10, 10, 10, 100, 0xFFFFFFFF);
    line_draw_improved(&map, 10, 10, 100, 10, 0xFFFFFFFF);
    line_draw_improved(&map, 100, 10, 100, 100, 0xFFFFFFFF);
    line_draw_improved(&map, 10, 100, 100, 100, 0xFFFFFFFF);
    pixmap_dump(&map, "output.data");
    pixmap_delete(&map);
}
