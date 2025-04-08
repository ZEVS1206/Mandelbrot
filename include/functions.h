#ifndef FUNCTIONS_H
#define FUNCTIONS_H

enum Errors
{
    NO_ERRORS             = 0,
    ERROR_OF_DRAW_PICTURE = 1,
    ERROR_OF_GET_FONT     = 2,
};

Errors mandelbrot_main_function_without_optimiztion();
Errors mandelbrot_main_function_arrays();
Errors mandelbrot_main_function_instructions();

//void sprite_moving(); 

#endif