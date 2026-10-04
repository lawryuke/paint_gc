#ifndef SHAPE_H
#define SHAPE_H

#include <vector>
#include <GL/glut.h>

struct Point { int x, y; };

class Shape {
protected:
    std::vector<Point> points;
public:
    float tx = 0, ty = 0, angle = 0, sx = 1.0f, sy = 1.0f;
    bool selected = false;

    virtual ~Shape() = default;
    
    // Virtual method to draw the specific shape
    virtual void draw() const = 0;

    // Common methods
    void addPoint(Point p);
    void setPoint(size_t idx, Point p);
    std::vector<Point> getPoints() const;
    
    Point getCenter() const;
    void applyTransform() const;
    void drawHitbox() const;
    void getTransformedBounds(int& minx, int& maxx, int& miny, int& maxy) const;
};

// Derived shapes
class LineShape : public Shape {
public:
    void draw() const override;
};

class RectShape : public Shape {
public:
    void draw() const override;
};

class EllipseShape : public Shape {
public:
    void draw() const override;
};

class PolyShape : public Shape {
public:
    void draw() const override;
};

// Algoritmos base
void write_pixel(int x, int y);
void inc_line(int x1, int y1, int x2, int y2);
void draw_rect(int x1, int y1, int x2, int y2);
void MidPointEllipse(int cx, int cy, int a, int b);

#endif
