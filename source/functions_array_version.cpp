#include <SFML/Graphics.hpp>
#include <stdio.h>
#include <sstream>

#include "functions.h"

const int width = 1200;
const int height = 1200;
const int max_iterations = 256;
const int max_distance = 4;
const int size = 4;
// const double scale = 0.003f;
// const double scale_y = 0.003f;
// const double offset_x = 0.0f;
// const double offset_y = 0.0f;
static sf::Color getColor(int iterations);
static void check_mandelbrot(double *X, double *Y, int *N);

static void check_mandelbrot(double *X, double *Y, int *N)
{
    double start_x[4] = {};
    for (int i = 0; i < size; i++)
    {
        start_x[i] = X[i];
    }

    double start_y = Y[0];

    double X2[4] = {0};
    double Y2[4] = {0};
    double XY2[4] = {0};
    double R2[4] = {0};
    for (int counter = 0; counter <= max_iterations; counter++)
    {
        //int mask = 0;
        bool flag = true;
        for (int i = 0; i < size; i++)
        {
            if (N[i] == max_iterations)
            {
                continue;
            }
            X2[i] = X[i] * X[i];
            Y2[i] = Y[i] * Y[i];
            XY2[i] = X[i] * Y[i] * 2;
            R2[i] = X2[i] + Y2[i];
            if (R2[i] <= max_distance)
            {
                flag = false;
                N[i]++;
                X[i] = X2[i] - Y2[i] + start_x[i];
                Y[i] = XY2[i] + start_y;
            }
            else
            {
                if (N[i] < counter)
                {
                    N[i] = counter;
                }
            }
            
        }
        if (flag)
        {
            break;
        }
        // if (!mask)
        // {
        //     for (int j = 0; j < size; j++)
        //     {
        //         N[j] = max_iterations;
        //     }
        //     return;
        // }

        // for (int i = 0; i < size; i++)
        // {
        //     if (mask & (1 << i)) 
        //     {
        //         N[i]++;
        //         X[i] = X2[i] - Y2[i] + start_x[i];
        //         Y[i] = XY2[i] + start_y;
        //     }
        // }

    }

    for (int i = 0; i < size; i++) 
    {
        if (N[i] >= max_iterations) 
        {
            N[i] = max_iterations;
        }
    }

    
    return;
}

static sf::Color getColor(int iterations) 
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

Errors mandelbrot_main_function_arrays()
{
    //double scale = 0.003f;
    double zoom_x = 4.0f;
    double zoom_y = -4.0f;
    double offset_x = 0.0f;
    double offset_y = 0.0f;    
    sf::RenderWindow window(sf::VideoMode(width, height), "Mandelbrot System");
    window.setFramerateLimit(30);

    sf::Image image;
    image.create(width, height, sf::Color::Black);

    sf::Font font;
    if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
        return ERROR_OF_GET_FONT;
    }

    sf::Texture texture;
    sf::Sprite sprite;
    double center_x = width / 2;
    double center_y = height / 2;

    sf::Text fpsText;
    fpsText.setFont(font);
    fpsText.setCharacterSize(24);
    fpsText.setFillColor(sf::Color::White);
    fpsText.setPosition(10, 10);

    sf::Clock clock;
    double last_time = 0;
    int frame_count = 0;


    while (window.isOpen()) 
    {
        sf::Event event;
        while (window.pollEvent(event)) 
        {
            if (event.type == sf::Event::Closed) 
            {
                window.close();
            }
            if (event.type == sf::Event::KeyPressed) {
                switch (event.key.code) {
                    case sf::Keyboard::Up: offset_y += 0.1; break;
                    case sf::Keyboard::Down: offset_y -= 0.1; break;
                    case sf::Keyboard::Right: offset_x -= 0.1; break;
                    case sf::Keyboard::Left: offset_x += 0.1; break;
                    case sf::Keyboard::Equal:
                        if (event.key.control) 
                        { 
                            zoom_x *= 0.7; 
                            zoom_y *= 0.7; 
                        }
                        break;
                    case sf::Keyboard::Hyphen:
                        if (event.key.control) 
                        { 
                            zoom_x *= 1.3; 
                            zoom_y *= 1.3; 
                        }
                        break;
                    default: break;
                }
            }
        }
        for (int iy = 0; iy < height; iy++)
        {
            for (int ix = 0; ix < width; ix += 4)
            {
                const double scale_x = zoom_x / (double)width;
                const double scale_y = zoom_y / (double)height;
                double x = (ix - center_x) * scale_x + offset_x;
                double y = (iy - center_y) * scale_y + offset_y;
                double X[4] = {x, x + scale_x, x + 2 * scale_x, x + 3 * scale_x};
                double Y[4] = {y, y, y, y};
                int N[4] = {0};
                check_mandelbrot(X, Y, N);
                for (int i = 0; i < size; i++)
                {
                    int iterations = N[i];
                    image.setPixel(ix + i, iy, getColor(iterations));
                }
            }
        }
        
        if (!texture.loadFromImage(image))
        {
            return ERROR_OF_DRAW_PICTURE;
        }

        sprite.setTexture(texture);
        
        double current_time = clock.getElapsedTime().asSeconds();
        frame_count++;
        if (current_time - last_time >= 1.0f) 
        {
            double fps = frame_count / (current_time - last_time);
            frame_count = 0;
            last_time = current_time;

            std::stringstream ss;
            ss << "FPS: " << static_cast<int>(fps);
            fpsText.setString(ss.str());
        }

        window.clear();
        window.draw(sprite);
        window.draw(fpsText);
        window.display();
    }
    return NO_ERRORS;
}
