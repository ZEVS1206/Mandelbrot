#include <SFML/Graphics.hpp>

#include "functions.h"

int main() {
    Errors error = mandelbrot_main_function();
    if (error != NO_ERRORS)
    {
        fprintf(stderr, "error = %d\n", error);
        return 1;
    }

    return 0;
}
