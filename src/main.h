//
//  main.h
//  sdl2_testing
//
//  Created by Mike Farrell on 10/9/13.
//  Copyright (c) 2013 Mike Farrell. All rights reserved.
//

#ifndef sdl2_testing_main_h
#define sdl2_testing_main_h

#define STATUS_STATE_LIVES 0
#define STATUS_STATE_GAME  1
#define STATUS_STATE_GAMEOVER 2
#define STATUS_STATE_WIN 3

#define NUM_STARS 200
#define NUM_LEDGES 120

#include "SDL.h"
#include "SDL_ttf.h"
#include "SDL_image.h"
#include "SDL_mixer.h"

typedef struct
{
  float x, y;
  float dx, dy;
  short lives;
  char *name;
  int onLedge, isDead;
  
  int animFrame, facingLeft, slowingDown;
} Man;

typedef struct
{
  int x, y, baseX, baseY, mode;
  float phase;
} Star;

typedef struct
{
  int x, y, w, h;
} Ledge;

typedef struct
{
  float scrollX;
  
  //Players
  Man man;
  
  //Stars
  Star stars[NUM_STARS];
  
  //Ledges
  Ledge ledges[NUM_LEDGES];
  
  //Images
  SDL_Texture *star;
  SDL_Texture *manFrames[2];
  SDL_Texture *brick;
  SDL_Texture *fire;
  SDL_Texture *label, *label2;
  int labelW, labelH, label2W, label2H;
  
  //Joystick
  SDL_Joystick *joystick;
  
  //Fonts
  TTF_Font *font;
  
  //time
  int time, deathCountdown;
  int statusState;
  
  //Sounds
  int musicChannel;
  Mix_Chunk *bgMusic, *jumpSound, *landSound, *dieSound;
  
  //Renderer
  SDL_Renderer *renderer;
  
  //Window
  SDL_Window *window;
} GameState;

//Prototypes (function references)
void doRender(SDL_Renderer *renderer, GameState *game);

#endif
