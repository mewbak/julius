#include "game/system.h"
#include "graphics/color.h"
#include "input/cursor.h"

#include "SDL.h"

static SDL_Cursor *cursors[CURSOR_MAX];
static SDL_Surface *cursor_surfaces[CURSOR_MAX];
static int current_cursor_id = CURSOR_ARROW;

static const color_t mouse_colors[] = {
    ALPHA_TRANSPARENT,
    ALPHA_TRANSPARENT,
    ALPHA_TRANSPARENT,
    ALPHA_OPAQUE | COLOR_BLACK,
    ALPHA_OPAQUE | COLOR_MOUSE_DARK_GRAY,
    ALPHA_OPAQUE | COLOR_MOUSE_MEDIUM_GRAY,
    ALPHA_OPAQUE | COLOR_MOUSE_LIGHT_GRAY,
    ALPHA_OPAQUE | COLOR_WHITE
};

static SDL_Surface *generate_cursor_surface(const char *data, int width, int height)
{
    // make sure the cursor is a power of two
    int size = 32;
    while (size <= width || size <= height) {
        size *= 2;
    }
    SDL_Surface *cursor_surface =
        SDL_CreateRGBSurface(0, size, size, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
    color_t *pixels = cursor_surface->pixels;
    SDL_memset(pixels, 0, 4 * size * size);
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            pixels[y * size + x] = mouse_colors[data[y * width + x] - 32];
        }
    }
    return cursor_surface;
}

static cursor_scale get_cursor_scale(int scale_percentage)
{
    if (scale_percentage <= 100) {
        return CURSOR_SCALE_1;
    } else if (scale_percentage <= 150) {
        return CURSOR_SCALE_1_5;
    } else {
        return CURSOR_SCALE_2;
    }
}

void system_init_cursors(int scale_percentage)
{
    cursor_scale cur_scale = get_cursor_scale(scale_percentage);
    for (int i = 0; i < CURSOR_MAX; i++) {
        const cursor *c = input_cursor_data(i, cur_scale);
        if (cursor_surfaces[i]) {
            SDL_FreeSurface(cursor_surfaces[i]);
        }
        if (cursors[i]) {
            SDL_FreeCursor(cursors[i]);
        }
        cursor_surfaces[i] = generate_cursor_surface(c->data, c->width, c->height);
        cursors[i] = SDL_CreateColorCursor(cursor_surfaces[i], c->hotspot_x, c->hotspot_y);
    }
    system_set_cursor(current_cursor_id);
}

void system_set_cursor(int cursor_id)
{
    current_cursor_id = cursor_id;
    SDL_SetCursor(cursors[cursor_id]);
}
