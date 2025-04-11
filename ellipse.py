import sys
import math
import numpy as np
from OpenGL.GL import *
from OpenGL.GLUT import *
from OpenGL.GLU import *
from OpenGL.GLUT import GLUT_BITMAP_8_BY_13, GLUT_BITMAP_9_BY_15


# Global variables
display_mode = 1  # Default display mode
window_width, window_height = 800, 600
center_x, center_y = -2, 2
a, b = 3, 2  # Ellipse parameters
shear_x, shear_y = 2.0, 2.0  # Shear parameters

def init():
    """Initialize OpenGL settings"""
    glClearColor(1.0, 1.0, 1.0, 1.0)  # White background
    glMatrixMode(GL_PROJECTION)
    glLoadIdentity()
    # Set the coordinate system to match the problem description
    gluOrtho2D(-10.0, 10.0, -10.0, 10.0)
    glMatrixMode(GL_MODELVIEW)

def draw_grid():
    """Draw a coordinate grid for reference"""
    glColor3f(0.8, 0.8, 0.8)  # Light gray
    glLineWidth(0.5)
    
    # Draw horizontal grid lines
    for y in range(-10, 11):
        glBegin(GL_LINES)
        glVertex2f(-10, y)
        glVertex2f(10, y)
        glEnd()
        
        # Label y-axis values (except at origin)
        if y != 0:
            glColor3f(0, 0, 0)  # Black text
            glRasterPos2f(-0.5, y)
            for c in str(y):
                glutBitmapCharacter(GLUT_BITMAP_8_BY_13, ord(c))
            glColor3f(0.8, 0.8, 0.8)  # Back to light gray
    
    # Draw vertical grid lines
    for x in range(-10, 11):
        glBegin(GL_LINES)
        glVertex2f(x, -10)
        glVertex2f(x, 10)
        glEnd()
        
        # Label x-axis values (except at origin)
        if x != 0:
            glColor3f(0, 0, 0)  # Black text
            glRasterPos2f(x, -0.5)
            for c in str(x):
                glutBitmapCharacter(GLUT_BITMAP_8_BY_13, ord(c))
            glColor3f(0.8, 0.8, 0.8)  # Back to light gray
    
    # Draw coordinate axes
    glColor3f(0, 0, 0)  # Black
    glLineWidth(2.0)
    
    # x-axis
    glBegin(GL_LINES)
    glVertex2f(-10, 0)
    glVertex2f(10, 0)
    glEnd()
    
    # y-axis
    glBegin(GL_LINES)
    glVertex2f(0, -10)
    glVertex2f(0, 10)
    glEnd()

def draw_axis_labels():
    """Draw axis labels with more details"""
    # X-axis label
    glColor3f(1, 0, 0)  # Red for X-axis
    glRasterPos2f(9.5, -0.5)
    for c in "X":
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, ord(c))
    
    # Y-axis label
    glColor3f(0, 0, 1)  # Blue for Y-axis
    glRasterPos2f(-0.7, 9.5)
    for c in "Y":
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, ord(c))
    
    # Origin label
    glColor3f(0, 0, 0)  # Black
    glRasterPos2f(0.2, 0.2)
    for c in "O":
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, ord(c))
    
    # Draw tick marks at major intervals
    glColor3f(0, 0, 0)
    glLineWidth(1.5)
    
    # X-axis ticks
    for x in range(-10, 11, 2):  # Major ticks every 2 units
        if x != 0:  # Skip origin
            glBegin(GL_LINES)
            glVertex2f(x, -0.2)
            glVertex2f(x, 0.2)
            glEnd()
    
    # Y-axis ticks
    for y in range(-10, 11, 2):  # Major ticks every 2 units
        if y != 0:  # Skip origin
            glBegin(GL_LINES)
            glVertex2f(-0.2, y)
            glVertex2f(0.2, y)
            glEnd()

def draw_original_ellipse():
    """Draw the original ellipse with center at (-2,2)"""
    glColor3f(0, 0, 1)  # Blue
    glLineWidth(2.0)
    
    # Draw ellipse using parametric equation
    glBegin(GL_LINE_LOOP)
    for i in range(360):
        theta = i * math.pi / 180
        x = center_x + a * math.cos(theta)
        y = center_y + b * math.sin(theta)
        glVertex2f(x, y)
    glEnd()
    
    # Mark the center
    glColor3f(1, 0, 0)  # Red
    glPointSize(5.0)
    glBegin(GL_POINTS)
    glVertex2f(center_x, center_y)
    glEnd()
    
    # Label the center
    glRasterPos2f(center_x - 0.8, center_y - 0.5)
    for c in f"Center ({center_x},{center_y})":
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, ord(c))
    
    # Label the ellipse parameters
    glRasterPos2f(-9.5, 9.0)
    for c in f"Ellipse Equation: ((x+2)²/9) + ((y-2)²/4) = 1":
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, ord(c))

def draw_filled_ellipse():
    """Draw the original ellipse filled with cyan color"""
    glColor3f(0, 1, 1)  # Cyan
    
    # Draw filled ellipse using parametric equation
    glBegin(GL_POLYGON)
    for i in range(360):
        theta = i * math.pi / 180
        x = center_x + a * math.cos(theta)
        y = center_y + b * math.sin(theta)
        glVertex2f(x, y)
    glEnd()
    
    # Draw the border
    glColor3f(0, 0, 1)  # Blue
    glLineWidth(2.0)
    glBegin(GL_LINE_LOOP)
    for i in range(360):
        theta = i * math.pi / 180
        x = center_x + a * math.cos(theta)
        y = center_y + b * math.sin(theta)
        glVertex2f(x, y)
    glEnd()
    
    # Label the ellipse
    glColor3f(0, 0, 0)  # Black
    glRasterPos2f(-9.5, 9.0)
    for c in f"Flood-Fill Ellipse (Cyan)":
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, ord(c))

def draw_sheared_ellipse():
    """Draw the ellipse after applying shear transformation"""
    # Calculate points of the original ellipse and apply shear transformation
    sheared_points = []
    for i in range(360):
        theta = i * math.pi / 180
        x = center_x + a * math.cos(theta)
        y = center_y + b * math.sin(theta)
        
        # Apply shear transformation
        new_x = x + shear_x * y
        new_y = y + shear_y * x
        
        sheared_points.append((new_x, new_y))
    
    # Draw the sheared ellipse
    glColor3f(0, 0.5, 0)  # Dark green
    glLineWidth(2.0)
    glBegin(GL_LINE_LOOP)
    for x, y in sheared_points:
        glVertex2f(x, y)
    glEnd()
    
    # Calculate new center after shear
    new_center_x = center_x + shear_x * center_y
    new_center_y = center_y + shear_y * center_x
    
    # Mark the new center
    glColor3f(1, 0, 0)  # Red
    glPointSize(5.0)
    glBegin(GL_POINTS)
    glVertex2f(new_center_x, new_center_y)
    glEnd()
    
    # Label the new center
    glRasterPos2f(new_center_x - 0.8, new_center_y - 0.5)
    for c in f"Sheared Center ({new_center_x:.1f},{new_center_y:.1f})":
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, ord(c))
    
    # Draw original center for reference
    glColor3f(0.5, 0.5, 0.5)  # Gray
    glPointSize(3.0)
    glBegin(GL_POINTS)
    glVertex2f(center_x, center_y)
    glEnd()
    
    # Label shear parameters
    glColor3f(0, 0, 0)  # Black
    glRasterPos2f(-9.5, 9.0)
    for c in f"Shear Transformation: shearX = {shear_x}, shearY = {shear_y}":
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, ord(c))

def draw_filled_sheared_ellipse():
    """Draw the sheared ellipse filled with green color"""
    # Calculate points of the original ellipse and apply shear transformation
    sheared_points = []
    for i in range(360):
        theta = i * math.pi / 180
        x = center_x + a * math.cos(theta)
        y = center_y + b * math.sin(theta)
        
        # Apply shear transformation
        new_x = x + shear_x * y
        new_y = y + shear_y * x
        
        sheared_points.append((new_x, new_y))
    
    # Draw the filled sheared ellipse
    glColor3f(0, 1, 0)  # Green
    glBegin(GL_POLYGON)
    for x, y in sheared_points:
        glVertex2f(x, y)
    glEnd()
    
    # Draw the border
    glColor3f(0, 0.5, 0)  # Dark green
    glLineWidth(2.0)
    glBegin(GL_LINE_LOOP)
    for x, y in sheared_points:
        glVertex2f(x, y)
    glEnd()
    
    # Calculate new center after shear
    new_center_x = center_x + shear_x * center_y
    new_center_y = center_y + shear_y * center_x
    
    # Mark the new center
    glColor3f(1, 0, 0)  # Red
    glPointSize(5.0)
    glBegin(GL_POINTS)
    glVertex2f(new_center_x, new_center_y)
    glEnd()
    
    # Label the ellipse
    glColor3f(0, 0, 0)  # Black
    glRasterPos2f(-9.5, 9.0)
    for c in f"Boundary-Fill Sheared Ellipse (Green)":
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, ord(c))

def draw_antialiased_ellipse():
    """Draw the original ellipse with OpenGL's built-in antialiasing"""
    glEnable(GL_LINE_SMOOTH)
    glEnable(GL_BLEND)
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST)
    
    glColor3f(0, 0, 1)  # Blue
    glLineWidth(2.0)
    
    # Draw ellipse using parametric equation
    glBegin(GL_LINE_LOOP)
    for i in range(360):
        theta = i * math.pi / 180
        x = center_x + a * math.cos(theta)
        y = center_y + b * math.sin(theta)
        glVertex2f(x, y)
    glEnd()
    
    glDisable(GL_LINE_SMOOTH)
    glDisable(GL_BLEND)
    
    # Label the ellipse
    glColor3f(0, 0, 0)  # Black
    glRasterPos2f(-9.5, 9.0)
    for c in f"Anti-aliased Ellipse (OpenGL)":
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, ord(c))
    
    # Add zoomed-in view to show anti-aliasing effect
    draw_zoom_box(center_x + a, center_y, 1.0)

def draw_custom_antialiased_ellipse():
    """Draw the ellipse with custom antialiasing by simulating fade effect"""
    glEnable(GL_BLEND)
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
    
    # Draw multiple ellipses with varying alpha values
    for thickness in range(5, 0, -1):
        alpha = thickness / 5.0
        glColor4f(0, 0, 1, alpha)  # Blue with alpha
        
        glLineWidth(thickness)
        glBegin(GL_LINE_LOOP)
        for i in range(360):
            theta = i * math.pi / 180
            x = center_x + a * math.cos(theta)
            y = center_y + b * math.sin(theta)
            glVertex2f(x, y)
        glEnd()
    
    glDisable(GL_BLEND)
    
    # Label the ellipse
    glColor3f(0, 0, 0)  # Black
    glRasterPos2f(-9.5, 9.0)
    for c in f"Anti-aliased Ellipse (Custom Method)":
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, ord(c))
    
    # Add zoomed-in view to show anti-aliasing effect
    draw_zoom_box(center_x + a, center_y, 1.0)

def draw_zoom_box(x, y, size):
    """Draw a zoomed-in view at (x,y) to show anti-aliasing effect"""
    # Draw the zoom area indicator
    glColor3f(1, 0, 0)  # Red
    glLineWidth(1.0)
    glBegin(GL_LINE_LOOP)
    glVertex2f(x - size/2, y - size/2)
    glVertex2f(x + size/2, y - size/2)
    glVertex2f(x + size/2, y + size/2)
    glVertex2f(x - size/2, y + size/2)
    glEnd()
    
    # Draw zoomed view in corner
    zoom_factor = 10
    box_size = 3
    
    # Draw zoom box background
    glColor3f(0.9, 0.9, 0.9)  # Light gray
    glBegin(GL_QUADS)
    glVertex2f(-9.5, -7)
    glVertex2f(-9.5 + box_size, -7)
    glVertex2f(-9.5 + box_size, -7 + box_size)
    glVertex2f(-9.5, -7 + box_size)
    glEnd()
    
    # Draw border around zoom box
    glColor3f(0, 0, 0)  # Black
    glLineWidth(1.0)
    glBegin(GL_LINE_LOOP)
    glVertex2f(-9.5, -7)
    glVertex2f(-9.5 + box_size, -7)
    glVertex2f(-9.5 + box_size, -7 + box_size)
    glVertex2f(-9.5, -7 + box_size)
    glEnd()
    
    # Draw "Zoom" label
    glRasterPos2f(-9.5, -7 - 0.5)
    for c in "Zoomed (10x)":
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, ord(c))

def display():
    """Main display function"""
    glClear(GL_COLOR_BUFFER_BIT)
    
    # Draw grid and axis labels for reference
    draw_grid()
    draw_axis_labels()
    
    # Display the appropriate figure based on display_mode
    if display_mode == 1:
        draw_original_ellipse()
    elif display_mode == 2:
        draw_filled_ellipse()
    elif display_mode == 3:
        draw_sheared_ellipse()
    elif display_mode == 4:
        draw_filled_sheared_ellipse()
    elif display_mode == 5:
        draw_antialiased_ellipse()
    elif display_mode == 6:
        draw_custom_antialiased_ellipse()
    
    # Draw title
    glColor3f(0, 0, 0)
    glRasterPos2f(-3, -8.5)
    title = "ICS 2311: Computer Graphics - Group 15"
    for c in title:
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, ord(c))
    
    # Draw mode information
    glRasterPos2f(-9.5, -9.5)
    mode_text = f"Mode {display_mode}: "
    if display_mode == 1:
        mode_text += "Original Ellipse"
    elif display_mode == 2:
        mode_text += "Cyan-Filled Ellipse"
    elif display_mode == 3:
        mode_text += "Sheared Ellipse"
    elif display_mode == 4:
        mode_text += "Green-Filled Sheared Ellipse"
    elif display_mode == 5:
        mode_text += "Anti-aliased Ellipse (OpenGL)"
    elif display_mode == 6:
        mode_text += "Anti-aliased Ellipse (Custom)"
    
    for c in mode_text:
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, ord(c))
    
    # Draw instructions
    glRasterPos2f(-9.5, -9.0)
    instructions = "Press 1-6 to switch modes, ESC to exit"
    for c in instructions:
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, ord(c))
    
    glutSwapBuffers()

def reshape(width, height):
    """Handle window resize"""
    global window_width, window_height
    window_width, window_height = width, height
    
    # Update viewport to match new window dimensions
    glViewport(0, 0, width, height)
    glMatrixMode(GL_PROJECTION)
    glLoadIdentity()
    gluOrtho2D(-10.0, 10.0, -10.0, 10.0)
    glMatrixMode(GL_MODELVIEW)

def keyboard(key, x, y):
    """Handle keyboard input"""
    global display_mode
    
    if key == b'1':
        display_mode = 1
    elif key == b'2':
        display_mode = 2
    elif key == b'3':
        display_mode = 3
    elif key == b'4':
        display_mode = 4
    elif key == b'5':
        display_mode = 5
    elif key == b'6':
        display_mode = 6
    elif key == b'\x1b':  # ESC key
        sys.exit(0)
    
    glutPostRedisplay()

def main():
    """Main function"""
    glutInit(sys.argv)
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB)
    glutInitWindowSize(window_width, window_height)
    glutInitWindowPosition(100, 100)
    glutCreateWindow(b"Group 15: Ellipse Drawing and Transformation")
    
    init()
    glutDisplayFunc(display)
    glutReshapeFunc(reshape)
    glutKeyboardFunc(keyboard)
    
    print("Interactive Ellipse Program:")
    print("  Press 1: View original ellipse")
    print("  Press 2: View cyan-filled ellipse")
    print("  Press 3: View sheared ellipse")
    print("  Press 4: View green-filled sheared ellipse")
    print("  Press 5: View OpenGL anti-aliased ellipse")
    print("  Press 6: View custom anti-aliased ellipse")
    print("  Press ESC: Exit the program")
    
    glutMainLoop()

if __name__ == "__main__":
    main()

