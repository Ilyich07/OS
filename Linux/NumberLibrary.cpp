
#include "NumberLibrary.h"


Number::Number(double val) : value(val) {}

Number::Number(int val) : value(static_cast<double>(val)) {}

Number::Number(const Number& other) : value(other.value) {}

double Number::getValue() const {
    return value;
}

Number Number::operator+(const Number& other) const {
    return Number(value + other.value);
}

Number Number::operator-(const Number& other) const {
    return Number(value - other.value);
}

Number Number::operator*(const Number& other) const {
    return Number(value * other.value);
}

Number Number::operator/(const Number& other) const {
    if (other.value == 0.0) {
        throw std::runtime_error("Division by zero!");
    }
    return Number(value / other.value);
}


Number Number::sqrt(const Number& num) {
    if (num.value < 0) {
        throw std::runtime_error("Square root of negative number!");
    }
    return Number(std::sqrt(num.value));
}

Number Number::arctg(const Number& x, const Number& y) {
    return Number(std::atan2(y.value, x.value));
}

std::ostream& operator<<(std::ostream& os, const Number& num) {
    os << num.value;
    return os;
}


const Number Number::ZERO(0.0);
const Number Number::ONE(1.0);
