#pragma once

#ifdef VECTORLIBRARY_EXPORTS
#define VECTORLIBRARY_API __declspec(dllexport)
#else
#define VECTORLIBRARY_API __declspec(dllimport)
#endif

#include "NumberLibrary.h"

class VECTORLIBRARY_API Vector {
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

// C-интерфейс использует Vector* вместо void*
extern "C" {
    VECTORLIBRARY_API extern Vector* VECTOR_ZERO;
    VECTORLIBRARY_API extern Vector* VECTOR_ONE_ONE;

    VECTORLIBRARY_API Vector* vector_create(double x, double y);
    VECTORLIBRARY_API void vector_delete(Vector* vector);
    VECTORLIBRARY_API double vector_get_x(Vector* vector);
    VECTORLIBRARY_API double vector_get_y(Vector* vector);
    VECTORLIBRARY_API double vector_get_radius(Vector* vector);
    VECTORLIBRARY_API double vector_get_theta(Vector* vector);
    VECTORLIBRARY_API Vector* vector_add(Vector* vector1, Vector* vector2);
    VECTORLIBRARY_API Vector* vector_subtract(Vector* vector1, Vector* vector2);
}