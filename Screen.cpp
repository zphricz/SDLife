#include "Screen.h"

/*
QuickCG 20071121

Copyright (c) 2004-2007, Lode Vandevenne

All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*
QuickCG is an SDL codebase that wraps some of the SDL functionality.
It's used by Lode's Computer Graphics Tutorial to work with simple function calls
to demonstrate graphical programs. It may or may not be of industrial strength
for games, though I've actually used it for some.

QuickCG can handle some things that standard C++ does not but that are useful, such as:
-drawing graphics
-a bitmap font
-simplified saving and loading of files
-reading keyboard and mouse input
-playing sound
-color models
-loading images

Contact info:
My email address is (puzzle the account and domain together with an @ symbol):
Domain: gmail dot com.
Account: lode dot vandevenne.
*/

Screen::Screen(int w, int h, bool full, std::string text) :
    width(w),
    height(h) {
    int colorDepth = 32;
    if (full)
    {
        scr = SDL_SetVideoMode(width, height, colorDepth, SDL_SWSURFACE | SDL_FULLSCREEN);
    }
    else
    {
        scr = SDL_SetVideoMode(width, height, colorDepth, SDL_HWSURFACE | SDL_HWPALETTE);
    }
    if(scr == NULL)
    {
        printf("Unable to set video: %s\n", SDL_GetError());
        std::exit(1);
    }
    if(SDL_MUSTLOCK(scr)) {
        SDL_LockSurface(scr);
    }
    SDL_WM_SetCaption(text.c_str(), NULL);

    SDL_EnableUNICODE(1); //for the text input things
}

Screen::~Screen() {
    if(SDL_MUSTLOCK(scr))
        SDL_UnlockSurface(scr);
}

void Screen::redraw() {
    SDL_UpdateRect(scr, 0, 0, 0, 0);
}

void Screen::cls(const ColorRGB& color) {
    SDL_FillRect(scr, NULL, 65536 * color.r + 256 * color.g + color.b);
}

void Screen::setColor(const ColorRGB& color) {
    colorSDL = SDL_MapRGB(scr->format, color.r, color.g, color.b);
}

void Screen::pset(int x, int y) {
    //if(x < 0 || y < 0 || x >= w || y >= h) return;
    Uint32* bufp = (Uint32*)scr->pixels + y * scr->pitch / 4 + x;
    *bufp = colorSDL;
}

void Screen::pset(int x, int y, const ColorRGB& color) {
    //if(x < 0 || y < 0 || x >= w || y >= h) return;
    Uint32* bufp = (Uint32*)scr->pixels + y * scr->pitch / 4 + x;
    *bufp = SDL_MapRGB(scr->format, color.r, color.g, color.b);
}


ColorRGB Screen::pget(int x, int y) {
    //if(x < 0 || y < 0 || x >= w || y >= h) return RGB_Black;
    Uint32* bufp;
    bufp = (Uint32*)scr->pixels + y * scr->pitch / 4 + x;
    Uint32 color_SDL = *bufp;
    ColorRGB8bit colorRGB;
    SDL_GetRGB(color_SDL, scr->format, &colorRGB.r, &colorRGB.g, &colorRGB.b);
    return ColorRGB(colorRGB);
}

void Screen::drawBuffer(Uint32* buffer) {
    Uint32* bufp;
    bufp = (Uint32*)scr->pixels;

    for(int y = 0; y < height; y++)
    {
        for(int x = 0; x < width; x++)
        {
            *bufp=buffer[height * x + y];
            bufp++;
        }
        bufp += scr->pitch / 4;
        bufp -= width;
    }
}

void Screen::getScreenBuffer(std::vector<Uint32>& buffer) {
    Uint32* bufp;
    bufp = (Uint32*)scr->pixels;

    buffer.resize(width * height);

    for(int y = 0; y < height; y++)
    {
        for(int x = 0; x < width; x++)
        {
            buffer[height * x + y] = *bufp;
            bufp++;
        }
        bufp += scr->pitch / 4;
        bufp -= width;
    }
}

bool Screen::onScreen(int x, int y) {
    return (x >= 0 && y >= 0 && x < height && y < height);
}

//Fast horizontal line from (x1,y) to (x2,y), with rgb color
bool Screen::horLine(int y, int x1, int x2) {
    if(x2 < x1)
    {
        x1 += x2;    //swap x1 and x2, x1 must be the leftmost endpoint
        x2 = x1 - x2;
        x1 -= x2;
    }
    if(x2 < 0 || x1 >= width || y < 0 || y >= height) return 0; //no single point of the line is on screen
    if(x1 < 0) x1 = 0; //clip
    if(x2 >= width) x2 = width - 1; //clip

    Uint32* bufp;
    bufp = (Uint32*)scr->pixels + y * scr->pitch / 4 + x1;
    for(int x = x1; x <= x2; x++)
    {
        *bufp = colorSDL;
        bufp++;
    }
    return 1;
}

//Fast vertical line from (x,y1) to (x,y2), with rgb color
bool Screen::verLine(int x, int y1, int y2) {
    if(y2 < y1)
    {
        y1 += y2;    //swap y1 and y2
        y2 = y1 - y2;
        y1 -= y2;
    }
    if(y2 < 0 || y1 >= height  || x < 0 || x >= width) return 0; //no single point of the line is on screen
    if(y1 < 0) y1 = 0; //clip
    if(y2 >= width) y2 = height - 1; //clip

    Uint32* bufp;

    bufp = (Uint32*)scr->pixels + y1 * scr->pitch / 4 + x;
    for(int y = y1; y <= y2; y++)
    {
        *bufp = colorSDL;
        bufp += scr->pitch / 4;
    }
    return 1;
}

//Bresenham line from (x1,y1) to (x2,y2) with rgb color
bool Screen::drawLine(int x1, int y1, int x2, int y2) {
    //if(x1 < 0 || x1 > w - 1 || x2 < 0 || x2 > w - 1 || y1 < 0 || y1 > h - 1 || y2 < 0 || y2 > h - 1) return 0;

    int deltax = std::abs(x2 - x1); //The difference between the x's
    int deltay = std::abs(y2 - y1); //The difference between the y's
    int x = x1; //Start x off at the first pixel
    int y = y1; //Start y off at the first pixel
    int xinc1, xinc2, yinc1, yinc2, den, num, numadd, numpixels, curpixel;

    if(x2 >= x1) //The x-values are increasing
    {
        xinc1 = 1;
        xinc2 = 1;
    }
    else //The x-values are decreasing
    {
        xinc1 = -1;
        xinc2 = -1;
    }
    if(y2 >= y1) //The y-values are increasing
    {
        yinc1 = 1;
        yinc2 = 1;
    }
    else //The y-values are decreasing
    {
        yinc1 = -1;
        yinc2 = -1;
    }
    if (deltax >= deltay) //There is at least one x-value for every y-value
    {
        xinc1 = 0; //Don't change the x when numerator >= denominator
        yinc2 = 0; //Don't change the y for every iteration
        den = deltax;
        num = deltax / 2;
        numadd = deltay;
        numpixels = deltax; //There are more x-values than y-values
    }
    else //There is at least one y-value for every x-value
    {
        xinc2 = 0; //Don't change the x for every iteration
        yinc1 = 0; //Don't change the y when numerator >= denominator
        den = deltay;
        num = deltay / 2;
        numadd = deltax;
        numpixels = deltay; //There are more y-values than x-values
    }
    for (curpixel = 0; curpixel <= numpixels; curpixel++)
    {
        pset(x % width, y % height);  //Draw the current pixel
        num += numadd;  //Increase the numerator by the top of the fraction
        if (num >= den) //Check if numerator >= denominator
        {
            num -= den; //Calculate the new numerator value
            x += xinc1; //Change the x as appropriate
            y += yinc1; //Change the y as appropriate
        }
        x += xinc2; //Change the x as appropriate
        y += yinc2; //Change the y as appropriate
    }

    return 1;
}

//Bresenham circle with center at (xc,yc) with radius and red green blue color
bool Screen::drawCircle(int xc, int yc, int radius) {
    //if(xc - radius < 0 || xc + radius >= w || yc - radius < 0 || yc + radius >= h) return 0;
    int x = 0;
    int y = radius;
    int p = 3 - (radius << 1);
    int a, b, c, d, e, f, g, h;
    while (x <= y)
    {
        a = xc + x; //8 pixels can be calculated at once thanks to the symmetry
        b = yc + y;
        c = xc - x;
        d = yc - y;
        e = xc + y;
        f = yc + x;
        g = xc - y;
        h = yc - x;
        pset(a, b);
        pset(c, d);
        pset(e, f);
        pset(g, f);
        if(x > 0) //avoid drawing pixels at same position as the other ones
        {
            pset(a, d);
            pset(c, b);
            pset(e, h);
            pset(g, h);
        }
        if(p < 0) p += (x++ << 2) + 6;
        else p += ((x++ - y--) << 2) + 10;
    }

    return 1;
}

//Filled bresenham circle with center at (xc,yc) with radius and red green blue color
bool Screen::drawDisk(int xc, int yc, int radius) {
    //if(xc + radius < 0 || xc - radius >= w || yc + radius < 0 || yc - radius >= h) return 0; //every single pixel outside screen, so don't waste time on it
    int x = 0;
    int y = radius;
    int p = 3 - (radius << 1);
    int a, b, c, d, e, f, g, h;
    int pb = yc + radius + 1, pd = yc + radius + 1; //previous values: to avoid drawing horizontal lines multiple times  (ensure initial value is outside the range)
    while (x <= y)
    {
        // write data
        a = xc + x;
        b = yc + y;
        c = xc - x;
        d = yc - y;
        e = xc + y;
        f = yc + x;
        g = xc - y;
        h = yc - x;
        if(b != pb) horLine(b, a, c);
        if(d != pd) horLine(d, a, c);
        if(f != b)  horLine(f, e, g);
        if(h != d && h != f) horLine(h, e, g);
        pb = b;
        pd = d;
        if(p < 0) p += (x++ << 2) + 6;
        else p += ((x++ - y--) << 2) + 10;
    }

    return 1;
}

//Rectangle with corners (x1,y1) and (x2,y2) and rgb color
bool Screen::drawRect(int x1, int y1, int x2, int y2) {
    //if(x1 < 0 || x1 > w - 1 || x2 < 0 || x2 > w - 1 || y1 < 0 || y1 > h - 1 || y2 < 0 || y2 > h - 1) return 0;
    SDL_Rect rec;
    rec.x = x1;
    rec.y = y1;
    rec.w = x2 - x1 + 1;
    rec.h = y2 - y1 + 1;
    SDL_FillRect(scr, &rec, colorSDL);  //SDL's ability to draw a hardware rectangle is used for now
    return 1;
}

//Functions for clipping a 2D line to the screen, which is the rectangle (0,0)-(w,h)
//This is the Cohen-Sutherland Clipping Algorithm
//Each of 9 regions gets an outcode, based on if it's at the top, bottom, left or right of the screen
// 1001 1000 1010  9 8 10
// 0001 0000 0010  1 0 2
// 0101 0100 0110  5 4 6
//int findregion returns which of the 9 regions a point is in, void clipline does the actual clipping
int Screen::findRegion(int x, int y) {
    int code=0;
    if(y >= height)
        code |= 1; //top
    else if( y < 0)
        code |= 2; //bottom
    if(x >= width)
        code |= 4; //right
    else if (x < 0)
        code |= 8; //left
    return(code);
}

bool Screen::clipLine(int x1, int y1, int x2, int y2, int & x3, int & y3, int & x4, int & y4) {
    int code1, code2, codeout;
    bool accept = 0, done=0;
    code1 = findRegion(x1, y1); //the region outcodes for the endpoints
    code2 = findRegion(x2, y2);
    do  //In theory, this can never end up in an infinite loop, it'll always come in one of the trivial cases eventually
    {
        if(!(code1 | code2)) accept = done = 1;  //accept because both endpoints are in screen or on the border, trivial accept
        else if(code1 & code2) done = 1; //the line isn't visible on screen, trivial reject
        else  //if no trivial reject or accept, continue the loop
        {
            int x, y;
            codeout = code1 ? code1 : code2;
            if(codeout & 1) //top
            {
                x = x1 + (x2 - x1) * (height - y1) / (y2 - y1);
                y = height - 1;
            }
            else if(codeout & 2) //bottom
            {
                x = x1 + (x2 - x1) * -y1 / (y2 - y1);
                y = 0;
            }
            else if(codeout & 4) //right
            {
                y = y1 + (y2 - y1) * (width - x1) / (x2 - x1);
                x = width - 1;
            }
            else //left
            {
                y = y1 + (y2 - y1) * -x1 / (x2 - x1);
                x = 0;
            }
            if(codeout == code1) //first endpoint was clipped
            {
                x1 = x;
                y1 = y;
                code1 = findRegion(x1, y1);
            }
            else //second endpoint was clipped
            {
                x2 = x;
                y2 = y;
                code2 = findRegion(x2, y2);
            }
        }
    }
    while(done == 0);

    if(accept)
    {
        x3 = x1;
        x4 = x2;
        y3 = y1;
        y4 = y2;
        return 1;
    }
    else
    {
        x3 = x4 = y3 = y4 = 0;
        return 0;
    }
}

//Draws character n at position x,y with color RGB and, if enabled, background color
//This function is used by the text printing functions below, and uses the font data
//defined below to draw the letter pixel by pixel
void Screen::drawLetter(unsigned char n, int x, int y, const ColorRGB& color, bool bg, const ColorRGB& color2) {
    int u,v;

    for (v = 0; v < 8; v++)
        for (u = 0; u < 8; u++)
        {
            if(font[n][u][v]) pset(x + u, y + v, color);
            else if(bg) pset(x + u, y + v, color2);
        }
}

//Draws a string of text
int Screen::printString(const std::string& text, int x, int y, const ColorRGB& color, bool bg, const ColorRGB& color2, int forceLength) {
    int amount = 0;
    for(size_t i = 0; i < text.size(); i++) {
        amount++;
        drawLetter(text[i], x, y, color, bg, color2);
        x += 8;
        if(x > width - 8) {
            x %= 8;
            y += 8;
        }
        if(y > height - 8) {
            y %= 8;
        }
    }
    while(amount < forceLength) {
        amount++;
        drawLetter(' ', x, y, color, bg, color2);
        x += 8;
        if(x > width - 8) {
            x %= 8;
            y += 8;
        }
        if(y > height - 8) {
            y %= 8;
        }
    }
    return height * x + y;
}

