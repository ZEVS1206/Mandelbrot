#include <SFML/Graphics.hpp>
#include <stdio.h>
#include <sstream>
#include <immintrin.h>

#include "functions.h"

static void check_mandelbrot(double *X, double *Y, double *N);

inline uint64_t rdtsc() 
{
    return __rdtsc();
}

static void check_mandelbrot(double *X, double *Y, double *N)
{
    __m256d x = _mm256_loadu_pd(X);
    __m256d y = _mm256_loadu_pd(Y);
    __m256d start_x = x;
    __m256d start_y = y;
    
    __m256d n = _mm256_setzero_pd();
    __m256d one = _mm256_set1_pd(1.0);
    __m256d two = _mm256_set1_pd(2.0);
    __m256d max_dist = _mm256_set1_pd(max_distance);
    __m256d max_iter = _mm256_set1_pd(max_iterations);

    for (int counter = 0; counter < max_iterations; counter++)
    {
        __m256d x2 = _mm256_mul_pd(x, x);
        __m256d y2 = _mm256_mul_pd(y, y);
        __m256d r2 = _mm256_add_pd(x2, y2);
        
        __m256d mask = _mm256_cmp_pd(r2, max_dist, _CMP_LE_OS);
        
        if (_mm256_movemask_pd(mask) == 0) break;
        
        n = _mm256_add_pd(n, _mm256_and_pd(one, mask));
        
        __m256d xy = _mm256_mul_pd(x, y);
        x = _mm256_add_pd(_mm256_sub_pd(x2, y2), start_x);
        y = _mm256_add_pd(_mm256_mul_pd(xy, two), start_y);
    }
    
    __m256d in_mandelbrot_set = _mm256_cmp_pd(n, max_iter, _CMP_EQ_OS);
    n = _mm256_blendv_pd(n, max_iter, in_mandelbrot_set);//for choosing max_iterations or n
    
    _mm256_storeu_pd(N, n);
    return;
}


Errors mandelbrot_main_function_instructions()
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
                double N[4] = {0};
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
