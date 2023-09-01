#include <math.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

// Bresenham's line algorithm to draw lines between two points
void drawLine(int x1, int y1, int x2, int y2, char b[]) {
  int dx = abs(x2 - x1);
  int dy = abs(y2 - y1);
  int sx = (x1 < x2) ? 1 : -1;
  int sy = (y1 < y2) ? 1 : -1;
  int err = dx - dy;

  while (1) {
    // Plot the point if it's within the screen bounds
    if (x1 >= 0 && x1 < 80 && y1 >= 0 && y1 < 22) {
      b[x1 + 80 * y1] = '.';
    }

    // Break if the end point is reached
    if (x1 == x2 && y1 == y2) {
      break;
    }

    int e2 = 2 * err;
    if (e2 > -dy) {
      err -= dy;
      x1 += sx;
    }
    if (e2 < dx) {
      err += dx;
      y1 += sy;
    }
  }
}

int main() {
  float A = 0, B = 0; // Rotation angles for the cube
  char b[1760];       // Buffer to store the ASCII representation
  printf("\x1b[2J");  // Clear the terminal

  for (;;) {
    memset(b, 32, 1760); // Reset the buffer

    int projectedPoints[8][2]; // Store the 2D projected points of the cube

    // Cube vertices in 3D
    float points[8][3] = {{-1, -1, -1}, {1, -1, -1}, {1, 1, -1}, {-1, 1, -1},
                          {-1, -1, 1},  {1, -1, 1},  {1, 1, 1},  {-1, 1, 1}};

    // Rotate, project points, and store them in the projectedPoints array
    for (int i = 0; i < 8; i++) {
      float x = points[i][0];
      float y = points[i][1];
      float z = points[i][2];

      // Rotate around Y-axis
      float tempX = x * cos(A) - z * sin(A);
      float tempZ = x * sin(A) + z * cos(A);

      // Rotate around X-axis
      float tempY = y * cos(B) - tempZ * sin(B);
      z = y * sin(B) + tempZ * cos(B);

      x = tempX;
      y = tempY;

      // Perspective projection
      float scale = 2 / (z + 5);
      int xp = (int)(40 + 20 * x * scale);
      int yp = (int)(12 + 10 * y * scale);

      projectedPoints[i][0] = xp;
      projectedPoints[i][1] = yp;
    }

    // Draw lines between projected points to form the cube
    for (int i = 0; i < 4; i++) {
      // Draw the bottom face
      drawLine(projectedPoints[i][0], projectedPoints[i][1],
               projectedPoints[(i + 1) % 4][0], projectedPoints[(i + 1) % 4][1],
               b);
      // Draw the top face
      drawLine(projectedPoints[i + 4][0], projectedPoints[i + 4][1],
               projectedPoints[((i + 1) % 4) + 4][0],
               projectedPoints[((i + 1) % 4) + 4][1], b);
      // Connect top and bottom faces
      drawLine(projectedPoints[i][0], projectedPoints[i][1],
               projectedPoints[i + 4][0], projectedPoints[i + 4][1], b);
    }

    printf("\x1b[H"); // Move the cursor to the top-left corner
    for (int k = 0; k < 1760; k++) {
      // Print the buffer content
      putchar(k % 80 ? b[k] : 10);
    }
    A += 0.04;     // Adjust the rotation angle for Y-axis
    B += 0.03;     // Adjust the rotation angle for X-axis
    usleep(30000); // Delay to control the spinning speed
  }
  return 0;
}
