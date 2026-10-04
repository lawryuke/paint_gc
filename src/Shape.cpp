#include "Shape.h"
#include <cmath>
#include <cstdlib>

void write_pixel(int x, int y) {
    glBegin(GL_POINTS);
        glVertex2i(x, y);
    glEnd();
}

void inc_line(int x1, int y1, int x2, int y2) {
    int dx = std::abs(x2 - x1), sx = x1 < x2 ? 1 : -1;
    int dy = -std::abs(y2 - y1), sy = y1 < y2 ? 1 : -1;
    int err = dx + dy, e2; 
    while (true) {
        write_pixel(x1, y1);
        if (x1 == x2 && y1 == y2) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x1 += sx; }
        if (e2 <= dx) { err += dx; y1 += sy; }
    }
}

void draw_rect(int x1, int y1, int x2, int y2) {
    inc_line(x1, y1, x2, y1);
    inc_line(x2, y1, x2, y2);
    inc_line(x2, y2, x1, y2);
    inc_line(x1, y2, x1, y1);
}

void EllipsePoints(int cx, int cy, int x, int y) {
    write_pixel(cx + x, cy + y);
    write_pixel(cx - x, cy + y);
    write_pixel(cx + x, cy - y);
    write_pixel(cx - x, cy - y);
}

void MidPointEllipse(int cx, int cy, int a, int b) {
    if (a == 0 || b == 0) return;
    double d1, d2;
    int x = 0; int y = b;
    double a2 = a * a, b2 = b * b;
    double dx = 2 * b2 * x, dy = 2 * a2 * y;
    
    d1 = b2 - (a2 * b) + (0.25 * a2);
    EllipsePoints(cx, cy, x, y);
    while (dx < dy) {
        x++; dx += 2 * b2;
        if (d1 < 0) d1 += dx + b2;
        else { y--; dy -= 2 * a2; d1 += dx - dy + b2; }
        EllipsePoints(cx, cy, x, y);
    }
    d2 = b2 * ((x + 0.5) * (x + 0.5)) + a2 * ((y - 1) * (y - 1)) - (a2 * b2);
    while (y >= 0) {
        y--; dy -= 2 * a2;
        if (d2 > 0) d2 += a2 - dy;
        else { x++; dx += 2 * b2; d2 += a2 - dy + dx; }
        EllipsePoints(cx, cy, x, y);
    }
}

// --- Shape implementation ---
void Shape::addPoint(Point p) { points.push_back(p); }
void Shape::setPoint(size_t idx, Point p) { if (idx < points.size()) points[idx] = p; }
std::vector<Point> Shape::getPoints() const { return points; }

Point Shape::getCenter() const {
    if (points.empty()) return {0,0};
    int minx = points[0].x, maxx = points[0].x;
    int miny = points[0].y, maxy = points[0].y;
    for (const auto& p : points) {
        if (p.x < minx) minx = p.x; if (p.x > maxx) maxx = p.x;
        if (p.y < miny) miny = p.y; if (p.y > maxy) maxy = p.y;
    }
    return {(minx + maxx)/2, (miny + maxy)/2};
}

void Shape::applyTransform() const {
    glPushMatrix();
    Point c = getCenter();
    glTranslatef(c.x + tx, c.y + ty, 0.0f);
    glRotatef(angle, 0.0f, 0.0f, 1.0f);
    glScalef(sx, sy, 1.0f);
    glTranslatef(-c.x, -c.y, 0.0f);
}

void Shape::drawHitbox() const {
    if (points.empty()) return;
    int minx = points[0].x, maxx = points[0].x;
    int miny = points[0].y, maxy = points[0].y;
    for (const auto& p : points) {
        if (p.x < minx) minx = p.x; if (p.x > maxx) maxx = p.x;
        if (p.y < miny) miny = p.y; if (p.y > maxy) maxy = p.y;
    }
    
    glColor3f(1.0f, 1.0f, 0.0f); // Amarillo
    glLineStipple(1, 0xAAAA);
    glEnable(GL_LINE_STIPPLE);
    
    glBegin(GL_LINE_LOOP);
        glVertex2i(minx - 5, miny - 5);
        glVertex2i(maxx + 5, miny - 5);
        glVertex2i(maxx + 5, maxy + 5);
        glVertex2i(minx - 5, maxy + 5);
    glEnd();
    
    glDisable(GL_LINE_STIPPLE);
    
    glColor3f(0.0f, 1.0f, 1.0f);
    int pad = 5;
    auto draw_anchor = [&](int x, int y) {
        glBegin(GL_QUADS);
            glVertex2i(x-4, y-4); glVertex2i(x+4, y-4);
            glVertex2i(x+4, y+4); glVertex2i(x-4, y+4);
        glEnd();
    };
    draw_anchor(minx - pad, miny - pad);
    draw_anchor(maxx + pad, miny - pad);
    draw_anchor(maxx + pad, maxy + pad);
    draw_anchor(minx - pad, maxy + pad);
    
    glColor3f(1.0f, 0.0f, 1.0f);
    int midx = (minx + maxx) / 2;
    glBegin(GL_LINES);
        glVertex2i(midx, maxy + pad);
        glVertex2i(midx, maxy + pad + 15);
    glEnd();
    draw_anchor(midx, maxy + pad + 15);
}

void Shape::getTransformedBounds(int& minx, int& maxx, int& miny, int& maxy) const {
    if (points.empty()) return;
    minx = points[0].x; maxx = minx;
    miny = points[0].y; maxy = miny;
    for (auto p : points) {
        if (p.x < minx) minx = p.x; if (p.x > maxx) maxx = p.x;
        if (p.y < miny) miny = p.y; if (p.y > maxy) maxy = p.y;
    }
    Point c = getCenter();
    float w = (maxx - minx + 10) * sx; 
    float h = (maxy - miny + 10) * sy;
    minx = c.x + tx - w/2;
    maxx = c.x + tx + w/2;
    miny = c.y + ty - h/2;
    maxy = c.y + ty + h/2;
}

// --- Derived Shapes ---
void LineShape::draw() const {
    if(points.size() == 2)
        inc_line(points[0].x, points[0].y, points[1].x, points[1].y);
}

void RectShape::draw() const {
    if(points.size() == 2)
        draw_rect(points[0].x, points[0].y, points[1].x, points[1].y);
}

void EllipseShape::draw() const {
    if(points.size() == 2) {
        int cx = (points[0].x + points[1].x) / 2;
        int cy = (points[0].y + points[1].y) / 2;
        int a = std::abs(points[1].x - points[0].x) / 2;
        int b = std::abs(points[1].y - points[0].y) / 2;
        MidPointEllipse(cx, cy, a, b);
    }
}

void PolyShape::draw() const {
    if(points.size() > 1) {
        for (size_t i = 0; i < points.size() - 1; ++i)
            inc_line(points[i].x, points[i].y, points[i+1].x, points[i+1].y);
        inc_line(points.back().x, points.back().y, points[0].x, points[0].y);
    }
}
