#include "Screen.h"

Screen::Screen(int sx, int sy, bool fs, const char * name) :
    pixels(new Uint32[sx * sy]),
    width(sx),
    height(sy),
    rshift(16),
    gshift(8),
    bshift(0) {
    if (fs) {
        window = SDL_CreateWindow(name, 0, 0,
                        width, height,
                        SDL_WINDOW_FULLSCREEN);
    } else {
        window = SDL_CreateWindow(name,
                        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                        width, height,
                        0);
    }
#if VSYNC == 1
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED |
                                              SDL_RENDERER_PRESENTVSYNC);
#else
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
#endif
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                                          SDL_TEXTUREACCESS_STREAMING,
                                          width, height);
    Color c{255, 255, 255};
    default_color = format_color(c);
}

Screen::~Screen() {
    delete [] pixels;
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}

inline Uint32& Screen::pixel_at(int x, int y) {
    return pixels[y * width + x];
}

inline Uint32 Screen::format_color(Color c) {
    return (c.r << rshift) | (c.g << gshift) | (c.b << bshift);
}

void Screen::commit_screen() {
    SDL_UpdateTexture(texture, NULL, pixels, width * sizeof (Uint32));
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

bool Screen::on_screen(int x, int y) {
    return (x >= 0) && (x < width) && (y >= 0) && (y < height);
}

void Screen::set_color(Uint8 r, Uint8 g, Uint8 b) {
    Color c{r, g, b};
    default_color = format_color(c);
}

void Screen::set_color(Color c) {
    default_color = format_color(c);
}

void Screen::set_pixel(int x, int y) {
    pixel_at(x, y) = default_color;
}

void Screen::set_pixel(int x, int y, Color c) {
    pixel_at(x, y) = format_color(c);
}

void Screen::cls() {
    memset(pixels, 0, width * height * sizeof(Uint32));
}

void Screen::fill_screen() {
    fill_rect_helper(0, 0, width - 1, height - 1, default_color);
}

void Screen::fill_screen(Color c) {
    fill_rect_helper(0, 0, width - 1, height - 1, format_color(c));
}

void Screen::hor_line_helper(int y, int x1, int x2, Uint32 c) {
    int diff = x2 > x1? 1: -1;
    while (true) {
        pixel_at(x1, y) = c;
        if (x1 == x2) {
            break;
        }
        x1 += diff;
    }
}

// Prefer drawing horizontal lines to vertical ones
void Screen::hor_line(int y, int x1, int x2) {
    hor_line_helper(y, x1, x2, default_color);
}

void Screen::hor_line(int y, int x1, int x2, Color c) {
    hor_line_helper(y, x1, x2, format_color(c));
}

void Screen::ver_line_helper(int x, int y1, int y2, Uint32 c) {
    int diff = y2 > y1? 1: -1;
    while (true) {
        pixel_at(x, y1) = c;
        if (y1 == y2) {
            break;
        }
        y1 += diff;
    }
}

void Screen::ver_line(int x, int y1, int y2) {
    ver_line_helper(x, y1, y2, default_color);
}

void Screen::ver_line(int x, int y1, int y2, Color c) {
    ver_line_helper(x, y1, y2, format_color(c));
}

void Screen::draw_rect_helper(int x1, int y1, int x2, int y2, Uint32 c) {
    hor_line_helper(y1, x1, x2, c);
    hor_line_helper(y2, x1, x2, c);
    ver_line_helper(x1, y1, y2, c);
    ver_line_helper(x2, y1, y2, c);
}

void Screen::draw_rect(int x1, int y1, int x2, int y2) {
    draw_rect_helper(x1, y1, x2, y2, default_color);
}

void Screen::draw_rect(int x1, int y1, int x2, int y2, Color c) {
    draw_rect_helper(x1, y1, x2, y2, format_color(c));
}

void Screen::fill_rect_helper(int x1, int y1, int x2, int y2, Uint32 c) {
    int diff = y2 > y1 ? 1: -1;
    while (true) {
        hor_line_helper(y1, x1, x2, c);
        if (y1 == y2) {
            break;
        }
        y1 += diff;
    }
}

void Screen::fill_rect(int x1, int y1, int x2, int y2) {
    fill_rect_helper(x1, y1, x2, y2, default_color);
}

void Screen::fill_rect(int x1, int y1, int x2, int y2, Color c) {
    fill_rect_helper(x1, y1, x2, y2, format_color(c));
}

// Code taken from Rosetta Code
void Screen::draw_line_helper(int x1, int y1, int x2, int y2, Uint32 c) {
    int dx = abs(x2 - x1), sx = x1 < x2 ? 1 : -1;
    int dy = abs(y2 - y1), sy = y1 < y2 ? 1 : -1; 
    int err = (dx > dy ? dx : -dy) / 2, e2;
    while (true) {
        pixel_at(x1, y1) = c;
        if (x1 == x2 && y1 == y2) {
            break;
        }
        e2 = err;
        if (e2 > -dx) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dy) {
            err += dx;
            y1 += sy;
        }
    }
}

void Screen::draw_line(int x1, int y1, int x2, int y2) {
    draw_line_helper(x1, y1, x2, y2, default_color);
}

void Screen::draw_line(int x1, int y1, int x2, int y2, Color c) {
    draw_line_helper(x1, y1, x2, y2, format_color(c));
}

void Screen::draw_circle_helper(int x, int y, int r, Uint32 c) {
}

void Screen::draw_circle(int x, int y, int r) {
    draw_circle_helper(x, y, r, default_color);
}

void Screen::draw_circle(int x, int y, int r, Color c) {
    draw_circle_helper(x, y, r, format_color(c));
}

void Screen::fill_circle_helper(int x, int y, int r, Uint32 c) {
}

void Screen::fill_circle(int x, int y, int r) {
    fill_circle_helper(x, y, r, default_color);
}

void Screen::fill_circle(int x, int y, int r, Color c) {
    fill_circle_helper(x, y, r, format_color(c));
}

