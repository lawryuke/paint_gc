#include "App.h"
#include <GL/glut.h>
#include <iostream>
#include <cmath>
#include <string>

App* App::instance = nullptr;

App::App() : 
    currentTool(LINE_TOOL), dragMode(DRAG_NONE), selectedShapeIndex(-1),
    isDragging(false), dragStart({0,0}), dragCurrent({0,0}),
    panX(0), panY(0), panStart({0,0}),
    winWidth(800), winHeight(600), navbarHeight(50) 
{
    instance = this;
}

void App::run(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(winWidth, winHeight);
    glutInitWindowPosition(50, 50);
    glutCreateWindow("Paint POO - OpenGL");
    
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    
    glutDisplayFunc(displayCallback);
    glutReshapeFunc(reshapeCallback);
    glutMouseFunc(mouseCallback);
    glutMotionFunc(motionCallback);
    glutSpecialFunc(keyboardCallback);
    glutKeyboardFunc(normalKeyboardCallback);
    
    glutMainLoop();
}

void App::drawNavbar() {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, winWidth, 0, winHeight);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    glColor3f(0.2f, 0.2f, 0.2f);
    glBegin(GL_QUADS);
        glVertex2i(0, winHeight - navbarHeight);
        glVertex2i(winWidth, winHeight - navbarHeight);
        glVertex2i(winWidth, winHeight);
        glVertex2i(0, winHeight);
    glEnd();
    
    std::string tools[] = {
        "Select", "Pan", "Line", 
        "Rect", "Ellipse", "Polygon"
    };
    
    for(int i=0; i<6; ++i) {
        if((int)currentTool == i+1) glColor3f(0.0f, 1.0f, 0.0f);
        else glColor3f(1.0f, 1.0f, 1.0f);
        
        int x = 10 + i * 90;
        int y = winHeight - 30;
        glRasterPos2i(x, y);
        for (char c : tools[i]) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
    }
    
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void App::drawPreview() {
    glColor3f(0.8f, 0.8f, 0.8f);
    if (isDragging && currentTool >= LINE_TOOL && currentTool <= ELLIPSE_TOOL) {
        if (currentTool == LINE_TOOL) inc_line(dragStart.x, dragStart.y, dragCurrent.x, dragCurrent.y);
        else if (currentTool == RECT_TOOL) draw_rect(dragStart.x, dragStart.y, dragCurrent.x, dragCurrent.y);
        else if (currentTool == ELLIPSE_TOOL) {
            int cx = (dragStart.x + dragCurrent.x) / 2;
            int cy = (dragStart.y + dragCurrent.y) / 2;
            int a = std::abs(dragCurrent.x - dragStart.x) / 2;
            int b = std::abs(dragCurrent.y - dragStart.y) / 2;
            MidPointEllipse(cx, cy, a, b);
        }
    }
    
    if (currentTool == POLY_TOOL && !currentPoly.empty()) {
        glColor3f(1.0f, 0.0f, 0.0f);
        for (auto p : currentPoly) {
            glBegin(GL_QUADS);
                glVertex2i(p.x-2, p.y-2); glVertex2i(p.x+2, p.y-2);
                glVertex2i(p.x+2, p.y+2); glVertex2i(p.x-2, p.y+2);
            glEnd();
        }
        glColor3f(0.8f, 0.8f, 0.8f);
        if (currentPoly.size() > 1) {
            for (size_t i = 0; i < currentPoly.size() - 1; ++i)
                inc_line(currentPoly[i].x, currentPoly[i].y, currentPoly[i+1].x, currentPoly[i+1].y);
        }
    }
}

void App::display() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    glPushMatrix();
    glTranslatef(panX, panY, 0.0f);
    
    for (const auto& s : shapes) {
        glColor3f(1.0f, 1.0f, 1.0f);
        s->applyTransform();
        s->draw();
        if (s->selected) s->drawHitbox();
        glPopMatrix();
    }
    
    drawPreview();
    
    glPopMatrix();
    drawNavbar();
    glutSwapBuffers();
}

void App::reshape(int w, int h) {
    winWidth = w; winHeight = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void App::mouse(int button, int state, int x, int y) {
    int worldX = x - panX;
    int worldY = (winHeight - y) - panY;
    
    if (winHeight - y >= winHeight - navbarHeight) {
        if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON) {
            for(int i = 0; i < 6; ++i) {
                int btnStartX = 10 + i * 90;
                if (x >= btnStartX && x <= btnStartX + 80) {
                    currentTool = (ToolMode)(i + 1);
                    isDragging = false;
                    currentPoly.clear();
                    glutPostRedisplay();
                    break;
                }
            }
        }
        return;
    }

    if (currentTool == POLY_TOOL) {
        if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON) {
            currentPoly.push_back({worldX, worldY});
        } else if (state == GLUT_DOWN && button == GLUT_RIGHT_BUTTON) {
            if (currentPoly.size() >= 3) {
                auto s = std::make_unique<PolyShape>();
                for (auto p : currentPoly) s->addPoint(p);
                shapes.push_back(std::move(s));
            }
            currentPoly.clear();
            currentTool = SELECT_TOOL;
        }
    } 
    else if (currentTool == PAN_TOOL) {
        if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON) {
            isDragging = true;
            panStart = {x, y};
        } else if (state == GLUT_UP) {
            isDragging = false;
        }
    }
    else if (currentTool >= LINE_TOOL && currentTool <= ELLIPSE_TOOL) {
        if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON) {
            isDragging = true;
            dragStart = {worldX, worldY};
            dragCurrent = {worldX, worldY};
        } else if (state == GLUT_UP && button == GLUT_LEFT_BUTTON && isDragging) {
            isDragging = false;
            std::unique_ptr<Shape> s;
            if(currentTool == LINE_TOOL) s = std::make_unique<LineShape>();
            else if(currentTool == RECT_TOOL) s = std::make_unique<RectShape>();
            else if(currentTool == ELLIPSE_TOOL) s = std::make_unique<EllipseShape>();
            
            s->addPoint(dragStart);
            s->addPoint(dragCurrent);
            shapes.push_back(std::move(s));
        }
    }
    else if (currentTool == SELECT_TOOL) {
        if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON) {
            dragMode = DRAG_NONE;
            isDragging = false;
            
            if (selectedShapeIndex != -1) {
                int minx, maxx, miny, maxy;
                shapes[selectedShapeIndex]->getTransformedBounds(minx, maxx, miny, maxy);
                int pad = 5;
                auto in_rect = [&](int px, int py, int rx, int ry) {
                    return px >= rx-8 && px <= rx+8 && py >= ry-8 && py <= ry+8;
                };
                
                if (in_rect(worldX, worldY, minx-pad, maxy+pad)) dragMode = DRAG_SCALE_TL;
                else if (in_rect(worldX, worldY, maxx+pad, maxy+pad)) dragMode = DRAG_SCALE_TR;
                else if (in_rect(worldX, worldY, maxx+pad, miny-pad)) dragMode = DRAG_SCALE_BR;
                else if (in_rect(worldX, worldY, minx-pad, miny-pad)) dragMode = DRAG_SCALE_BL;
                else if (in_rect(worldX, worldY, (minx+maxx)/2, maxy+pad+15)) dragMode = DRAG_ROTATE;
                else if (worldX >= minx && worldX <= maxx && worldY >= miny && worldY <= maxy) {
                    dragMode = DRAG_TRANSLATE;
                }
                
                if (dragMode != DRAG_NONE) {
                    isDragging = true;
                    dragStart = {worldX, worldY};
                }
            }
            
            if (!isDragging) {
                for (auto& s : shapes) s->selected = false;
                selectedShapeIndex = -1;
                
                for (int i = shapes.size()-1; i >= 0; --i) {
                    int minx, maxx, miny, maxy;
                    shapes[i]->getTransformedBounds(minx, maxx, miny, maxy);
                    if (worldX >= minx && worldX <= maxx && worldY >= miny && worldY <= maxy) {
                        shapes[i]->selected = true;
                        selectedShapeIndex = i;
                        dragMode = DRAG_TRANSLATE;
                        isDragging = true;
                        dragStart = {worldX, worldY};
                        break;
                    }
                }
            }
        } 
        else if (state == GLUT_UP && button == GLUT_LEFT_BUTTON) {
            isDragging = false;
            dragMode = DRAG_NONE;
        }
    }
    glutPostRedisplay();
}

void App::motion(int x, int y) {
    if (isDragging) {
        if (currentTool == PAN_TOOL) {
            panX += (x - panStart.x);
            panY += -(y - panStart.y);
            panStart = {x, y};
        } 
        else if (currentTool == SELECT_TOOL && selectedShapeIndex != -1) {
            int worldX = x - (int)panX;
            int worldY = (winHeight - y) - (int)panY;
            int dx = worldX - dragStart.x;
            int dy = worldY - dragStart.y;
            
            auto& s = shapes[selectedShapeIndex];
            
            if (dragMode == DRAG_TRANSLATE) {
                s->tx += dx;
                s->ty += dy;
            } 
            else if (dragMode >= DRAG_SCALE_TL && dragMode <= DRAG_SCALE_BL) {
                float scale_step_x = (float)dx / 100.0f;
                float scale_step_y = (float)dy / 100.0f;
                
                if (dragMode == DRAG_SCALE_TR) { s->sx += scale_step_x; s->sy += scale_step_y; }
                else if (dragMode == DRAG_SCALE_TL) { s->sx -= scale_step_x; s->sy += scale_step_y; }
                else if (dragMode == DRAG_SCALE_BR) { s->sx += scale_step_x; s->sy -= scale_step_y; }
                else if (dragMode == DRAG_SCALE_BL) { s->sx -= scale_step_x; s->sy -= scale_step_y; }
                
                if(s->sx < 0.1f) s->sx = 0.1f;
                if(s->sy < 0.1f) s->sy = 0.1f;
            }
            else if (dragMode == DRAG_ROTATE) {
                Point c = s->getCenter();
                float cx = c.x + s->tx;
                float cy = c.y + s->ty;
                float vecX = worldX - cx;
                float vecY = worldY - cy;
                float angle_rad = atan2(vecY, vecX);
                s->angle = (angle_rad * 180.0f / 3.14159265f) - 90.0f;
            }
            dragStart = {worldX, worldY};
        }
        else {
            dragCurrent = {x - (int)panX, (winHeight - y) - (int)panY};
        }
        glutPostRedisplay();
    }
}

void App::keyboard(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_F1: currentTool = SELECT_TOOL; break;
        case GLUT_KEY_F2: currentTool = PAN_TOOL; break;
        case GLUT_KEY_F3: currentTool = LINE_TOOL; break;
        case GLUT_KEY_F4: currentTool = RECT_TOOL; break;
        case GLUT_KEY_F5: currentTool = ELLIPSE_TOOL; break;
        case GLUT_KEY_F6: currentTool = POLY_TOOL; break;
    }
    isDragging = false;
    currentPoly.clear();
    glutPostRedisplay();
}

void App::normalKeyboard(unsigned char key, int x, int y) {
    // GLUT sends Backspace (8) and Delete (127) as regular keyboard events.
    if ((key != 8 && key != 127) || selectedShapeIndex == -1) return;

    shapes.erase(shapes.begin() + selectedShapeIndex);
    selectedShapeIndex = -1;
    isDragging = false;
    dragMode = DRAG_NONE;
    glutPostRedisplay();
}

// Static callbacks wrapper
void App::displayCallback() { if(instance) instance->display(); }
void App::reshapeCallback(int w, int h) { if(instance) instance->reshape(w, h); }
void App::mouseCallback(int button, int state, int x, int y) { if(instance) instance->mouse(button, state, x, y); }
void App::motionCallback(int x, int y) { if(instance) instance->motion(x, y); }
void App::keyboardCallback(int key, int x, int y) { if(instance) instance->keyboard(key, x, y); }
void App::normalKeyboardCallback(unsigned char key, int x, int y) { if(instance) instance->normalKeyboard(key, x, y); }
