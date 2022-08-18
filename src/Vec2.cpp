#include "Vec2.h"
#include <math.h>
#include <sstream>

Vec2::Vec2()
{

}

Vec2::Vec2(float xin, float yin)
    : x(xin)
    , y(yin)
{

}

Vec2 Vec2::operator + (const Vec2 & rhs) const
{
    return Vec2(x + rhs.x, y + rhs.y);
}

Vec2 Vec2::operator - (const Vec2 & rhs) const
{
    return Vec2(x - rhs.x, y - rhs.y);
}

Vec2 Vec2::operator / (const float & val) const
{
    return Vec2(x / val, y / val);
}

Vec2 Vec2::operator * (const float & val) const
{
    return Vec2(x * val, y * val);
}

bool Vec2::operator == (const Vec2 & rhs) const
{
    return x == rhs.x && y == rhs.y;
}
                          
bool Vec2::operator != (const Vec2 & rhs) const
{
    return !(*this == rhs);
}
                          
void Vec2::operator += (const Vec2 & rhs)
{
    x += rhs.x;
    y += rhs.y;
}

void Vec2::operator -= (const Vec2 & rhs)
{
    x -= rhs.x;
    y -= rhs.y;
}

void Vec2::operator *= (const float & val)
{
    x *= val;
    y *= val;
}

void Vec2::operator /= (const float & val)
{
    x /= val;
    y /= val;
}
                          
float Vec2::dist(const Vec2 & rhs) const
{
    return sqrtf((x - rhs.x)*(x - rhs.x) + (y - rhs.y)*(y - rhs.y));
}

Vec2 Vec2::abs() const
{
    return Vec2(x < 0 ? -x : x, y < 0 ? -y : y);
}

float Vec2::operator * (const Vec2& rhs) const
{
    return x * rhs.y - rhs.x * y;
}

std::string Vec2::print() const
{
    std::stringstream ss;
    ss << x << " " << y;
    return ss.str();
}

float Vec2::cross(const Vec2& rhs) const
{
    return x * rhs.y - y * rhs.x;
}

Vec2 Vec2::round() const
{
    return Vec2
    (
        roundf(2 * x) / 2,
        roundf(2 * y) / 2
    );
}


// Copyright (C) David Churchill - All Rights Reserved
// COMP4300 - 2021FALL - Assignment 4
// Written by David Churchill (dave.churchill@gmail.com)
// Unauthorized copying of these files are strictly prohibited
// Distributed only for course work at Memorial University
// If you see this file online please contact email above
