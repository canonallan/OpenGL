#include <GL/glut.h>
#include <cmath>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <stack> // Added for flood-fill

// Constants for the ellipse equation based on (x-2)²/36 + (y+1)²/25 = 1
const float a = 6.0f; // Semi-major axis = sqrt(36)
const float b = 5.0f; // Semi-minor axis = sqrt(25)

// Center coordinates from the equation
const float centerX = 2.0f;  // h = 2 in (x-h)²
const float centerY = -1.0f; // k = -1 in (y-k)² (or +1 in (y+1)²)

// Window size
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

// Shear parameters
const float shearX = 2.0f;
const float shearY = 2.0f;

// Grid properties
const float GRID_MIN_X = -15.0f;
const float GRID_MAX_X = 15.0f;
const float GRID_MIN_Y = -15.0f;
const float GRID_MAX_Y = 17.0f;

// Structure to represent a point in ellipse coordinates
struct Point {
    float x, y;
    Point(float _x, float _y) : x(_x), y(_y) {}
};

// Structure for pixel coordinates (used in flood-fill and boundary-fill)
struct Pixel {
    int x, y;
    Pixel(int _x, int _y) : x(_x), y(_y) {}
};

// Global variables
std::vector<Point> originalEllipsePoints;
std::vector<Point> shearedEllipsePoints;
int currentMode = 0; // 0=Original, 1=Cyan-filled, 2=Sheared, 3=Green-filled, 4=Anti-aliased, 5=Custom AA

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

// Check if point is inside the original ellipse
bool isInsideOriginalEllipse(float x, float y) {
    float dx = x - centerX;
    float dy = y - centerY;
    
    // Ellipse equation: (x-h)²/a² + (y-k)²/b² <= 1
    return ((dx*dx)/(a*a) + (dy*dy)/(b*b)) <= 1.0f;
}

// Corrected isInsideShearedEllipse function
bool isInsideShearedEllipse(float x, float y) {
    // Apply inverse shear transformation to convert to original ellipse coordinates
    float det = 1.0f - shearX * shearY;
    if (det == 0.0f) return false; // Prevent division by zero
    float invX = (x - shearX * y) / det;
    float invY = (-shearY * x + y) / det;
    
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
    
    // Label equation
    glRasterPos2f(GRID_MIN_X + 0.5f, GRID_MAX_Y - 2.0f);
    const char* eqn = "Ellipse: (x-2)^2/36 + (y+1)^2/25 = 1";
    for (const char* c = eqn; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
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
    
    // Label shear parameters
    glRasterPos2f(GRID_MIN_X + 0.5f, GRID_MAX_Y - 2.0f);
    char shearInfo[50];
    sprintf(shearInfo, "Shear: X=%.1f, Y=%.1f", shearX, shearY);
    for (char* c = shearInfo; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }
}

// Flood-fill algorithm for the original ellipse (Cyan)
void floodFillOriginalEllipse() {
    // Cyan color
    float fillColor[3] = {0.0f, 1.0f, 1.0f}; // Cyan (RGB)
    
    // Start from the center of the ellipse (seed point)
    int seedX, seedY;
    worldToScreen(centerX, centerY, seedX, seedY);
    
    std::stack<Pixel> stack;
    stack.push(Pixel(seedX, seedY));
    
    // Create a buffer to track filled pixels
    bool** filled = new bool*[WINDOW_WIDTH];
    for (int i = 0; i < WINDOW_WIDTH; i++) {
        filled[i] = new bool[WINDOW_HEIGHT];
        for (int j = 0; j < WINDOW_HEIGHT; j++) {
            filled[i][j] = false;
        }
    }
    
    glPointSize(1.0f);
    glColor3f(fillColor[0], fillColor[1], fillColor[2]);
    glBegin(GL_POINTS);
    
    while (!stack.empty()) {
        Pixel p = stack.top();
        stack.pop();
        
        int x = p.x;
        int y = p.y;
        
        // Skip if already filled or outside window
        if (x < 0 || x >= WINDOW_WIDTH || y < 0 || y >= WINDOW_HEIGHT || filled[x][y]) {
            continue;
        }
        
        // Convert to world coordinates to check if inside ellipse
        float wx, wy;
        screenToWorld(x, y, wx, wy);
        
        if (isInsideOriginalEllipse(wx, wy)) {
            filled[x][y] = true;
            glVertex2f(wx, wy);
            
            // Push neighboring pixels
            stack.push(Pixel(x + 1, y));
            stack.push(Pixel(x - 1, y));
            stack.push(Pixel(x, y + 1));
            stack.push(Pixel(x, y - 1));
        }
    }
    
    glEnd();
    
    // Clean up
    for (int i = 0; i < WINDOW_WIDTH; i++) {
        delete[] filled[i];
    }
    delete[] filled;
}

// : Boundary-fill algorithm for the sheared ellipse (Green)
void boundaryFillShearedEllipse() {
    // Green color for filling
    float fillColor[3] = {0.0f, 1.0f, 0.0f}; // Green (RGB)
    
    // Calculate transformed center
    float transformedCenterX = centerX + shearX * centerY;
    float transformedCenterY = centerY + shearY * centerX;
    
    // Use an iterative approach with a stack
    std::stack<Pixel> stack;
    int seedX, seedY;
    worldToScreen(transformedCenterX, transformedCenterY, seedX, seedY);
    stack.push(Pixel(seedX, seedY));
    
    // Create a buffer to track filled pixels
    bool** filled = new bool*[WINDOW_WIDTH];
    for (int i = 0; i < WINDOW_WIDTH; i++) {
        filled[i] = new bool[WINDOW_HEIGHT];
        for (int j = 0; j < WINDOW_HEIGHT; j++) {
            filled[i][j] = false;
        }
    }
    
    // Fill the ellipse first
    glPointSize(1.0f);
    glColor3f(fillColor[0], fillColor[1], fillColor[2]);
    glBegin(GL_POINTS);
    
    while (!stack.empty()) {
        Pixel p = stack.top();
        stack.pop();
        
        int x = p.x;
        int y = p.y;
        
        // Skip if outside window or already filled
        if (x < 0 || x >= WINDOW_WIDTH || y < 0 || y >= WINDOW_HEIGHT || filled[x][y]) {
            continue;
        }
        
        // Convert to world coordinates
        float wx, wy;
        screenToWorld(x, y, wx, wy);
        
        // Check if inside the sheared ellipse
        if (isInsideShearedEllipse(wx, wy)) {
            filled[x][y] = true;
            glVertex2f(wx, wy);
            
            // Push neighboring pixels
            stack.push(Pixel(x + 1, y));
            stack.push(Pixel(x - 1, y));
            stack.push(Pixel(x, y + 1));
            stack.push(Pixel(x, y - 1));
        }
    }
    
    glEnd();
    
    // Draw the boundary after filling to ensure it's visible
    drawShearedEllipse();
    
    // Clean up
    for (int i = 0; i < WINDOW_WIDTH; i++) {
        delete[] filled[i];
    }
    delete[] filled;
}

// Draw anti-aliased ellipse using OpenGL's built-in anti-aliasing
void drawAntiAliasedEllipse() {
    // Enable anti-aliasing
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    
    glLineWidth(1.0f);
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

void customAntiAliasedEllipse() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Use smaller point size for sharper lines
    glPointSize(0.8f);
    
    glBegin(GL_POINTS);
    
    // Use smaller step size for smoother curve
    const float step = 0.0005f;
    for (float angle = 0.0f; angle <= 2.0f * M_PI; angle += step) {
        float exactX = centerX + a * cos(angle);
        float exactY = centerY + b * sin(angle);
        
        // Get integer pixel coordinates
        int pixelX = (int)floor(exactX);
        int pixelY = (int)floor(exactY);
        
        // Calculate sub-pixel position
        float fracX = exactX - pixelX;
        float fracY = exactY - pixelY;
        
        // Draw center pixel with calculated intensity
        float centerIntensity = 1.0f;
        glColor4f(0.0f, 0.0f, 0.0f, centerIntensity);
        glVertex2f(exactX, exactY);
        
        // Calculate and draw anti-aliased pixels
        float radius = 0.15f;  // Smaller radius for sharper edges
        for(float dx = -radius; dx <= radius; dx += 0.15f) {
            for(float dy = -radius; dy <= radius; dy += 0.15f) {
                if(dx == 0.0f && dy == 0.0f) continue;
                
                float dist = sqrt(dx*dx + dy*dy);
                if(dist <= radius) {
                    float alpha = (radius - dist) / radius;
                    alpha *= 0.2f;  // Reduced intensity for sharper edges
                    
                    glColor4f(0.0f, 0.0f, 0.0f, alpha);
                    glVertex2f(exactX + dx, exactY + dy);
                }
            }
        }
    }
    glEnd();
    
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
            // Boundary is already drawn inside boundaryFillShearedEllipse
            break;
        case 4: // Anti-aliased ellipse
            drawAntiAliasedEllipse();
            break;
        case 5: // Custom anti-aliasing implementation
            customAntiAliasedEllipse();
            break;
    }
    
    // For anti-aliased modes, add a zoomed view of the edge
    if (currentMode == 4 || currentMode == 5) {
        // Save current viewport/projection
        glPushMatrix();
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        
        // Set up zoom viewport (bottom-right corner)
        glViewport(WINDOW_WIDTH-200, 0, 200, 200);
        gluOrtho2D(centerX + a - 1.0f, centerX + a + 1.0f, 
                   centerY - 1.0f, centerY + 1.0f);

        // Redraw relevant content
        glColor3f(0.0f, 0.0f, 0.0f);
        if (currentMode == 4) {
            drawAntiAliasedEllipse();
        } else {
            customAntiAliasedEllipse();
        }

        // Restore original viewport/projection
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
        glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

        // Draw zoom indicator box
        float zoomAreaX = centerX + a;
        float zoomAreaY = centerY;
        glColor3f(1.0f, 0.0f, 0.0f);
        glBegin(GL_LINE_LOOP);
        glVertex2f(zoomAreaX - 0.5f, zoomAreaY - 0.5f);
        glVertex2f(zoomAreaX + 0.5f, zoomAreaY - 0.5f);
        glVertex2f(zoomAreaX + 0.5f, zoomAreaY + 0.5f);
        glVertex2f(zoomAreaX - 0.5f, zoomAreaY + 0.5f);
        glEnd();
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