#include <SFML/Graphics.hpp>
#include <stdio.h>
#include <sstream>
#include <immintrin.h>

#include "functions.h"

const int width = 1200;
const int height = 1200;
const int max_iterations = 256;
const int max_distance = 4;
static sf::Color getColor(int iterations);
static int check_mandelbrot(float x, float y);

inline uint64_t rdtsc() 
{
    return __rdtsc();
}

static int check_mandelbrot(float x, float y)
{
    float start_x = x;
    float start_y = y;
    int counter = 0;
    float x_2 = x * x;
    float y_2 = y * y;
    float xy2 = 2 * x * y;
    float next_x = 0;
    while (x_2 + y_2 <= 4.0f & counter <= max_iterations)
    {
        next_x = x_2 - y_2 + start_x;
        y = xy2 + start_y;
        x = next_x;
        x_2 = x * x;
        y_2 = y * y;
        xy2 = 2 * x * y;
        if (x == start_x && y == start_y)
        {
            return -1;
        }
        counter++;
    }
    if (counter >= max_iterations)
    {
        return -1;
    }
    return counter;
}

static sf::Color getColor(int iterations) 
{
    int r = 0;
    int g = 0;
    int b = 0;

    if (iterations == -1) {
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

Errors mandelbrot_main_function_without_optimiztion()
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
    float center_x = width / 2;
    float center_y = height / 2;

    sf::Text fpsText;
    fpsText.setFont(font);
    fpsText.setCharacterSize(24);
    fpsText.setFillColor(sf::Color::White);
    fpsText.setPosition(10, 10);

    sf::Clock clock;
    float last_time = 0;
    int frame_count = 0;
    uint64_t total_ticks = 0;


    while (window.isOpen()) {
        uint64_t start_ticks = rdtsc();
        
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::KeyPressed) {
                switch (event.key.code) {
                    case sf::Keyboard::Up: offset_y += 0.1; break;
                    case sf::Keyboard::Down: offset_y -= 0.1; break;
                    case sf::Keyboard::Right: offset_x -= 0.1; break;
                    case sf::Keyboard::Left: offset_x += 0.1; break;
                    case sf::Keyboard::Equal:
                        if (event.key.control) { 
                            zoom_x *= 0.7; 
                            zoom_y *= 0.7; 
                        }
                        break;
                    case sf::Keyboard::Hyphen:
                        if (event.key.control) { 
                            zoom_x *= 1.3; 
                            zoom_y *= 1.3; 
                        }
                        break;
                    default: break;
                }
            }
        }

        for (int ix = 0; ix < width; ix++) {
            for (int iy = 0; iy < height; iy++) {
                const double scale_x = zoom_x / (double)width;
                const double scale_y = zoom_y / (double)height;
                double x = (ix - center_x) * scale_x + offset_x;
                double y = (iy - center_y) * scale_y + offset_y;
                int iterations = check_mandelbrot(x, y);
                image.setPixel(ix, iy, getColor(iterations));
            }
        }
        
        if (!texture.loadFromImage(image)) {
            return ERROR_OF_DRAW_PICTURE;
        }

        sprite.setTexture(texture);
        
        uint64_t end_ticks = rdtsc();
        uint64_t frame_ticks = end_ticks - start_ticks;
        total_ticks += frame_ticks;

        float current_time = clock.getElapsedTime().asSeconds();
        frame_count++;
        if (current_time - last_time >= 1.0f) {
            float fps = frame_count / (current_time - last_time);
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

// void sprite_moving() 
// {
//     sf::RenderWindow window(sf::VideoMode(1024, 1024), "SFML Sprite Movement");

    
//     // sf::Texture texture;
//     // if (!texture.loadFromFile("source/car_2.png")) {
//     //     return; 
//     // }

//     sf::Font font;
//     if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
//         return;
//     }

//     sf::Text fpsText;
//     fpsText.setFont(font);
//     fpsText.setCharacterSize(24);
//     fpsText.setFillColor(sf::Color::White);
//     fpsText.setPosition(10, 10);

//     sf::Clock clock;
//     float lastTime = 0;
//     int frameCount = 0;
    
//     // sf::Sprite sprite(texture);
//     // sprite.setPosition(400, 300); 
//     // float speed = 5.0f; 

//     while (window.isOpen()) {
//         sf::Event event;
//         while (window.pollEvent(event)) 
//         {
//             if (event.type == sf::Event::Closed) 
//             {
//                 window.close();
//             }
//         }

        
//         // if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) 
//         // {
//         //     sprite.move(-speed, 0);
//         // }
//         // if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) 
//         // {
//         //     sprite.move(speed, 0);
//         // }
//         // if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) 
//         // {
//         //     sprite.move(0, -speed);
//         // }
//         // if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) 
//         // {
//         //     sprite.move(0, speed);
//         // }

//         float currentTime = clock.getElapsedTime().asSeconds();
//         frameCount++;
//         if (currentTime - lastTime >= 1.0f) 
//         {
//             float fps = frameCount / (currentTime - lastTime);
//             frameCount = 0;
//             lastTime = currentTime;

//             std::stringstream ss;
//             ss << "FPS: " << static_cast<int>(fps);
//             fpsText.setString(ss.str());
//         }

//         window.clear();
//         //window.draw(sprite);
//         window.draw(fpsText);
//         window.display();
//     }

//     return;
// }

// sf::RenderWindow window(sf::VideoMode(1200, 1000), "SFML Points Example");

    // // Создаем массив точек (тип sf::Points)
    // sf::VertexArray points(sf::Points, 5);

    // // Устанавливаем позиции и цвета точек
    // points[0].position = sf::Vector2f(100, 100);
    // points[0].color = sf::Color::Red;

    // points[1].position = sf::Vector2f(200, 150);
    // points[1].color = sf::Color::Green;

    // points[2].position = sf::Vector2f(300, 200);
    // points[2].color = sf::Color::Blue;

    // points[3].position = sf::Vector2f(400, 250);
    // points[3].color = sf::Color::Yellow;

    // points[4].position = sf::Vector2f(500, 300);
    // points[4].color = sf::Color::Magenta;

    // while (window.isOpen()) {
    //     sf::Event event;
    //     while (window.pollEvent(event)) {
    //         if (event.type == sf::Event::Closed) {
    //             window.close();
    //         }
    //     }

    //     window.clear();
    //     window.draw(points); // Рисуем точки
    //     window.display();
    // }