#pragma once
#include <iostream>
#include <cmath>
#include <stdexcept>

class Number {
private:
    double value;

public:
  
    Number(double val = 0.0);
    Number(int val);
    Number(const Number& other);

    ~Number() = default;

    double getValue() const;

    Number operator+(const Number& other) const;
    Number operator-(const Number& other) const;
    Number operator*(const Number& other) const;
    Number operator/(const Number& other) const;

    static Number sqrt(const Number& num);
    static Number arctg(const Number& x, const Number& y);

    friend std::ostream& operator<<(std::ostream& os, const Number& num);

    static const Number ZERO;
    static const Number ONE;
};

