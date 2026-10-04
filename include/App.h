#ifndef APP_H
#define APP_H

#include "Shape.h"
#include <vector>
#include <memory>

enum ToolMode { SELECT_TOOL=1, PAN_TOOL=2, LINE_TOOL=3, RECT_TOOL=4, ELLIPSE_TOOL=5, POLY_TOOL=6 };
enum DragMode { DRAG_NONE, DRAG_TRANSLATE, DRAG_SCALE_TL, DRAG_SCALE_TR, DRAG_SCALE_BR, DRAG_SCALE_BL, DRAG_ROTATE };

class App {
private:
    std::vector<std::unique_ptr<Shape>> shapes;
    std::vector<Point> currentPoly;
    
    ToolMode currentTool;
    DragMode dragMode;
    int selectedShapeIndex;
    
    bool isDragging;
    Point dragStart, dragCurrent;
    
    float panX, panY;
    Point panStart;
    
    int winWidth, winHeight;
    int navbarHeight;
    
    static App* instance; // Singleton for GLUT callbacks

public:
    App();
    
    void run(int argc, char** argv);
    
    // Callbacks
    void display();
    void reshape(int w, int h);
    void mouse(int button, int state, int x, int y);
    void motion(int x, int y);
    void keyboard(int key, int x, int y);
    
    // Drawing helpers
    void drawNavbar();
    void drawPreview();
    
    static void displayCallback();
    static void reshapeCallback(int w, int h);
    static void mouseCallback(int button, int state, int x, int y);
    static void motionCallback(int x, int y);
    static void keyboardCallback(int key, int x, int y);
};

#endif
