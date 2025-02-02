#include <assert.h>
#include <math.h>
#include <SDL/SDL.h>
#include <SDL/SDL_endian.h>
#include <stdlib.h>

#include "object.h"
#include "graphics.h"

object *g_obj;

/*
 *  Quit SDL and free memory associated with running main().
 */

void cleanup() {
  SDL_Quit();
  if (g_obj != NULL) {
    free_object(g_obj);
    g_obj = NULL;
  }
} /* cleanup() */

/*
 *  Draw the object onto the screen.
 */

void draw(SDL_Surface *screen, matrix *trans) {
  if (!g_obj) return;
  assert(screen && trans);

  sdl_lock(screen);
  draw_shade_background(screen);
  render(screen, g_obj, *trans);
  sdl_unlock(screen);
  SDL_Flip(screen);
} /* draw() */

/*
 *  main() is used to run the functions in hw12.c using command line arguments.
 *  It prints information related to running these functions.
 */

int main(int argc, char *argv[]) {
  SDL_Surface *screen = NULL;
  matrix trans = magnify(8, 8, 8);

  /* Check program arguments */
  if (argc != 2) {
    printf("Specify a single object file on the command line.  e.g.: %s %s\n",
           argv[0], "test_objs/cube.obj");
    return 1;
  }

  /* Read obj file */
  g_obj = read_object(argv[1]);
  if (g_obj == NULL) {
    printf("Error reading object.\n");
    return 1;
  }

  /* Initialize SDL */
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
    exit(1);
  }

  /* Free memory upon program exit */
  atexit(cleanup);

  /* Open a window */
  screen = SDL_SetVideoMode(640, 480, 32, SDL_HWSURFACE);
  if (screen == NULL) {
    fprintf(stderr, "Unable to set 640x480 video mode: %s\n", SDL_GetError());
    exit(1);
  }

  /* Setup object view and draw */
  scale_and_center(g_obj, screen);
  trans = identity();
  draw(screen, &trans);

  int mouse = -1;
  /* Event loop */
  while(1) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
      /* The window was closed */
      if (event.type == SDL_QUIT) {
        exit(0);
      }

      else if (event.type == SDL_VIDEOEXPOSE) {
        draw(screen, &trans);
      }

      else if (event.type == SDL_KEYDOWN) {
        /* Quit on ESC or q */
        if (event.key.keysym.sym == SDLK_ESCAPE ||
            event.key.keysym.sym == SDLK_q) {
          exit(0);
        }

        /* Revert to original view */
        else if (event.key.keysym.sym == SDLK_o) {
          trans = identity();
          draw(screen, &trans);
        }

        /* Zoom in */
        else if (event.key.keysym.sym == SDLK_PERIOD ||
                 event.key.keysym.sym == SDLK_PAGEUP) {
          trans = mult(trans, magnify(2, 2, 2));
          draw(screen, &trans);
        }

        /* Zoom out */
        else if (event.key.keysym.sym == SDLK_COMMA ||
                 event.key.keysym.sym == SDLK_PAGEDOWN) {
          trans = mult(trans, magnify(.5, .5, .5));
          draw(screen, &trans);
        }

        /* Rotate about X axis */
        else if (event.key.keysym.sym == SDLK_UP) {
          trans = mult(trans, rotate_x(10));
          draw(screen, &trans);
        }
        else if (event.key.keysym.sym == SDLK_DOWN) {
          trans = mult(trans, rotate_x(-10));
          draw(screen, &trans);
        }

        /* Rotate about Y axis */
        else if (event.key.keysym.sym == SDLK_LEFT) {
          trans = mult(trans, rotate_y(-10));
          draw(screen, &trans);
        }
        else if (event.key.keysym.sym == SDLK_RIGHT) {
          trans = mult(trans, rotate_y(10));
          draw(screen, &trans);
        }
      }

      /* Save mouse position on click */
      else if (event.type == SDL_MOUSEBUTTONDOWN) {
        if (event.button.button == SDL_BUTTON_LEFT ||
            event.button.button == SDL_BUTTON_RIGHT) {
          if (mouse == -1) {
            mouse = event.button.button;
          }
        }
      }
      /* Rotate/zoom the view when dragging the mouse */
      else if (event.type == SDL_MOUSEMOTION && mouse != -1) {
        SDL_Event event_array[100] = { 0 };

        /* Rotate with the left button */
        if (mouse == SDL_BUTTON_LEFT) {
          trans = mult(trans, rotate_y(event.motion.xrel));
          trans = mult(trans, rotate_x(-event.motion.yrel));
        }
        /* Zoom with the right button */
        else if (mouse == SDL_BUTTON_RIGHT) {
          double zoom = pow(2.0, (double)-event.motion.yrel / screen->h);
          trans = mult(trans, magnify(zoom, zoom, zoom));
        }
        draw(screen, &trans);

        /* Absorb excessive mouse motion events. */

        while (SDL_PeepEvents(event_array, 2, SDL_PEEKEVENT, 0) > 1) {
          if (event_array[0].type == SDL_MOUSEMOTION &&
              event_array[1].type == SDL_MOUSEMOTION) {
            SDL_PeepEvents(event_array, 1, SDL_GETEVENT, 0);
          }
        }
      }
      /* Stop rotating/zooming the view */
      else if (event.type == SDL_MOUSEBUTTONUP) {
        if (mouse != -1) {
          if (event.button.button == mouse) {
            mouse = -1;
          }
        }
      }
    }
  }
} /* main() */
