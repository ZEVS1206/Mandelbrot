# Mandelbrot Set Optimization Project

## Table of Contents

1. [Description](#description)
2. [Execution speed](#execution-speed)
   - [CPU ticks (compiler & prog versions)](#cpu-ticks-compiler--prog-versions)
   - [FPS (compiler & prog versions)](#fps-compiler--prog-versions)
3. [Profiler](#profiler)
4. [AVX acceleration](#avx-acceleration)
   - [How it works](#how-it-works)
   - [Intel AVX functions used in the project](#intel-avx-functions-used-in-the-project)

## Description

The Mandelbrot project was created to show how much you can influence code optimization with the  
help of different optimizations, in particular, vectorization is realized here.  
The project consists of three parts:  

1. A naive [version](src/buterbrod_spd0.cpp) of Mandelbrot set drawing.
2. The [first version](src/buterbrod_spd1.cpp) of the vectorization implementation.
3. The [second version](src/buterbrod_spd2.0.cpp) or another [second version](src/buterbrod_spd2myIntr.cpp)  
with my inline intrinsics of the vectorization implementation, sweep factor 4×1.
4. The [third version](src/buterbrod_spd2.1.cpp) of the vectorization implementation, sweep factor 7×1.
5. The [final version](src/buterbrod_spd3trueIntr.cpp) of the vectorization implementation with real  
intrinsics library `<immintrin.h>`.

## Execution speed

### CPU ticks (compiler & prog versions)

Table shows `CPU ticks * 10^6` for each version of the program with different compilators.

| Compiler Flags       | Naive Version | First Version | Vectorized (`no real AVX`, `4×1`) | Vectorized (`no real AVX`, `8×1`) | Vectorized (`real AVX`) |
|----------------------|---------------|---------------|---------------|---------------|---------------|
| `g++ -O2`           | 108.08 +- 3.13 | 70.60 +- 5.23 | 54.28 +- 7.75 | 50.76 +- 6.78 | 46.35 +- 3.94 |
| `g++ -O3`           | 107.59 +- 2.11 | 61.88 +- 8.63 | 59.10 +- 6.63 | 72.31 +- 6.81 | 47.58 +- 5.39 |
| `clang++ -O2`       | 109.76 +- 2.77 | 55.12 +- 6.43 | 45.07 +- 3.15 | 33.35 +- 2.89 | 45.88 +- 4.16 |
| `clang++ -O3`       | 111.23 +- 2.69 | 54.34 +- 5.75 | 47.05 +- 3.47 | 32.94 +- 3.38 | 45.49 +- 4.63 |

![Diagram](imgs/cpuTicksDiagram.svg)

### FPS (compiler & prog versions)

> [!NOTE]
> The speed of the program is measured in fps (frames per second).

Distribution: `Ubuntu 24.10`  
Linux version: `Linux version 6.11.0`  
CPU: `Intel Core i5 9300H (2.40 GHz)`  

| Compiler Flags       | Naive Version | First Version | Vectorized (`no real AVX`, `4×1`) | Vectorized (`no real AVX`, `7×1`) | Vectorized (`real AVX`) |
|----------------------|---------------|---------------|----------------------------------|----------------------------------|------------------------|
| `g++ -O2`           | 22-23         | 39            | 52-53                            | 17                               | 59-60                  |
| `g++ -O3`           | 23            | 50-51         | 47-48                            | 42                               | 60-61                  |
| `clang++ -O2`       | 21-22         | 53            | 59-60                            | 78-79                            | 60-61                  |
| `clang++ -O3`       | 22            | 53            | 58-60                            | 81                               | 60                     |

![Diagram](imgs/fpsDiagram.svg)
  
CPU: `Ryzen 9 5900H (4.60 GHz)`

| Compiler Flags       | Naive Version | First Version | Vectorized (`no real AVX`, `4×1`) | Vectorized (`no real AVX`, `7×1`) | Vectorized (`real AVX`) |
|----------------------|---------------|---------------|----------------------------------|----------------------------------|------------------------|
| `g++ -O2`           | 35         | 52-53            |  84-86                        | 26                                 | 86-87                  |
| `g++ -O3`           | 35            | 75        | 61-62                             | 57-58                              | 89-90                  |
| `clang++ -O2`       | 33-34         | 79-80            | 88-89                      | 106-107                            | 93                     |
| `clang++ -O3`       | 34            | 79-80            | 88-89                      | 107                                | 91                     |

### Profiler

Сpu ticks were measured using a simple profiler written by me to quickly calculate cpu ticks and the random error of the measured value as of the variance of the deviation.

The formula for the variance of deviation:

![Formula](imgs/deviation.png)

You can enable the profiler by using the -DON_STAT flag, and inserting a piece of code into your file:

```cpp
#ifdef ON_STAT
    #include "profiler.h"
    #define PROFILE_START_(name) profileStart(name);
    #define PROFILE_END_(name) profileEnd(name);
    #define PROFILE_SET_LIMIT_(limit) setProfileLimit(limit);
    #define PROFILE_INFO_ printStats();
#else
    #define PROFILE_START_(name)
    #define PROFILE_END_(name)
    #define PROFILE_SET_LIMIT_(limit)
    #define PROFILE_INFO_
#endif
```

In such a case, you can adjust the number of measurements of a piece of code like this:

```cpp
PROFILE_SET_LIMIT_(1000)
```

That would set a measurement limit of 1,000 pieces.  
To measure a piece of code you have to wrap it in the following:

```cpp
PROFILE_START_("funcName")
// function code
PROFILE_END_("funcName")
```

And the output of the statistics:

```cpp
PROFILE_INFO_
```

After that, you will see the following:

![progressBar](imgs/progressBar.gif)

## AVX acceleration

AVX (Advanced Vector Extensions) is a set of instructions for x86/x86-64 processors that enables parallel computations on multiple data points simultaneously (SIMD — Single Instruction, Multiple Data). In this program, AVX is utilized to accelerate Mandelbrot set calculations by processing 4 `double` values in a single operation.

### How it works

We reduce the number of iterations by sweeping cycles in a single iteration. We work with 4 points on the x-axis at once. This reduces the number of iterations from 600\*800 = 480000 to 600\*200 = 120000, which significantly improves the performance of the program.

![fastInit](imgs/fastInit.svg)
![cycleSweep](imgs/cycleSweep.svg)

### Intel AVX functions used in the project

> [!Note]
> You need the `-mavx` flag to make it work.

The table below lists the main AVX functions employed in the code:

| AVX Function Name          | AVX Function Parameters                      | Return Value            | Brief Description of What It Does                     |
|----------------------------|---------------------------------------------|-------------------------|-------------------------------------------------------|
| `_mm256_loadu_pd`          | `const double* ptr`                         | `__m256d`               | Loads 4 `double` values from memory into an AVX register |
| `_mm256_set1_pd`           | `double a`                                  | `__m256d`               | Creates an AVX register with 4 copies of a single value |
| `_mm256_mul_pd`            | `__m256d a, __m256d b`                      | `__m256d`               | Multiplies 4 pairs of `double` values element-wise    |
| `_mm256_add_pd`            | `__m256d a, __m256d b`                      | `__m256d`               | Adds 4 pairs of `double` values element-wise          |
| `_mm256_sub_pd`            | `__m256d a, __m256d b`                      | `__m256d`               | Subtracts 4 pairs of `double` values element-wise     |
| `_mm256_cmp_pd`            | `__m256d a, __m256d b, int imm8`            | `__m256d`               | Compares 4 pairs of `double` values (here, <=)        |
| `_mm256_movemask_pd`       | `__m256d a`                                 | `scalar bit mask (int)` | Converts comparison results into a bit mask           |
| `_mm_loadu_si128`          | `__m128i const* mem_addr`                   | `__m128i`               | Loads 4 `int` values into an SSE register             |
| `_mm_add_epi32`            | `__m128i a, __m128i b`                      | `__m128i`               | Adds 4 pairs of `int` values element-wise             |
| `_mm_storeu_si128`         | `__m128i* mem_addr, __m128i a`              | `void`                  | Stores 4 `int` values from a register into memory     |

- __m256d for 256-bit AVX registers holding 4 double values.  
- __m128i for 128-bit SSE registers holding 4 int values.  

These functions enable simultaneous computation on 4 points of the Mandelbrot set, significantly improving performance compared to sequential operations.
