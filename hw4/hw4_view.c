#include "hw4.h"  // for the julia() function

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <X11/Xlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define WIDTH         (700)
#define HEIGHT        (700)
#define MAX_COLOR     (255)
#define R             (3)

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

void drawJuliaANSI(complex_t, int, int);
void drawJuliaX(Display *, Window, GC, complex_t);
unsigned long rgbFromRange(double, double);

/*
 * Render the julia set in either terminal or through X
 * Other beautiful c values:
 * {-0.8, 0.156}
 */

int main(int argc, char *argv[]) {

  complex_t c = {-0.4, 0.9};  // Default value

  if (argc == 3) {
    c.x = atof(argv[1]);
    c.y = atof(argv[2]);
  }

  Display *display;
  Window window;
  GC gc;
  int screen;

  display = XOpenDisplay(NULL);

  // display in terminal if display not set
  if (!display) {

    // error if stdout is not a TTY
    if (!isatty(STDOUT_FILENO)) {
      fprintf(stderr,
              "Error: No X11 display found and stdout is not a terminal.\n");
      return 1;
    }

    // get the terminal size
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    int width = w.ws_col;
    int height = w.ws_row;

    // display in terminal
    drawJuliaANSI(c, height, width);

    // print suggestion
    printf("\033[31mRun using X forwarding for better results!\033[0m\n");
    return 0;
  }

  // create a window with desired properties (border, background, etc)
  screen = DefaultScreen(display);
  window = XCreateSimpleWindow(display, RootWindow(display, screen),
                               10, 10, WIDTH, HEIGHT, 1,
                               BlackPixel(display, screen),
                               BlackPixel(display, screen));

  // listen for events
  XSelectInput(display, window, ExposureMask | KeyPressMask);
  XMapWindow(display, window);
  gc = XCreateGC(display, window, 0, NULL);

  XEvent event;

  // render
  while (1) {
    XNextEvent(display, &event);
    if (event.type == Expose) {
      drawJuliaX(display, window, gc, c);
    }
    if (event.type == KeyPress) {
      break;
    }
  }

  XCloseDisplay(display);
  return 0;
} /* main() */

/*
 * Draw the Julia set in X display
 */

void drawJuliaX(Display *display, Window window, GC gc, complex_t c) {
  for (int y = 0; y < HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x++) {

      // compress x and y to values from -R to R
      complex_t z = {(x - WIDTH / 2.0) * 2.0 * R / WIDTH,
                     (y - HEIGHT / 2.0) * 2.0 * R / HEIGHT};
      int iterations = julia(z, c);

      // color pixel based on if in Julia set
      if ((iterations == MAX_JULIA) || (iterations < 7)) {
        XSetForeground(display, gc,
                       BlackPixel(display, DefaultScreen(display)));
      } else {
        // color based on iteration count, but multiply by 3 because
        // this version of julia() produces low values
        double range = iterations * 3.0 / MAX_JULIA;
        unsigned long color = rgbFromRange(range, MAX(range, 1.0));
        XSetForeground(display, gc, color);
      }
      XDrawPoint(display, window, gc, x, y);
    }
  }
} /* drawJuliaX() */

/*
 * Draw the Julia set in terminal
 */

void drawJuliaANSI(complex_t c, int height, int width) {
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {

      // compress x and y into -R to R range
      double realZ = (x - width / 2.0) * 2.0 * R / width;
      double imagZ = (y - height / 2.0) * 2.0 * R / height;
      complex_t Z = {realZ, imagZ};

      int iter = julia(Z, c);
      // set background color
      if ((iter == MAX_JULIA) || (iter < 7)) {
        printf("\033[48;5;0m "); // black
      } else {
        // convert to 8 bit representation
        double range = iter * 3.0 / MAX_JULIA;
        unsigned long color = rgbFromRange(range, MAX(range, 1.0));
        unsigned char r = (color >> 16) & 0xff;
        unsigned char g = (color >> 8) & 0xff;
        unsigned char b = color & 0xff;
        printf("\033[48;2;%d;%d;%dm ", r, g, b);
      }
    }
    printf("\033[0m\n"); // reset color
  }
} /* drawJuliaANSI() */

/*
 * Allocates an X11 color by interpolating a range of RGB values.
 * The colors go from 0xff0000 -> 0x00ff00 -> 0x0000ff.
 * The first argument is a value between 0 and 1 which picks a point within this
 * range to select the color from. The second argument is a value between 0 and
 * 1 which determines the brightness of this color, 0 being black and 1 being
 * full saturation.
 */

unsigned long rgbFromRange(double pct, double brightness) {
  if (pct > 1.0) {
    pct -= floor(pct);
  }
  if (pct <= 0.5) {
    /* Color is within [0xff0000, 0x00ff00]. */
    unsigned long offset = (pct * 2.0) * 0xff;
    unsigned long r = (0xff - offset) * brightness;
    unsigned long g = offset * brightness;
    return (r << 16) | (g << 8);
  }
  else {
    /* Color is within (0x00ff00, 0x0000ff]. */
    unsigned long offset = ((pct - 0.5) * 2.0) * 0xff;
    unsigned long g = (0xff - offset) * brightness;
    unsigned long b = offset * brightness;
    return (g << 8) | b;
  }
} /* rgbFromRange() */

/* vim: set ft=c sw=4 ts=4 et : */
