#ifndef SCREEN_H
#define SCREEN_H

#include <string>
#include "quickcg.h"

using namespace QuickCG;

using std::string;

class Screen
{
    private:
        SDL_Surface * scr;
        Uint32 colorSDL;

        void drawLetter(unsigned char n, int x, int y, const ColorRGB& color, bool bg, const ColorRGB& color2);
        int printString(const std::string& text, int x, int y, const ColorRGB& color, bool bg, const ColorRGB& color2, int forceLength);
    public:
        const int width;
        const int height;
        Screen(int w, int h, bool full, string name);
        ~Screen();
        void redraw();
        void cls(const ColorRGB& color = RGB_Black);
        void setColor(const ColorRGB& color);
        void pset(int x, int y);
        void pset(int x, int y, const ColorRGB& color);
        ColorRGB pget(int x, int y);
        void drawBuffer(Uint32* buffer);
        void getScreenBuffer(std::vector<Uint32>& buffer);
        bool onScreen(int x, int y);
        bool horLine(int y, int x1, int x2);
        bool verLine(int x, int y1, int y2);
        bool drawLine(int x1, int y1, int x2, int y2);
        bool drawCircle(int xc, int yc, int radius);
        bool drawDisk(int xc, int yc, int radius);
        bool drawRect(int x1, int y1, int x2, int y2);
        int findRegion(int x, int y);
        bool clipLine(int x1, int y1, int x2, int y2, int & x3, int & y3, int & x4, int & y4);

        template<typename T>
        int print(const T& val, int x = 0, int y = 0, const ColorRGB& color = RGB_White, bool bg = 0, const ColorRGB& color2 = RGB_Black, int forceLength = 0) {
            std::string text = valtostr(val);
            return printString(text, x, y, color, bg, color2, forceLength);
        }
};

#endif
