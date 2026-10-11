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

    /**
     * @brief return the local fill boundary, or an empty contour for unsupported shapes
     *
     * @return ordered local boundary vertices, or an empty vector when no valid fill boundary exists
     */
    virtual std::vector<FillPoint> getFillContour() const;
public:
    float tx = 0, ty = 0, angle = 0, sx = 1.0f, sy = 1.0f;
    bool selected = false;

    /**
     * @brief release shape resources when destroyed through a base class pointer
     */
    virtual ~Shape() = default;
    
    /**
     * @brief draw the shape outline using its original points and the current matrix
     */
    virtual void draw() const = 0;

    /**
     * @brief enable filling and store RGB components for a valid fill contour
     *
     * @param r red color component in the range from 0 to 1
     * @param g green color component in the range from 0 to 1
     * @param b blue color component in the range from 0 to 1
     */
    void setFillColor(float r, float g, float b);

    /**
     * @brief transform and scan-line fill the contour before applying the shape matrix
     */
    void drawFill() const;

    /**
     * @brief append a point to the shape geometry
     *
     * @param p point to append in the original shape coordinates
     */
    void addPoint(Point p);

    /**
     * @brief replace the point at the given index if the index is valid
     *
     * @param idx zero-based index of the point to replace
     * @param p replacement point in the original shape coordinates
     */
    void setPoint(size_t idx, Point p);

    /**
     * @brief return a copy of the original shape points
     *
     * @return copy of the original shape points
     */
    std::vector<Point> getPoints() const;
    
    /**
     * @brief return the center of the original bounding box, or the origin for no points
     *
     * @return integer center of the original bounding box, or the origin when there are no points
     */
    Point getCenter() const;

    /**
     * @brief push the current matrix and apply translation, rotation and scaling around the center
     *
     * require the caller to restore the matrix with glPopMatrix after drawing
     */
    void applyTransform() const;

    /**
     * @brief draw the selection box, corner handles and rotation handle using the current matrix
     */
    void drawHitbox() const;

    /**
     * @brief compute padded selection bounds with translation and scale, excluding rotation
     *
     * leave output values unchanged when the shape has no points
     *
     * @param minx output minimum x coordinate of the padded selection bounds
     * @param maxx output maximum x coordinate of the padded selection bounds
     * @param miny output minimum y coordinate of the padded selection bounds
     * @param maxy output maximum y coordinate of the padded selection bounds
     */
    void getTransformedBounds(int& minx, int& maxx, int& miny, int& maxy) const;
};

// derived shapes
class LineShape : public Shape {
public:
    /**
     * @brief draw a line between two points using Bresenham's algorithm
     */
    void draw() const override;
};

class RectShape : public Shape {
protected:
    /**
     * @brief return the four rectangle corners defined by two opposite points
     *
     * @return ordered local boundary vertices, or an empty vector when no valid fill boundary exists
     */
    std::vector<FillPoint> getFillContour() const override;
public:
    /**
     * @brief draw the four rectangle sides using Bresenham's algorithm
     */
    void draw() const override;
};

class EllipseShape : public Shape {
protected:
    /**
     * @brief approximate the ellipse boundary with segments sampled according to its scaled size
     *
     * @return ordered local boundary vertices, or an empty vector when no valid fill boundary exists
     */
    std::vector<FillPoint> getFillContour() const override;
public:
    /**
     * @brief draw an ellipse inside the two-point bounding box using the midpoint algorithm
     */
    void draw() const override;
};

class PolyShape : public Shape {
protected:
    /**
     * @brief return the polygon vertices as a fill contour with floating-point coordinates
     *
     * @return ordered local boundary vertices, or an empty vector when no valid fill boundary exists
     */
    std::vector<FillPoint> getFillContour() const override;
public:
    /**
     * @brief connect consecutive vertices and close the polygon using Bresenham's algorithm
     */
    void draw() const override;
};

// drawing algorithms

/**
 * @brief compute inclusive horizontal pixel spans using ET/EAT and the even-odd rule
 *
 * @param contour ordered vertices of the closed boundary with an implicit last-to-first edge
 * @return horizontal spans with inclusive x endpoints ordered by scan-line and x coordinate
 */
std::vector<ScanlineSpan> scanlineFill(const std::vector<FillPoint>& contour);

/**
 * @brief draw one pixel at the given coordinates with the current color and matrix
 *
 * @param x x coordinate of the pixel
 * @param y y coordinate of the pixel
 */
void write_pixel(int x, int y);

/**
 * @brief rasterize a line between two endpoints using Bresenham's integer error updates
 *
 * @param x1 x coordinate of the first endpoint
 * @param y1 y coordinate of the first endpoint
 * @param x2 x coordinate of the second endpoint
 * @param y2 y coordinate of the second endpoint
 */
void inc_line(int x1, int y1, int x2, int y2);

/**
 * @brief draw a rectangle outline by connecting its four corners with rasterized lines
 *
 * @param x1 x coordinate of the first opposite corner
 * @param y1 y coordinate of the first opposite corner
 * @param x2 x coordinate of the second opposite corner
 * @param y2 y coordinate of the second opposite corner
 */
void draw_rect(int x1, int y1, int x2, int y2);

/**
 * @brief rasterize an ellipse using its center, horizontal radius and vertical radius
 *
 * @param cx x coordinate of the ellipse center
 * @param cy y coordinate of the ellipse center
 * @param a nonnegative horizontal radius in pixels
 * @param b nonnegative vertical radius in pixels
 */
void MidPointEllipse(int cx, int cy, int a, int b);

#endif
