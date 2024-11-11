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

int main(void)
{
    Pixmap map = pixmap_create(800, 600);
    pixmap_clear_color(&map, 0xFF000000);
    pixmap_dump(&map, "output.data");
    pixmap_delete(&map);
}
