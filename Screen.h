#ifndef SCREEN_H
#define SCREEN_H

#include <SDL2/SDL.h>

#define VSYNC 1

struct Color {
    Uint8 r;
    Uint8 g;
    Uint8 b;
};

class Screen {
    private:
        Uint32* const pixels;
        Uint32 default_color;
        SDL_Window* window;
        SDL_Renderer* renderer;
        SDL_Texture* texture;

        inline Uint32& pixel_at(int x, int y);
        inline Uint32 format_color(Color c);
        void hor_line_helper(int y, int x1, int x2, Uint32 c);
        void ver_line_helper(int x, int y1, int y2, Uint32 c);
        void draw_rect_helper(int x1, int y1, int x2, int y2, Uint32 c);
        void fill_rect_helper(int x1, int y1, int x2, int y2, Uint32 c);
        void draw_line_helper(int x1, int y1, int x2, int y2, Uint32 c);
        void draw_circle_helper(int x, int y, int r, Uint32 c);
        void fill_circle_helper(int x, int y, int r, Uint32 c);

    public:
        const int width;
        const int height;
        const int rshift;
        const int gshift;
        const int bshift;

        Screen(int sx, int sy, bool fs, const char * name);
        ~Screen();

        void commit_screen();
        bool on_screen(int x, int y);
        void set_color(Uint8 r, Uint8 g, Uint8 b);
        void set_color(Color c);
        void set_pixel(int x, int y);
        void set_pixel(int x, int y, Color c);
        void cls();
        void fill_screen();
        void fill_screen(Color c);
        void hor_line(int y, int x1, int x2);
        void hor_line(int y, int x1, int x2, Color c);
        void ver_line(int x, int y1, int y2);
        void ver_line(int x, int y1, int y2, Color c);
        void draw_rect(int x1, int y1, int x2, int y2);
        void draw_rect(int x1, int y1, int x2, int y2, Color c);
        void fill_rect(int x1, int y1, int x2, int y2);
        void fill_rect(int x1, int y1, int x2, int y2, Color c);
        void draw_line(int x1, int y1, int x2, int y2);
        void draw_line(int x1, int y1, int x2, int y2, Color c);
        void draw_circle(int x, int y, int r);
        void draw_circle(int x, int y, int r, Color c);
        void fill_circle(int x, int y, int r);
        void fill_circle(int x, int y, int r, Color c);
};

#endif
