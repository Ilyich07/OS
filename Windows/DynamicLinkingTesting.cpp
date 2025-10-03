#include <iostream>
#include <windows.h>
#include <stdio.h>

typedef void* (*VECTOR_CREATE)(double, double);
typedef void(*VECTOR_DELETE)(void*);
typedef double(*VECTOR_GET_X)(void*);
typedef double(*VECTOR_GET_Y)(void*);
typedef double(*VECTOR_GET_RADIUS)(void*);
typedef double(*VECTOR_GET_THETA)(void*);
typedef void* (*VECTOR_ADD)(void*, void*);
typedef void* (* VECTOR_SUBTRACT)(void*, void*);

int main() {
    HINSTANCE hDll = NULL;

    hDll = LoadLibrary(TEXT("../x64/Debug/VectorLibrary.dll"));

    if (hDll != NULL) {


        VECTOR_CREATE vector_create = (VECTOR_CREATE)GetProcAddress(hDll, "vector_create");
        VECTOR_DELETE vector_delete = (VECTOR_DELETE)GetProcAddress(hDll, "vector_delete");
        VECTOR_GET_X vector_get_x = (VECTOR_GET_X)GetProcAddress(hDll, "vector_get_x");
        VECTOR_GET_Y vector_get_y = (VECTOR_GET_Y)GetProcAddress(hDll, "vector_get_y");
        VECTOR_GET_RADIUS vector_get_radius = (VECTOR_GET_RADIUS)GetProcAddress(hDll, "vector_get_radius");
        VECTOR_GET_THETA vector_get_theta = (VECTOR_GET_THETA)GetProcAddress(hDll, "vector_get_theta");
        VECTOR_ADD vector_add = (VECTOR_ADD)GetProcAddress(hDll, "vector_add");
        VECTOR_SUBTRACT vector_subtract = (VECTOR_SUBTRACT)GetProcAddress(hDll, "vector_subtract");


        if (vector_create && vector_delete && vector_get_x &&
            vector_get_y && vector_get_radius && vector_get_theta &&
            vector_add && vector_subtract) {

            void* vec1 = vector_create(3.0, 4.0);
            void* vec2 = vector_create(1.0, 2.0);

            std::cout << "Vector 1: (" << vector_get_x(vec1)
                << ", " << vector_get_y(vec1) << ")" << std::endl;
            std::cout << "Vector 2: (" << vector_get_x(vec2)
                << ", " << vector_get_y(vec2) << ")" << std::endl;

            std::cout << std::endl;

            void* sum = vector_add(vec1, vec2);
            void* diff = vector_subtract(vec1, vec2);

            std::cout << "Addition: (" << vector_get_x(sum)
                << ", " << vector_get_y(sum) << ")" << std::endl;
            std::cout << "Subtraction: (" << vector_get_x(diff)
                << ", " << vector_get_y(diff) << ")" << std::endl;

            std::cout << std::endl;

            std::cout << "Vector 1 - radius: " << vector_get_radius(vec1) << std::endl;
            std::cout << "Vector 1 - theta: " << vector_get_theta(vec1) << std::endl;

            std::cout << std::endl;

            void** VECTOR_ZERO = (void**)GetProcAddress(hDll, "VECTOR_ZERO");
            if (VECTOR_ZERO) {
                void* zero_vec = *VECTOR_ZERO;
                std::cout << "Zero vector: (" << vector_get_x(zero_vec)
                    << ", " << vector_get_y(zero_vec) << ")" << std::endl;
            }

            std::cout << std::endl;

            std::cout << "Vector 2 - radius: " << vector_get_radius(vec2) << std::endl;
            std::cout << "Vector 2 - theta: " << vector_get_theta(vec2) << std::endl;
            std::cout << std::endl;

            vector_delete(vec1);
            vector_delete(vec2);
            vector_delete(sum);
            vector_delete(diff);
        }
        FreeLibrary(hDll);
    }
    return 0;
}