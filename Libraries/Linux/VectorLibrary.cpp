#include "VectorLibrary.h"
#include <cmath>

Vector::Vector(const Number& x_val, const Number& y_val) : x(x_val), y(y_val) {}
Vector::Vector(const Vector& other) : x(other.x), y(other.y) {}

Number Vector::getX() const { return x; }
Number Vector::getY() const { return y; }

Number Vector::getRadius() const {
    return Number::sqrt(x * x + y * y);
}

Number Vector::getTheta() const {
    return Number::arctg(x, y);
}

Vector Vector::add(const Vector& other) const {
    return Vector(x + other.x, y + other.y);
}

Vector Vector::subtract(const Vector& other) const {
    return Vector(x - other.x, y - other.y);
}

Vector* VECTOR_ZERO = new Vector(Number(0.0), Number(0.0));
Vector* VECTOR_ONE_ONE = new Vector(Number(1.0), Number(1.0));


extern "C" {
    Vector* vector_create(double x, double y) {
        return new Vector(Number(x), Number(y));  
    }

    void vector_delete(Vector* vector) {
        delete vector;
    }

    double vector_get_x(Vector* vector) {
        return vector->getX().getValue();
    }

    double vector_get_y(Vector* vector) {
        return vector->getY().getValue();
    }

    double vector_get_radius(Vector* vector) {
        return vector->getRadius().getValue();
    }

    double vector_get_theta(Vector* vector) {
        return vector->getTheta().getValue();
    }

    Vector* vector_add(Vector* vector1, Vector* vector2) {
        Vector result = vector1->add(*vector2);
        return new Vector(result.getX(), result.getY());
    }

    Vector* vector_subtract(Vector* vector1, Vector* vector2) {
        Vector result = vector1->subtract(*vector2);
        return new Vector(result.getX(), result.getY());
    }
}
