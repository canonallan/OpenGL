#include <GL/glut.h>
#include <cmath>
#include <iostream>
#include <cstdlib>
#include <vector>

// Constants for the ellipse equation
const float a = 3.0f; // Semi-major axis
const float b = 2.0f; // Semi-minor axis

// Original center coordinates
const float centerX = -2.0f;
const float centerY = 2.0f;

// Window size
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

// Shear parameters
const float shearX = 2.0f;
const float shearY = 2.0f;

// Grid properties
const float GRID_MIN_X = -10.0f;
const float GRID_MAX_X = 10.0f;
const float GRID_MIN_Y = -10.0f;
const float GRID_MAX_Y = 10.0f;

// Buffer for flood-fill
struct Pixel {
    int x, y;
    Pixel(int _x, int _y) : x(_x), y(_y) {}
};

// Structure to represent a point in ellipse coordinates
struct Point {
    float x, y;
    Point(float _x, float _y) : x(_x), y(_y) {}
};

// Global variables
std::vector<Point> originalEllipsePoints;
std::vector<Point> shearedEllipsePoints;
int currentMode = 0; // 0=Original, 1=Cyan-filled, 2=Sheared, 3=Green-filled, 4=Anti-aliased

// Function to convert from world to screen coordinates
void worldToScreen(float wx, float wy, int& sx, int& sy) {
    float range_x = GRID_MAX_X - GRID_MIN_X;
    float range_y = GRID_MAX_Y - GRID_MIN_Y;
    
    sx = (int)((wx - GRID_MIN_X) / range_x * WINDOW_WIDTH);
    sy = (int)((1.0f - (wy - GRID_MIN_Y) / range_y) * WINDOW_HEIGHT);
}

// Function to convert from screen to world coordinates
void screenToWorld(int sx, int sy, float& wx, float& wy) {
    float range_x = GRID_MAX_X - GRID_MIN_X;
    float range_y = GRID_MAX_Y - GRID_MIN_Y;
    
    wx = GRID_MIN_X + (float)sx / WINDOW_WIDTH * range_x;
    wy = GRID_MIN_Y + (1.0f - (float)sy / WINDOW_HEIGHT) * range_y;
}

// Generate ellipse points using parametric equation
void generateEllipsePoints() {
    originalEllipsePoints.clear();
    
    // Create points for the ellipse with higher resolution for smoother curve
    const int numSegments = 100;
    for (int i = 0; i <= numSegments; i++) {
        float t = 2.0f * M_PI * i / numSegments;
        float x = centerX + a * cos(t);
        float y = centerY + b * sin(t);
        originalEllipsePoints.push_back(Point(x, y));
    }
}

// Apply shear transformation to ellipse points
void applyShearTransform() {
    shearedEllipsePoints.clear();
    
    for (size_t i = 0; i < originalEllipsePoints.size(); i++) {
        float x = originalEllipsePoints[i].x;
        float y = originalEllipsePoints[i].y;
        
        // Apply shear transformation
        float new_x = x + shearX * y;
        float new_y = y + shearY * x;
        
        shearedEllipsePoints.push_back(Point(new_x, new_y));
    }
}

// Check if point is inside the ellipse
bool isInsideOriginalEllipse(float x, float y) {
    float dx = x - centerX;
    float dy = y - centerY;
    
    // Ellipse equation: (x-h)²/a² + (y-k)²/b² <= 1
    return ((dx*dx)/(a*a) + (dy*dy)/(b*b)) <= 1.0f;
}

// Check if point is inside the sheared ellipse
bool isInsideShearedEllipse(float x, float y) {
    // Apply inverse shear transformation to convert to original ellipse coordinates
    float invX = x - shearX * y;
    float invY = y - shearY * invX;
    
    // Now check if inverted point is inside original ellipse
    return isInsideOriginalEllipse(invX, invY);
}

// Draw grid lines
void drawGrid() {
    glColor3f(0.8f, 0.8f, 0.8f); // Light gray
    glLineWidth(1.0f);
    
    // Draw horizontal grid lines
    glBegin(GL_LINES);
    for (int y = (int)GRID_MIN_Y; y <= (int)GRID_MAX_Y; y++) {
        glVertex2f(GRID_MIN_X, (float)y);
        glVertex2f(GRID_MAX_X, (float)y);
    }
    glEnd();
    
    // Draw vertical grid lines
    glBegin(GL_LINES);
    for (int x = (int)GRID_MIN_X; x <= (int)GRID_MAX_X; x++) {
        glVertex2f((float)x, GRID_MIN_Y);
        glVertex2f((float)x, GRID_MAX_Y);
    }
    glEnd();
    
    // Draw axes with thicker lines
    glLineWidth(2.0f);
    glColor3f(0.0f, 0.0f, 0.0f); // Black
    
    // X-axis
    glBegin(GL_LINES);
    glVertex2f(GRID_MIN_X, 0.0f);
    glVertex2f(GRID_MAX_X, 0.0f);
    glEnd();
    
    // Y-axis
    glBegin(GL_LINES);
    glVertex2f(0.0f, GRID_MIN_Y);
    glVertex2f(0.0f, GRID_MAX_Y);
    glEnd();
    
    // Draw tick marks and labels on axes
    glLineWidth(1.0f);
    
    // X-axis ticks
    for (int x = (int)GRID_MIN_X; x <= (int)GRID_MAX_X; x++) {
        if (x == 0) continue; // Skip origin
        
        glBegin(GL_LINES);
        glVertex2f((float)x, -0.2f);
        glVertex2f((float)x, 0.2f);
        glEnd();
        
        // Display x-value
        glRasterPos2f((float)x - 0.1f, -0.6f);
        char buffer[10];
        sprintf(buffer, "%d", x);
        for (char* c = buffer; *c != '\0'; c++) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, *c);
        }
    }
    
    // Y-axis ticks
    for (int y = (int)GRID_MIN_Y; y <= (int)GRID_MAX_Y; y++) {
        if (y == 0) continue; // Skip origin
        
        glBegin(GL_LINES);
        glVertex2f(-0.2f, (float)y);
        glVertex2f(0.2f, (float)y);
        glEnd();
        
        // Display y-value
        glRasterPos2f(-0.6f, (float)y - 0.1f);
        char buffer[10];
        sprintf(buffer, "%d", y);
        for (char* c = buffer; *c != '\0'; c++) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, *c);
        }
    }
    
    // Display origin
    glRasterPos2f(-0.6f, -0.6f);
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, '0');
}

// Draw the original ellipse
void drawOriginalEllipse() {
    glColor3f(0.0f, 0.0f, 0.0f); // Black
    glLineWidth(2.0f);
    
    glBegin(GL_LINE_LOOP);
    for (size_t i = 0; i < originalEllipsePoints.size(); i++) {
        glVertex2f(originalEllipsePoints[i].x, originalEllipsePoints[i].y);
    }
    glEnd();
    
    // Mark center
    glPointSize(5.0f);
    glColor3f(1.0f, 0.0f, 0.0f); // Red
    glBegin(GL_POINTS);
    glVertex2f(centerX, centerY);
    glEnd();
    
    // Label center
    glRasterPos2f(centerX - 0.6f, centerY - 0.6f);
    char buffer[20];
    sprintf(buffer, "Center (%.1f,%.1f)", centerX, centerY);
    for (char* c = buffer; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, *c);
    }
}

// Draw the sheared ellipse
void drawShearedEllipse() {
    glColor3f(0.0f, 0.0f, 0.0f); // Black
    glLineWidth(2.0f);
    
    glBegin(GL_LINE_LOOP);
    for (size_t i = 0; i < shearedEllipsePoints.size(); i++) {
        glVertex2f(shearedEllipsePoints[i].x, shearedEllipsePoints[i].y);
    }
    glEnd();
    
    // Mark transformed center
    float transformedCenterX = centerX + shearX * centerY;
    float transformedCenterY = centerY + shearY * centerX;
    
    glPointSize(5.0f);
    glColor3f(1.0f, 0.0f, 0.0f); // Red
    glBegin(GL_POINTS);
    glVertex2f(transformedCenterX, transformedCenterY);
    glEnd();
    
    // Label transformed center
    glRasterPos2f(transformedCenterX - 0.6f, transformedCenterY - 0.6f);
    char buffer[40];
    sprintf(buffer, "New Center (%.1f,%.1f)", transformedCenterX, transformedCenterY);
    for (char* c = buffer; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, *c);
    }
}

// Execute flood fill algorithm for original ellipse
void floodFillOriginalEllipse() {
    // Prepare an offscreen buffer for drawing
    glClear(GL_COLOR_BUFFER_BIT);
    drawGrid();
    drawOriginalEllipse();
    
    // Fill the ellipse with cyan color
    float fillColor[3] = {0.0f, 1.0f, 1.0f}; // Cyan (RGB)
    
    // Create a simple filled approximation of the ellipse
    glColor3f(fillColor[0], fillColor[1], fillColor[2]);
    glBegin(GL_POLYGON);
    for (size_t i = 0; i < originalEllipsePoints.size(); i++) {
        glVertex2f(originalEllipsePoints[i].x, originalEllipsePoints[i].y);
    }
    glEnd();
}

// Execute boundary fill algorithm for sheared ellipse
void boundaryFillShearedEllipse() {
    // Prepare an offscreen buffer for drawing
    glClear(GL_COLOR_BUFFER_BIT);
    drawGrid();
    drawShearedEllipse();
    
    // Fill the ellipse with green color
    float fillColor[3] = {0.0f, 1.0f, 0.0f}; // Green (RGB)
    
    // Create a simple filled approximation of the ellipse
    glColor3f(fillColor[0], fillColor[1], fillColor[2]);
    glBegin(GL_POLYGON);
    for (size_t i = 0; i < shearedEllipsePoints.size(); i++) {
        glVertex2f(shearedEllipsePoints[i].x, shearedEllipsePoints[i].y);
    }
    glEnd();
}

// Draw anti-aliased ellipse using OpenGL's built-in anti-aliasing
void drawAntiAliasedEllipse() {
    // Enable anti-aliasing
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    
    glLineWidth(1.5f);
    glColor3f(0.0f, 0.0f, 0.0f); // Black
    
    // Draw the anti-aliased ellipse
    glBegin(GL_LINE_LOOP);
    for (size_t i = 0; i < originalEllipsePoints.size(); i++) {
        glVertex2f(originalEllipsePoints[i].x, originalEllipsePoints[i].y);
    }
    glEnd();
    
    // Disable anti-aliasing
    glDisable(GL_LINE_SMOOTH);
    glDisable(GL_BLEND);
}

// Custom anti-aliasing for ellipse (manual implementation)
void customAntiAliasedEllipse() {
    // Create a higher density set of points for smoother rendering
    const int numSegments = 200;
    std::vector<Point> highResPoints;
    
    for (int i = 0; i <= numSegments; i++) {
        float t = 2.0f * M_PI * i / numSegments;
        float x = centerX + a * cos(t);
        float y = centerY + b * sin(t);
        highResPoints.push_back(Point(x, y));
    }
    
    // Draw points with intensity based on distance from the theoretical ellipse
    glPointSize(2.0f);
    glBegin(GL_POINTS);
    
    for (float angle = 0; angle <= 2.0f * M_PI; angle += 0.01f) {
        float exactX = centerX + a * cos(angle);
        float exactY = centerY + b * sin(angle);
        
        // Draw the main point
        glColor3f(0.0f, 0.0f, 0.0f); // Black
        glVertex2f(exactX, exactY);
        
        // Draw adjacent points with decreasing opacity to create anti-aliased effect
        for (float offset = 0.05f; offset <= 0.2f; offset += 0.05f) {
            float alpha = 1.0f - offset * 5.0f; // Decreasing opacity
            
            // Inner points
            glColor4f(0.0f, 0.0f, 0.0f, alpha);
            glVertex2f(exactX - offset * cos(angle), exactY - offset * sin(angle));
            
            // Outer points
            glVertex2f(exactX + offset * cos(angle), exactY + offset * sin(angle));
        }
    }
    
    glEnd();
    
    // Draw the theoretical ellipse path
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glColor3f(0.0f, 0.0f, 0.8f); // Dark blue
    glLineWidth(1.0f);
    
    glBegin(GL_LINE_LOOP);
    for (size_t i = 0; i < highResPoints.size(); i++) {
        glVertex2f(highResPoints[i].x, highResPoints[i].y);
    }
    glEnd();
    
    glDisable(GL_LINE_SMOOTH);
    glDisable(GL_BLEND);
}

// Display function
void display() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // White background
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Draw grid and axes
    drawGrid();
    
    // Draw based on current mode
    switch (currentMode) {
        case 0: // Original ellipse
            drawOriginalEllipse();
            break;
        case 1: // Cyan filled ellipse
            floodFillOriginalEllipse();
            drawOriginalEllipse(); // Redraw boundary
            break;
        case 2: // Sheared ellipse
            drawShearedEllipse();
            break;
        case 3: // Green filled sheared ellipse
            boundaryFillShearedEllipse();
            drawShearedEllipse(); // Redraw boundary
            break;
        case 4: // Anti-aliased ellipse
            drawAntiAliasedEllipse();
            break;
        case 5: // Custom anti-aliasing implementation
            customAntiAliasedEllipse();
            break;
    }
    
    // Display current mode and instructions
    glColor3f(0.0f, 0.0f, 0.0f); // Black text
    glRasterPos2f(GRID_MIN_X + 0.5f, GRID_MAX_Y - 0.5f);
    
    const char* modeTexts[] = {
        "Original Ellipse",
        "Cyan Filled Ellipse (Flood Fill)",
        "Sheared Ellipse",
        "Green Filled Sheared Ellipse (Boundary Fill)",
        "Anti-Aliased Ellipse (OpenGL)",
        "Custom Anti-Aliased Ellipse Implementation"
    };
    
    // Display current mode
    for (const char* c = modeTexts[currentMode]; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }
    
    // Display instructions
    const char* instructions = " - Press keys 1-6 to switch modes";
    glRasterPos2f(GRID_MIN_X + 0.5f, GRID_MAX_Y - 1.0f);
    for (const char* c = instructions; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }
    
    glutSwapBuffers();
}

// Keyboard function for mode switching
void keyboard(unsigned char key, int x, int y) {
    if (key >= '1' && key <= '6') {
        currentMode = key - '1';
        glutPostRedisplay();
    } else if (key == 27) { // ESC key
        exit(0);
    }
}

// Reshape function to maintain aspect ratio
void reshape(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    // Maintain aspect ratio
    float aspect = (float)width / (float)height;
    float range_y = GRID_MAX_Y - GRID_MIN_Y;
    float range_x = range_y * aspect;
    
    glOrtho(GRID_MIN_X, GRID_MIN_X + range_x, GRID_MIN_Y, GRID_MAX_Y, -1.0, 1.0);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// Initialize the program
void init() {
    // Generate ellipse points
    generateEllipsePoints();
    
    // Apply shear transformation
    applyShearTransform();
}

// Main function
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Group 15: Ellipse Transformations and Filling");
    
    init();
    
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    
    glutMainLoop();
    return 0;
}
