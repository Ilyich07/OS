#pragma once


#include "NumberLibrary.h"

class Vector {
private:
    Number x;
    Number y;
public:
    Vector(const Number& x_val, const Number& y_val);
    Vector(const Vector& other);

    Number getX() const;
    Number getY() const;
    Number getRadius() const;
    Number getTheta() const;

    Vector add(const Vector& other) const;
    Vector subtract(const Vector& other) const;
};


extern "C" {
    extern Vector* VECTOR_ZERO;
    extern Vector* VECTOR_ONE_ONE;

    Vector* vector_create(double x, double y);
    void vector_delete(Vector* vector);
    double vector_get_x(Vector* vector);
    double vector_get_y(Vector* vector);
    double vector_get_radius(Vector* vector);
    double vector_get_theta(Vector* vector);
    Vector* vector_add(Vector* vector1, Vector* vector2);
    Vector* vector_subtract(Vector* vector1, Vector* vector2);
}
