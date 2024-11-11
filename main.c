#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/**
 * Structure representing a Pixmap, a 2D pixel-based image.
 * Contains:
 * - width: Width of the pixmap in pixels.
 * - height: Height of the pixmap in pixels.
 * - length: Total number of pixels in the pixmap (width * height).
 * - size: Total memory size of the pixel data in bytes.
 * - data: Pointer to the pixel data (each pixel is a uint32_t representing color).
 */
typedef struct Pixmap
{
    uint32_t width, height;
    uint32_t length;
    uint32_t size;
    uint32_t *data;
} Pixmap;

/**
 * Creates and initializes a Pixmap with the specified width and height.
 * 
 * @param width Width of the pixmap.
 * @param height Height of the pixmap.
 * @return An initialized Pixmap structure with allocated memory for pixel data.
 */
Pixmap pixmap_create(uint32_t width, uint32_t height)
{
    Pixmap map = {
        .width = width,
        .height = height,
    };
    map.length = width * height;
    map.size = width * height * sizeof(uint32_t);
    map.data = (uint32_t *)malloc(map.size);
    memset(map.data, 0, map.size); // Set all pixels to 0 (black).
    return map;
}

/**
 * Frees the memory associated with a Pixmap and resets its properties.
 *
 * @param pixmap Pointer to the Pixmap to delete.
 */
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

/**
 * Sets all pixels in the Pixmap to a specified color.
 *
 * @param pixmap Pointer to the Pixmap to modify.
 * @param color Color to set each pixel to (32-bit value).
 */
void pixmap_clear_color(Pixmap *pixmap, uint32_t color)
{
    for (int i = 0; i < pixmap->length; i++)
        pixmap->data[i] = color;
}

/**
 * Saves the Pixmap's pixel data to a binary file.
 *
 * @param pixmap Pointer to the Pixmap to save.
 * @param file_name The name of the output file.
 */
void pixmap_dump(const Pixmap *pixmap, const char *file_name)
{
    FILE *f = fopen(file_name, "wb");
    fwrite(pixmap->data, sizeof(uint32_t), pixmap->width * pixmap->height, f);
    fclose(f);
}

/**
 * Sets a specific pixel in the Pixmap to a specified color.
 *
 * @param pixmap Pointer to the Pixmap to modify.
 * @param x X-coordinate of the pixel.
 * @param y Y-coordinate of the pixel.
 * @param color Color to set the pixel to (32-bit value).
 */
void pixel_set(Pixmap *pixmap, int32_t x, int32_t y, uint32_t color)
{
    pixmap->data[y * pixmap->width + x] = color;
}

/**
 * Draws a line on the Pixmap using a naive algorithm based on the line equation (y = mx + b).
 * 
 * Pros:
 * - Simple to implement.
 * - Easy to understand.
 *
 * Cons:
 * - Floating-point calculations are used (may be slower).
 * - Limited support for certain slopes (dx = 0 causes division by zero).
 *
 * @param pixmap Pointer to the Pixmap to draw on.
 * @param x0 Starting x-coordinate of the line.
 * @param y0 Starting y-coordinate of the line.
 * @param x1 Ending x-coordinate of the line.
 * @param y1 Ending y-coordinate of the line.
 * @param color Color of the line.
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
 * Draws a line on the Pixmap using an improved algorithm that supports all slopes and avoids division by zero.
 *
 * Pros:
 * - Supports all slopes.
 * - Avoids division by zero.
 *
 * Cons:
 * - Uses floating-point calculations.
 *
 * @param pixmap Pointer to the Pixmap to draw on.
 * @param x0 Starting x-coordinate of the line.
 * @param y0 Starting y-coordinate of the line.
 * @param x1 Ending x-coordinate of the line.
 * @param y1 Ending y-coordinate of the line.
 * @param color Color of the line.
 */
void line_draw_improved(Pixmap *pixmap, int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint32_t color)
{
    int32_t dx = x1 - x0;
    int32_t dy = y1 - y0;

    if (abs(dx) > abs(dy)) // Slope < 1
    {
        float m = (float)dy / dx;
        float b = y0 - m * x0;
        dx = (dx < 0) ? -1 : 1;
        for (int32_t x = x0; x != x1 + 1; x += dx)
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
            for (int32_t y = y0; y != y1 + 1; y += dy)
            {
                pixel_set(pixmap, x0, y, color);
                x0 = (int32_t)roundf(m * y + b);
            }
        }
    }
}

/**
 * Draws a line on the Pixmap using Bresenham's line algorithm.
 * 
 * Pros:
 * - Efficient for integer calculations (no floating point).
 * - Supports all slopes.
 * - Suitable for hardware implementation.
 *
 * Cons:
 * - More complex logic compared to naive methods.
 *
 * @param pixmap Pointer to the Pixmap to draw on.
 * @param x0 Starting x-coordinate of the line.
 * @param y0 Starting y-coordinate of the line.
 * @param x1 Ending x-coordinate of the line.
 * @param y1 Ending y-coordinate of the line.
 * @param color Color of the line.
 */
void line_draw_bresenham(Pixmap *pixmap, int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint32_t color)
{
    int32_t dx = x1 - x0;
    int32_t dy = y1 - y0;

    if (abs(dx) > abs(dy))
    {
        int32_t D = 2 * dy - dx;
        dx = (dx < 0) ? -1 : 1;

        for (int32_t x = x0; x != x1 + 1; x += dx)
        {
            pixel_set(pixmap, x, y0, color);
            if (D > 0)
            {
                y0++;
                D -= 2 * dx;
            }
            D += 2 * dy;
        }
    }
    else
    {
        int32_t D = 2 * dx - dy;
        dy = (dy < 0) ? -1 : 1;

        for (int32_t y = y0; y != y1 + 1; y += dy)
        {
            pixel_set(pixmap, x0, y, color);
            if (D > 0)
            {
                x0++;
                D -= 2 * dy;
            }
            D += 2 * dx;
        }
    }
}

int main(int argc, char const *argv[])
{
    Pixmap map = pixmap_create(800, 600);
    pixmap_clear_color(&map, 0xFF000000);
    pixmap_dump(&map, "output.data");
    pixmap_delete(&map);
    return 0;
}
