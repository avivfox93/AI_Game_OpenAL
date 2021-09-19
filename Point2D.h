//
// Created by Aviv on 29/06/2021.
//

#ifndef TEST_POINT2D_H
#define TEST_POINT2D_H


class Point2D {
public:
    Point2D(double x, double y) : x(x), y(y){}


    double Distance(const Point2D& point)const;
    double ManhattanDistance(const Point2D& point)const;
    bool InLine(const Point2D& end, const Point2D& point)const;

    Point2D& operator+=(const Point2D& point);
    bool operator==(const Point2D &rhs) const;
    bool operator!=(const Point2D &rhs) const;
    Point2D operator+(const Point2D& point)const;
    Point2D operator*(double constant)const;
    static double AngleBetweenPoints(const Point2D& a, const Point2D& b);

    static Point2D GetRandomInRadius(const Point2D& center, double radius);

    double x,y;
};


#endif //TEST_POINT2D_H
