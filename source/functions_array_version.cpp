#include <SFML/Graphics.hpp>
#include <stdio.h>
#include <sstream>
#include <immintrin.h>

#include "functions.h"


static void check_mandelbrot(double *X, double *Y, int *N);

inline uint64_t rdtsc() 
{
    return __rdtsc();
}

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



Errors mandelbrot_main_function_arrays()
{
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
    

    sf::Text fpsText;
    fpsText.setFont(font);
    fpsText.setCharacterSize(24);
    fpsText.setFillColor(sf::Color::White);
    fpsText.setPosition(10, 10);

    sf::Clock clock;
    double last_time = 0;
    int frame_count = 0;
    uint64_t total_ticks = 0;


    while (window.isOpen()) 
    {
        uint64_t start_ticks = rdtsc();

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
                    image.setPixel(ix + i, iy, get_color(iterations));
                }
            }
        }
        
        if (!texture.loadFromImage(image))
        {
            return ERROR_OF_DRAW_PICTURE;
        }

        sprite.setTexture(texture);

        uint64_t end_ticks = rdtsc();
        uint64_t frame_ticks = end_ticks - start_ticks;
        total_ticks += frame_ticks;
        
        double current_time = clock.getElapsedTime().asSeconds();
        frame_count++;
        if (current_time - last_time >= 1.0f) 
        {
            double fps = frame_count / (current_time - last_time);
            float avg_ticks_per_frame = static_cast<float>(total_ticks) / frame_count;
            frame_count = 0;
            total_ticks = 0;
            last_time = current_time;

            std::stringstream ss;
            ss << "FPS: " << static_cast<int>(fps) << "\n";
            ss << "Ticks/frame: " << static_cast<uint64_t>(avg_ticks_per_frame);
            fpsText.setString(ss.str());
        }

        window.clear();
        window.draw(sprite);
        window.draw(fpsText);
        window.display();
    }
    return NO_ERRORS;
}
