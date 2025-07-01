#include <SFML/Graphics.hpp>
#include <stdio.h>

#include "functions.h"

sf::Color get_color(int iterations) 
{
    int r = 0;
    int g = 0;
    int b = 0;

    if (iterations == max_iterations) {
        r = 0;
        g = 0;
        b = 0;
    } else if (iterations == 0) {
        r = 255;
        g = 0;
        b = 0;
    } else {
        // colour gradient:      Red -> Blue -> Green -> Red -> Black
        // corresponding values:  0  ->  16  ->  64   -> 128  ->  256 (or -1)
        if (iterations < 16) {
            r = 16 * (16 - iterations);
            g = 0;
            b = 16 * iterations - 1;
        } else if (iterations < 64) {
            r = 0;
            g = 16 * (iterations - 16);
            b = 16 * (64 - iterations) - 1;
        } else if (iterations < 128) {
            r = 8 * (iterations - 64);
            g = 8 * (128 - iterations) - 1;
            b = 0;
        } else { // range is 128 - 256
            r = 255 - (iterations - 128) * 4;
            g = 0;
            b = 0;
        }
    }

    return sf::Color(r, g, b);
}