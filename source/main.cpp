#include <SFML/Graphics.hpp>

#include "functions.h"

int main() 
{
    //Errors error = mandelbrot_main_function_without_optimiztion();
    //Errors error = mandelbrot_main_function_arrays();
    Errors error = mandelbrot_main_function_instructions();
    if (error != NO_ERRORS)
    {
        fprintf(stderr, "error = %d\n", error);
        return 1;
    }

    return 0;
}
