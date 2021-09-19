//
// Created by Aviv on 29/06/2021.
//

#include "Point2D.h"
#include <cmath>
#include <iostream>
#include "Random.h"

Point2D& Point2D::operator+=(const Point2D& point) {
    Point2D res(x,y);
    x += point.x;
    y += point.y;
    return *this;
}

bool Point2D::operator==(const Point2D &rhs) const {
    return ((int)round(x)) == ((int)round(rhs.x)) &&
            ((int)round(y)) == ((int) round(rhs.y));
}

bool Point2D::operator!=(const Point2D &rhs) const {
    return !(rhs == *this);
}

Point2D Point2D::operator+(const Point2D &point) const {
    return {x + point.x, y + point.y};
}

double Point2D::Distance(const Point2D& point) const {
    return std::sqrt(std::pow(x - point.x,2) + std::pow(y - point.y,2));
}

double Point2D::ManhattanDistance(const Point2D& point)const{
    return std::abs(x - point.x) + std::abs(y - point.y);
}

bool Point2D::InLine(const Point2D &end, const Point2D& point) const {
    int dAE = (int)(Distance(end)*100);
    int dAP = (int)(Distance(point)*100);
    int dEP = (int)(end.Distance(point)*100);
    return (dAP + dEP) == dAE;
}

Point2D Point2D::operator*(double constant) const {
    return {x * constant, y * constant};
}

double Point2D::AngleBetweenPoints(const Point2D &a, const Point2D &b) {
    double res = std::atan2(b.y - a.y, b.x - a.x);
    return res;
}

Point2D Point2D::GetRandomInRadius(const Point2D &center, double radius) {
    double rad = Random::Rand(4,radius);
    double deg = Random::Rand(0,M_PI);
    Point2D res(rad*sin(deg),rad*cos(deg));
    res += center;
    return res;
}


