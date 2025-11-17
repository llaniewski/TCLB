/*
  This software contains source code provided by NVIDIA Corporation
  taken from materials for the book "CUDA by example"
  Modified by L Laniewski-Wollk for CLB project
*/


#ifndef GUI_H
#define GUI_H

#include "Consts.h"
#include "Global.h"
#include "Solver.h"
#include <cuda.h>
#include <cuda_gl_interop.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>


struct gui_window {
    Solver * solver;
    int window_width;
    int window_height;
	SDL_Window* sdl_window;
	SDL_Renderer* sdl_renderer;
	SDL_Texture* sdl_display;
  SDL_Texture *sdl_texture;
  SDL_Surface* sdl_surface;
  uchar4* outputBitmap;
  SDL_Rect srcrect, dstrect;
  int calibrate();
public:	
    gui_window(int window_width_, int window_height_, Solver * solver_);
    ~gui_window();
    int eventloop();
};

#endif  // __GPU_ANIM_H__

