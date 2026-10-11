#ifndef APP_H
#define APP_H

#include "Shape.h"
#include <vector>
#include <memory>

enum ToolMode { 
    SELECT_TOOL = 1,
    PAN_TOOL = 2,
    LINE_TOOL = 3,
    RECT_TOOL = 4,
    ELLIPSE_TOOL = 5,
    POLY_TOOL = 6
};

enum DragMode { 
    DRAG_NONE, 
    DRAG_TRANSLATE, 
    DRAG_SCALE_TL, 
    DRAG_SCALE_TR, 
    DRAG_SCALE_BR, 
    DRAG_SCALE_BL, 
    DRAG_ROTATE 
};

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
    
    static App* instance; // active application instance for GLUT callbacks

public:
    /**
     * @brief initialize tools, selection, dragging, canvas position and window dimensions
     */
    App();
    
    /**
     * @brief create the GLUT window, register callbacks and start the event loop
     *
     * @param argc number of command-line arguments passed to GLUT
     * @param argv command-line argument strings passed to GLUT
     */
    void run(int argc, char** argv);
    
    /**
     * @brief render the canvas, shape fills, outlines, selection controls and toolbar
     */
    void display();

    /**
     * @brief update window dimensions, viewport and orthographic projection
     *
     * @param w new window width in pixels
     * @param h new window height in pixels
     */
    void reshape(int w, int h);

    /**
     * @brief handle mouse buttons to switch tools, create shapes and start or stop dragging
     *
     * @param button identifier used by GLUT of the mouse button that changed state
     * @param state button state reported as GLUT_DOWN or GLUT_UP
     * @param x mouse x coordinate measured from the left edge of the window
     * @param y mouse y coordinate measured from the top edge of the window
     */
    void mouse(int button, int state, int x, int y);

    /**
     * @brief update canvas panning, drawing previews or the selected shape while dragging
     *
     * @param x mouse x coordinate measured from the left edge of the window
     * @param y mouse y coordinate measured from the top edge of the window
     */
    void motion(int x, int y);

    /**
     * @brief switch tools with keys 1 through 6, fill with R, G or B and delete with Delete or Backspace
     *
     * @param key character code identifying the pressed regular key
     * @param x mouse x coordinate measured from the left edge of the window, unused by this handler
     * @param y mouse y coordinate measured from the top edge of the window, unused by this handler
     */
    void keyboard(unsigned char key, int x, int y);
    
    /**
     * @brief draw the toolbar in window coordinates and highlight the current tool
     */
    void drawNavbar();

    /**
     * @brief draw the unfinished shape or polygon vertices in canvas coordinates
     */
    void drawPreview();
    
    /**
     * @brief forward the GLUT display event to the active application instance
     */
    static void displayCallback();

    /**
     * @brief forward the GLUT window resize event to the active application instance
     *
     * @param w new window width in pixels
     * @param h new window height in pixels
     */
    static void reshapeCallback(int w, int h);

    /**
     * @brief forward the GLUT mouse button event to the active application instance
     *
     * @param button identifier used by GLUT of the mouse button that changed state
     * @param state button state reported as GLUT_DOWN or GLUT_UP
     * @param x mouse x coordinate measured from the left edge of the window
     * @param y mouse y coordinate measured from the top edge of the window
     */
    static void mouseCallback(int button, int state, int x, int y);

    /**
     * @brief forward the GLUT mouse drag event to the active application instance
     *
     * @param x mouse x coordinate measured from the left edge of the window
     * @param y mouse y coordinate measured from the top edge of the window
     */
    static void motionCallback(int x, int y);

    /**
     * @brief forward the GLUT regular key event to the active application instance
     *
     * @param key character code identifying the pressed regular key
     * @param x mouse x coordinate measured from the left edge of the window
     * @param y mouse y coordinate measured from the top edge of the window
     */
    static void keyboardCallback(unsigned char key, int x, int y);
};

#endif
