#define GEO_USE_OSN

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>

#ifdef __WIN32
#include <windows.h>
#endif

#include "SDL2/SDL.h"

#include "geo.h"
#include "view.h"
#include "algorithm.h"

static volatile bool worker_working;

typedef struct {
  SDL_Renderer *renderer;
  volatile View *view;
} worker_data_t;

static int init_work(void *worker_data_) {
  worker_working = true;
  worker_data_t *worker_data = (worker_data_t *) worker_data_;
  SDL_Renderer *renderer = worker_data->renderer;
  GEO_NewGeo(time(NULL));

  worker_data->view = GEO_NewView(renderer);
  worker_working = false;
  return 0;
}

int geo_main(void) {
  SDL_Init(SDL_INIT_VIDEO);

  SDL_Window *window;
  SDL_Renderer *renderer;

  bool running = true;

  window = SDL_CreateWindow(
    "GEO",
    SDL_WINDOWPOS_UNDEFINED,
    SDL_WINDOWPOS_UNDEFINED,
    800,
    600,
    SDL_WINDOW_OPENGL
  );

  if (window == NULL) {
    printf("Could not create window: %s\n", SDL_GetError());
    return EXIT_FAILURE;
  }

  renderer = SDL_CreateRenderer(
    window,
    -1,
    SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED
  );

  if (renderer == NULL) {
    printf("Could not create renderer: %s\n", SDL_GetError());
    return EXIT_FAILURE;
  }

  SDL_Thread *worker = NULL;
  worker_data_t worker_data = {renderer, NULL};
  bool work_initiated = false;
 
  // Ser the window background before running 
  SDL_SetRenderDrawColor(renderer, 0x20, 0x20, 0x20, 0x04);
  SDL_RenderClear(renderer);
  SDL_RenderPresent(renderer);

  while (running) {
    bool alive = true;
    if (!work_initiated) {
      // worker not working, start new work
      work_initiated = true;
      worker = SDL_CreateThread(init_work, "Worker", &worker_data);
    }

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      // check for quit event only
      if (SDL_QUIT == event.type) {
        running = false;
      }
    }

    // if work has not finished, loop
    if (worker_working) continue;

    // work has finished
    View *view = (View *) worker_data.view;
    GEO_UpdateView(view);
    // release thread resources
    SDL_WaitThread(worker, NULL);
    worker = NULL;

    while (alive) {
      SDL_SetRenderDrawColor(renderer, 0x20, 0x20, 0x20, 0xFF);
      SDL_RenderClear(renderer);

      GEO_DrawView(view);

      SDL_RenderPresent(renderer);

      SDL_Event event;
      while (SDL_PollEvent(&event)) {
        if (SDL_QUIT == event.type) {
          alive = false;
          running = false;
        } else if (SDL_WINDOWEVENT == event.type) {
          if (SDL_WINDOWEVENT_CLOSE == event.window.event) {
            alive = false;
            running = false;
          }
        } else if (SDL_KEYDOWN == event.type) {
          switch(event.key.keysym.scancode) {
            case SDL_SCANCODE_Z:
              GEO_ALG_CalculateTileDistance(
                  NULL,
                  TILE(view->centre_y, view->centre_x),
                  8000,
                  &geo->tile_distances
              );
              GEO_UpdateView(view);
              break;
            case SDL_SCANCODE_S:
              GEO_SaveImage(view, geo->seed);
              break;
            case SDL_SCANCODE_SPACE:
              alive = false;
              break;
            case SDL_SCANCODE_PAGEDOWN:
              view->mode = (view->mode + 1) % 10;
              if (GEO_VIEW_MODE_DISTANCE == view->mode) {
                GEO_ALG_CalculateTileDistance(
                    NULL,
                    TILE(view->centre_y, view->centre_x),
                    8000,
                    &geo->tile_distances
                );
              }
              GEO_UpdateView(view);
              break;
            case SDL_SCANCODE_PAGEUP:
              view->mode = (view->mode + 9) % 10;
              if (GEO_VIEW_MODE_DISTANCE == view->mode) {
                GEO_ALG_CalculateTileDistance(
                    NULL,
                    TILE(view->centre_y, view->centre_x),
                    8000,
                    &geo->tile_distances
                );
              }
              GEO_UpdateView(view);
              break;
            case SDL_SCANCODE_LEFTBRACKET:
              if (view->tile_size > 1) {
                view->move_y = view->move_y / view->tile_size;
                view->move_x = view->move_x / view->tile_size;
                view->tile_size /= 2;
                view->move_y = view->move_y * view->tile_size;
                view->move_x = view->move_x * view->tile_size;
                GEO_UpdateView(view);
              }
              break;
            case SDL_SCANCODE_RIGHTBRACKET:
              if (view->tile_size < 128) {
                view->move_y = view->move_y / view->tile_size;
                view->move_x = view->move_x / view->tile_size;
                view->tile_size *= 2;
                view->move_y = view->move_y * view->tile_size;
                view->move_x = view->move_x * view->tile_size;
                GEO_UpdateView(view);
              }
              break;
            default:
              break;
          }
        }
      }
      const uint8_t *key_state = SDL_GetKeyboardState(NULL);
      if (key_state[SDL_SCANCODE_Z]) {
        GEO_UpdateView(view);
      }
      if (key_state[SDL_SCANCODE_LEFT]) {
        if (view->move_x > 0) {
          view->move_x -= 4;
        }
        view->centre_x = view->move_x / view->tile_size;
      }
      if (key_state[SDL_SCANCODE_UP]) {
        if (view->move_y > 0) {
          view->move_y -= 4;
        }
        view->centre_y = view->move_y / view->tile_size;
      }
      if (key_state[SDL_SCANCODE_RIGHT]) {
        if ((view->move_x / view->tile_size) < MAP_SIZE - 1) {
          view->move_x += 4;
        }
        view->centre_x = view->move_x / view->tile_size;
      }
      if (key_state[SDL_SCANCODE_DOWN]) {
        if ((view->move_y / view->tile_size) < MAP_SIZE - 1) {
          view->move_y += 4;
        }
        view->centre_y = view->move_y / view->tile_size;
      }
      if (key_state[SDL_SCANCODE_ESCAPE]) {
        alive = false;
        running = false;
      }
    }

    GEO_DestroyGeo();
    GEO_DestroyView(view);
    work_initiated = false;
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return EXIT_SUCCESS;
}

#ifdef __WIN32

int CALLBACK WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR     lpCmdLine,
    int       nCmdShow) {  
  // WinMain parameters are unused
  (void)hInstance;
  (void)hPrevInstance;
  (void)lpCmdLine;
  (void)nCmdShow;
  geo_main();
  return 0;
}

#else

int main() {
  geo_main();
  return EXIT_SUCCESS;
}

#endif
