#define VECTORLIBRARY_EXPORTS
#include "VectorLibrary.h"
#include <cmath>

Number create_number(double value) {
    return Number(value);
}

Vector::Vector(const Number& x_val, const Number& y_val) : x(x_val), y(y_val) {}
Vector::Vector(const Vector& other) : x(other.x), y(other.y) {}

Number Vector::getX() const { return x; }
Number Vector::getY() const { return y; }

Number Vector::getRadius() const {
    Number x_squared = x * x;
    Number y_squared = y * y;
    Number sum = x_squared + y_squared;
    return Number::sqrt(sum);
}

Number Vector::getTheta() const {
    return Number::arctg(x, y);
}

Vector Vector::add(const Vector& other) const {
    Number new_x = x + other.x;
    Number new_y = y + other.y;
    return Vector(new_x, new_y);
}

Vector Vector::subtract(const Vector& other) const {
    Number new_x = x - other.x;
    Number new_y = y - other.y;
    return Vector(new_x, new_y);
}

static Vector* zero_vector = nullptr;
static Vector* one_one_vector = nullptr;


Vector* VECTOR_ZERO = new Vector(create_number(0.0), create_number(0.0));
Vector* VECTOR_ONE_ONE = new Vector(create_number(1.0), create_number(1.0));


Vector* vector_create(double x, double y) {
    Number num_x = create_number(x);
    Number num_y = create_number(y);
    return new Vector(num_x, num_y);
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


