#ifndef SHAPE_H
#define SHAPE_H

#include <vector>
#include <GL/glut.h>

struct Point { int x, y; };
struct FillPoint { double x, y; };
struct ScanlineSpan { int y, x1, x2; };

class Shape {
protected:
    std::vector<Point> points;
    bool filled = false;
    float fillR = 1.0f, fillG = 0.0f, fillB = 0.0f;
    virtual std::vector<FillPoint> getFillContour() const;
public:
    float tx = 0, ty = 0, angle = 0, sx = 1.0f, sy = 1.0f;
    bool selected = false;

    virtual ~Shape() = default;
    
    // Virtual method to draw the specific shape
    virtual void draw() const = 0;
    void setFillColor(float r, float g, float b);
    void drawFill() const;

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
protected:
    std::vector<FillPoint> getFillContour() const override;
public:
    void draw() const override;
};

class EllipseShape : public Shape {
protected:
    std::vector<FillPoint> getFillContour() const override;
public:
    void draw() const override;
};

class PolyShape : public Shape {
protected:
    std::vector<FillPoint> getFillContour() const override;
public:
    void draw() const override;
};

// Algoritmos base
std::vector<ScanlineSpan> scanlineFill(const std::vector<FillPoint>& contour);
void write_pixel(int x, int y);
void inc_line(int x1, int y1, int x2, int y2);
void draw_rect(int x1, int y1, int x2, int y2);
void MidPointEllipse(int cx, int cy, int a, int b);

#endif
