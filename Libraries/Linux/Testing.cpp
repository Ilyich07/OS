#include <iostream>
#include "VectorLibrary.h";
#include "NumberLibrary.h";


int main() {
    
    Vector* vec1 = vector_create(5.0, 4.0);
    Vector* vec2 = vector_create(1.0, 2.0);

    std::cout << "Vector 1: (" << vector_get_x(vec1) << ", " << vector_get_y(vec1) << ")" << std::endl;
    std::cout << "Vector 2: (" << vector_get_x(vec2) << ", " << vector_get_y(vec2) << ")" << std::endl;

    std::cout << '\n';

    Vector* sum = vector_add(vec1, vec2);
    Vector* diff = vector_subtract(vec1, vec2);

    std::cout << "Vector 1 + Vector 2 = (" << vector_get_x(sum) << ", " << vector_get_y(sum) << ")" << std::endl;
    std::cout << "Vector 1 - Vector 2 = (" << vector_get_x(diff) << ", " << vector_get_y(diff) << ")" << std::endl;

    std::cout << '\n';

    std::cout << "Vector 1 - Radius: " << vector_get_radius(vec1) << std::endl;
    std::cout << "Vector 1 - Theta: " << vector_get_theta(vec1) << std::endl;

    Vector* zero_vec = VECTOR_ZERO;

    std::cout << "ZERO: (" << vector_get_x(zero_vec) << ", " << vector_get_y(zero_vec) << ")" << std::endl;

    vector_delete(vec1);
    vector_delete(vec2);
    vector_delete(sum);
    vector_delete(diff);
    vector_delete(zero_vec);

}
