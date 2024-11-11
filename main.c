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

void line_draw_naive(Pixmap *pixmap, int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint32_t color)
{
    int32_t dx = x1 - x0;
    int32_t dy = y1 - y0;
    if (dx != 0)
    {
        float m = (float)dy / (float)dx;
        float b = y0 - m * x0;
        for (int32_t x = x0; x <= x1; x++)
        {
            pixmap->data[x + pixmap->width * y0] = color;
            y0 = (int32_t)roundf(m * x + y0);
        }
    }
}

int main(void)
{
    Pixmap map = pixmap_create(800, 600);
    pixmap_clear_color(&map, 0xFF000000);
    line_draw_naive(&map, 10, 10, 200, 200, 0xFFFFFFFF);
    pixmap_dump(&map, "output.data");
    pixmap_delete(&map);
}
