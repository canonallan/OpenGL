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
center_x, center_y = 2, -1  # center based on (x-2)²/36 + (y+1)²/25 = 1
a, b = 6, 5  #ellipse parameters: a=sqrt(36), b=sqrt(25)
shear_x, shear_y = 2.0, 2.0  # Shear parameters

def init():
    """Initialize OpenGL settings"""
    glClearColor(1.0, 1.0, 1.0, 1.0)  # White background
    glMatrixMode(GL_PROJECTION)
    glLoadIdentity()
    # Set the coordinate system to match the updated grid
    gluOrtho2D(-15.0, 15.0, -15.0, 17.0)  # Updated grid range
    glMatrixMode(GL_MODELVIEW)

def draw_grid():
    """Draw a coordinate grid for reference"""
    glColor3f(0.8, 0.8, 0.8)  # Light gray
    glLineWidth(0.5)
    
    # Draw horizontal grid lines
    for y in range(-15, 18):  # range
        glBegin(GL_LINES)
        glVertex2f(-15, y)
        glVertex2f(15, y)
        glEnd()
        
        # Label y-axis values (except at origin)
        if y != 0:
            glColor3f(0, 0, 0)  # Black text
            glRasterPos2f(-0.5, y - 0.1)
            for c in str(y):
                glutBitmapCharacter(GLUT_BITMAP_8_BY_13, ord(c))
            glColor3f(0.8, 0.8, 0.8)  # Back to light gray
    
    # Draw vertical grid lines
    for x in range(-15, 16):  # range
        glBegin(GL_LINES)
        glVertex2f(x, -15)
        glVertex2f(x, 17)
        glEnd()
        
        # Label x-axis values (except at origin)
        if x != 0:
            glColor3f(0, 0, 0)  # Black text
            glRasterPos2f(x - 0.1, -0.5)
            for c in str(x):
                glutBitmapCharacter(GLUT_BITMAP_8_BY_13, ord(c))
            glColor3f(0.8, 0.8, 0.8)  # Back to light gray
    
    # Draw coordinate axes
    glColor3f(0, 0, 0)  # Black
    glLineWidth(2.0)
    
    # x-axis
    glBegin(GL_LINES)
    glVertex2f(-15, 0)
    glVertex2f(15, 0)
    glEnd()
    
    # y-axis
    glBegin(GL_LINES)
    glVertex2f(0, -15)
    glVertex2f(0, 17)
    glEnd()

def draw_axis_labels():
    """Draw axis labels with more details"""
    # X-axis label
    glColor3f(1, 0, 0)  # Red for X-axis
    glRasterPos2f(14.5, -0.5)
    for c in "X":
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, ord(c))
    
    # Y-axis label
    glColor3f(0, 0, 1)  # Blue for Y-axis
    glRasterPos2f(-0.7, 16.5)
    for c in "Y":
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, ord(c))
    
    # Origin label
    glColor3f(0, 0, 0)  # Black
    glRasterPos2f(-0.6, -0.6)
    for c in "0":
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, ord(c))
    
    # Draw tick marks at major intervals
    glColor3f(0, 0, 0)
    glLineWidth(1.0)
    
    # X-axis ticks
    for x in range(-15, 16, 1):  # Ticks every unit
        if x != 0:  # Skip origin
            glBegin(GL_LINES)
            glVertex2f(x, -0.2)
            glVertex2f(x, 0.2)
            glEnd()
    
    # Y-axis ticks
    for y in range(-15, 18, 1):  # Ticks every unit
        if y != 0:  # Skip origin
            glBegin(GL_LINES)
            glVertex2f(-0.2, y)
            glVertex2f(0.2, y)
            glEnd()

def world_to_screen(wx, wy):
    """Convert world coordinates to screen coordinates"""
    range_x = 15.0 - (-15.0)
    range_y = 17.0 - (-15.0)
    sx = int((wx - (-15.0)) / range_x * window_width)
    sy = int((1.0 - (wy - (-15.0)) / range_y) * window_height)
    return sx, sy

def screen_to_world(sx, sy):
    """Convert screen coordinates to world coordinates"""
    range_x = 15.0 - (-15.0)
    range_y = 17.0 - (-15.0)
    wx = -15.0 + (sx / window_width) * range_x
    wy = -15.0 + (1.0 - (sy / window_height)) * range_y
    return wx, wy

def is_inside_original_ellipse(x, y):
    """Check if a point is inside the original ellipse"""
    return ((x - center_x)**2 / a**2 + (y - center_y)**2 / b**2) <= 1.0

def is_inside_sheared_ellipse(x, y):
    """Check if a point is inside the sheared ellipse"""
    det = 1.0 - shear_x * shear_y
    if det == 0:
        return False
    inv_x = (x - shear_x * y) / det
    inv_y = (-shear_y * x + y) / det
    return is_inside_original_ellipse(inv_x, inv_y)

def draw_original_ellipse():
    """Draw the original ellipse with center at (2,-1)"""
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
    glRasterPos2f(center_x - 0.6, center_y - 0.6)
    for c in f"Center ({center_x},{center_y})":
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, ord(c))
    
    # Label the ellipse equation
    glRasterPos2f(-14.5, 16.0)  # Adjusted position for new grid
    for c in "Ellipse: (x-2)^2/36 + (y+1)^2/25 = 1":
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, ord(c))

def draw_filled_ellipse():
    """Draw the original ellipse filled with cyan color using flood-fill"""
    # Start flood-fill from the center
    seed_x, seed_y = world_to_screen(center_x, center_y)
    
    # Track filled pixels
    filled = [[False] * window_height for _ in range(window_width)]
    stack = [(seed_x, seed_y)]
    
    glPointSize(1.0)
    glColor3f(0, 1, 1)  # Cyan
    glBegin(GL_POINTS)
    
    while stack:
        x, y = stack.pop()
        
        if not (0 <= x < window_width and 0 <= y < window_height) or filled[x][y]:
            continue
        
        wx, wy = screen_to_world(x, y)
        if is_inside_original_ellipse(wx, wy):
            filled[x][y] = True
            glVertex2f(wx, wy)
            
            # Add neighboring pixels
            stack.append((x + 1, y))
            stack.append((x - 1, y))
            stack.append((x, y + 1))
            stack.append((x, y - 1))
    
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
    glRasterPos2f(-14.5, 16.0)
    for c in "Flood-Fill Ellipse (Cyan)":
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
    glRasterPos2f(new_center_x - 0.6, new_center_y - 0.6)
    for c in f"New Center ({new_center_x:.1f},{new_center_y:.1f})":
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, ord(c))
    
    # Label shear parameters
    glColor3f(0, 0, 0)  # Black
    glRasterPos2f(-14.5, 16.0)
    for c in f"Shear: X={shear_x}, Y={shear_y}":
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, ord(c))


def draw_filled_sheared_ellipse():
    """Draw the sheared ellipse filled with green color using boundary-fill"""
    # Calculate new center after shear
    new_center_x = center_x + shear_x * center_y
    new_center_y = center_y + shear_y * center_x
    
    # Start boundary-fill from the new center
    seed_x, seed_y = world_to_screen(new_center_x, new_center_y)
    
    # Track filled pixels
    filled = [[False] * window_height for _ in range(window_width)]
    
    # Use an iterative approach with a stack
    stack = [(seed_x, seed_y)]
    
    # Fill the ellipse first
    glPointSize(1.0)
    glColor3f(0, 1, 0)  # Green
    glBegin(GL_POINTS)
    
    while stack:
        x, y = stack.pop()
        
        # Skip if outside window or already filled
        if not (0 <= x < window_width and 0 <= y < window_height) or filled[x][y]:
            continue
        
        # Convert to world coordinates
        wx, wy = screen_to_world(x, y)
        
        # Check if inside the sheared ellipse
        if is_inside_sheared_ellipse(wx, wy):
            filled[x][y] = True
            glVertex2f(wx, wy)
            
            # Add neighboring pixels
            stack.append((x + 1, y))
            stack.append((x - 1, y))
            stack.append((x, y + 1))
            stack.append((x, y - 1))
    
    glEnd()
    
    # Draw the boundary after filling to ensure it's visible
    draw_sheared_ellipse()
    
    # Label the ellipse
    glColor3f(0, 0, 0)  # Black
    glRasterPos2f(-14.5, 16.0)
    for c in "Boundary-Fill Sheared Ellipse (Green)":
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, ord(c))

def draw_antialiased_ellipse():
    """Draw the original ellipse with OpenGL's built-in antialiasing"""
    glEnable(GL_LINE_SMOOTH)
    glEnable(GL_BLEND)
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST)
    
    glColor3f(0, 0, 1)  # Blue
    glLineWidth(1.0)
    
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
    glRasterPos2f(-14.5, 16.0)
    for c in "Anti-aliased Ellipse (OpenGL)":
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, ord(c))
    
    # Add zoomed-in view to show anti-aliasing effect
    draw_zoom_box(center_x + a, center_y, 1.0)

def wuAntiAliasedEllipse():
    """Draw anti-aliased ellipse using improved Wu's algorithm"""
    glEnable(GL_BLEND)
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
    
    # Use smaller point size for better control
    glPointSize(0.8)
    
    glBegin(GL_POINTS)
    
    # Use smaller step size for smoother curve
    step = 0.0005
    for angle in np.arange(0, 2 * np.pi, step):
        exact_x = center_x + a * np.cos(angle)
        exact_y = center_y + b * np.sin(angle)
        
        # Get integer pixel coordinates
        pixel_x = int(np.floor(exact_x))
        pixel_y = int(np.floor(exact_y))
        
        # Calculate sub-pixel position
        frac_x = exact_x - pixel_x
        frac_y = exact_y - pixel_y
        
        # Draw center pixel with calculated intensity
        center_intensity = 1.0
        glColor4f(0.0, 0.0, 0.0, center_intensity)
        glVertex2f(exact_x, exact_y)
        
        # Calculate and draw anti-aliased pixels
        radius = 0.15  # Smaller radius for sharper edges
        for dx in np.arange(-radius, radius + 0.15, 0.15):
            for dy in np.arange(-radius, radius + 0.15, 0.15):
                if dx == 0.0 and dy == 0.0:
                    continue
                
                dist = np.sqrt(dx*dx + dy*dy)
                if dist <= radius:
                    alpha = (radius - dist) / radius
                    alpha *= 0.2  # Reduced intensity for sharper edges
                    
                    glColor4f(0.0, 0.0, 0.0, alpha)
                    glVertex2f(exact_x + dx, exact_y + dy)
    
    glEnd()
    glDisable(GL_BLEND)
    
    # Label the ellipse (only if not in zoom box)
    if display_mode == 6:  # Only show label in main view
        glColor3f(0, 0, 0)
        glRasterPos2f(-14.5, 16.0)
        for c in "Custom Anti-Aliased Ellipse (Wu's)":
            glutBitmapCharacter(GLUT_BITMAP_9_BY_15, ord(c))
        
        # Add zoomed view indicator
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
    
    # Save current viewport/projection
    glPushMatrix()
    glMatrixMode(GL_PROJECTION)
    glPushMatrix()
    glLoadIdentity()
    
    # Set up zoom viewport (bottom-right corner)
    glViewport(window_width - 200, 0, 200, 200)
    gluOrtho2D(x - 1.0, x + 1.0, y - 1.0, y + 1.0)
    
    # Draw the zoomed portion without recursion
    glEnable(GL_BLEND)
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
    
    # Draw the anti-aliased portion in the zoom box
    step = 0.0005
    glPointSize(0.8)
    glBegin(GL_POINTS)
    for angle in np.arange(0, 2 * np.pi, step):
        exact_x = center_x + a * np.cos(angle)
        exact_y = center_y + b * np.sin(angle)
        
        glColor4f(0.0, 0.0, 0.0, 1.0)
        glVertex2f(exact_x, exact_y)
        
        radius = 0.15
        for dx in np.arange(-radius, radius + 0.15, 0.15):
            for dy in np.arange(-radius, radius + 0.15, 0.15):
                if dx == 0.0 and dy == 0.0:
                    continue
                dist = np.sqrt(dx*dx + dy*dy)
                if dist <= radius:
                    alpha = (radius - dist) / radius * 0.2
                    glColor4f(0.0, 0.0, 0.0, alpha)
                    glVertex2f(exact_x + dx, exact_y + dy)
    glEnd()
    glDisable(GL_BLEND)
    
    # Restore original viewport/projection
    glPopMatrix()
    glMatrixMode(GL_MODELVIEW)
    glPopMatrix()
    glViewport(0, 0, window_width, window_height)

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
        wuAntiAliasedEllipse()

    
    # Draw title
    glColor3f(0, 0, 0)
    glRasterPos2f(-14.5, -14.5)  # Adjusted position
    title = "ICS 2311: Computer Graphics - Group 15"
    for c in title:
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, ord(c))
    
    # Draw mode information and instructions
    glColor3f(0, 0, 0)
    glRasterPos2f(-14.5, 15.5)
    mode_texts = [
        "Original Ellipse",
        "Cyan Filled Ellipse (Flood Fill)",
        "Sheared Ellipse",
        "Green Filled Sheared Ellipse (Boundary Fill)",
        "Anti-Aliased Ellipse (OpenGL)",
        "Custom Anti-Aliased Ellipse Implementation"
    ]
    for c in mode_texts[display_mode-1]:
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, ord(c))
    
    glRasterPos2f(-14.5, 15.0)
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
    # Maintain aspect ratio
    aspect = width / height
    range_y = 17.0 - (-15.0)  # Updated range
    range_x = range_y * aspect
    gluOrtho2D(-15.0, -15.0 + range_x, -15.0, 17.0)  # Updated grid range
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