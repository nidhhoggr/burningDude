#include "SDL.h"
#include <stdio.h>
#include <time.h>
#include "main.h"
#include "status.h"

#define GRAVITY  0.35f

void initStars(GameState *game)
{
  //init stars
  for(int i = 0; i < NUM_STARS; i++)
  {
    game->stars[i].baseX = 320+random()%38400;
    game->stars[i].baseY = random()%480;
    game->stars[i].mode = random()%2;
    game->stars[i].phase = 2*3.14*(random()%360)/360.0f;
  }
}

void loadGame(GameState *game)
{
  SDL_Surface *surface = NULL;

  //Load images and create rendering textures from them
  surface = IMG_Load("files/images/star.png");
  if(surface == NULL)
  {
    printf("Cannot find star.png!\n\n");
    SDL_Quit();
    exit(1);
  }
  
  game->star = SDL_CreateTextureFromSurface(game->renderer, surface);
  SDL_FreeSurface(surface);
  
  surface = IMG_Load("files/images/man_lta.png");
  if(surface == NULL)
  {
    printf("Cannot find man_lta.png!\n\n");
    SDL_Quit();
    exit(1);
  }
  game->manFrames[0] = SDL_CreateTextureFromSurface(game->renderer, surface);
  SDL_FreeSurface(surface);

  surface = IMG_Load("files/images/man_ltb.png");
  if(surface == NULL)
  {
    printf("Cannot find man_ltb.png!\n\n");
    SDL_Quit();
    exit(1);
  }
  game->manFrames[1] = SDL_CreateTextureFromSurface(game->renderer, surface);
  SDL_FreeSurface(surface);
  
  surface = IMG_Load("files/images/fire.png");
  game->fire = SDL_CreateTextureFromSurface(game->renderer, surface);
  SDL_FreeSurface(surface);
  
  surface = IMG_Load("files/images/brick.png");
  game->brick = SDL_CreateTextureFromSurface(game->renderer, surface);
  SDL_FreeSurface(surface);
  
  //Load fonts
  game->font = TTF_OpenFont("files/fonts/Crazy-Pixel.ttf", 48);
  if(!game->font)
  {
    printf("Cannot find font file!\n\n");
    SDL_Quit();
    exit(1);
  }
  
  //Load sounds
  game->bgMusic = Mix_LoadWAV("files/sounds/music.wav");
  if(game->bgMusic != NULL)
  {
    Mix_VolumeChunk(game->bgMusic, 32);
  }
  game->dieSound = Mix_LoadWAV("files/sounds/die.wav");
  game->jumpSound = Mix_LoadWAV("files/sounds/jump.wav");
  game->landSound = Mix_LoadWAV("files/sounds/land.wav");
  
  game->label = NULL;
  game->label2 = NULL;

  game->man.x = 100;
  //game->man.x = 38000;
  game->man.y = 240-40;
  game->man.dx = 0;
  game->man.dy = 0;
  game->man.onLedge = 0;
  game->man.animFrame = 0;
  game->man.facingLeft = 1;
  game->man.slowingDown = 0;
  game->man.lives = 3;
  game->man.isDead = 0;
  game->statusState = STATUS_STATE_LIVES;
  
  init_status_lives(game);
  
  game->time = 0;
  game->scrollX = 0;
  game->deathCountdown = -1;

  initStars(game);

  //init ledges
  for(int i = 0; i < NUM_LEDGES-20; i++)
  {
    game->ledges[i].w = 256;
    game->ledges[i].h = 64;
    game->ledges[i].x = i*384;
    if(i == 0)
      game->ledges[i].y = 400;
    else
      game->ledges[i].y = 300+100-random()%200;
  }
  
  for(int i = NUM_LEDGES-20; i < NUM_LEDGES; i++)
  {
    game->ledges[i].w = 256;
    game->ledges[i].h = 64;
    game->ledges[i].x = 350+random()%38400;
    if(i % 2 == 0)
      game->ledges[i].y = 200;
    else
      game->ledges[i].y = 350;
  }
}

void process(GameState *game)
{
  //add time
  game->time++;
  
  if(game->statusState == STATUS_STATE_LIVES)
  {
    if(game->time > 120)
    {
      shutdown_status_lives(game);
      game->statusState = STATUS_STATE_GAME;
      game->musicChannel = Mix_PlayChannel(-1, game->bgMusic, -1);
    }
  }
  else if(game->statusState == STATUS_STATE_GAMEOVER)
  {
    if(game->time > 190)
    {
      SDL_Quit();
      exit(0);
    }
  }
  else if(game->statusState == STATUS_STATE_GAME)
  {
    if(!game->man.isDead)
    {
      //man movement
      Man *man = &game->man;
      man->x += man->dx;
      man->y += man->dy;
      
      if(man->dx != 0 && man->onLedge && !man->slowingDown)
      {
        if(game->time % 8 == 0)
        {
          if(man->animFrame == 0)
          {
            man->animFrame = 1;
          }
          else
          {
            man->animFrame = 0;
          }
        }
      }
      
      if(man->x > 38320)
      {
        init_game_win(game);
        game->statusState = STATUS_STATE_WIN;
      }
      
      man->dy += GRAVITY;
      
      //Star movement
      for(int i = 0; i < NUM_STARS; i++)
      {
        game->stars[i].x = game->stars[i].baseX;
        game->stars[i].y = game->stars[i].baseY;
        
        if(game->stars[i].mode == 0)
        {
          game->stars[i].x = game->stars[i].baseX+sinf(game->stars[i].phase+game->time*0.06f)*75;
        }
        else
        {
          game->stars[i].y = game->stars[i].baseY+cosf(game->stars[i].phase+game->time*0.06f)*75;
        }
      }
    }
    
    if(game->man.isDead && game->deathCountdown < 0)
    {
      game->deathCountdown = 120;
    }
    if(game->deathCountdown >= 0)
    {
      game->deathCountdown--;
      if(game->deathCountdown < 0)
      {
        //init_game_over(game);
        //game->statusState = STATUS_STATE_GAMEOVER;
        
        game->man.lives--;
        
        if(game->man.lives >= 0)
        {
          init_status_lives(game);
          game->statusState = STATUS_STATE_LIVES;
          game->time = 0;
          
          //reset
          game->man.isDead = 0;
          game->man.x = 100;
          game->man.y = 240-40;
          game->man.dx = 0;
          game->man.dy = 0;
          game->man.onLedge = 0;
          initStars(game);
        }
        else
        {
          init_game_over(game);
          game->statusState = STATUS_STATE_GAMEOVER;
          game->time = 0;
        }
      }
    }
  }
  
  game->scrollX = -game->man.x+320;
  if(game->scrollX > 0)
    game->scrollX = 0;
  if(game->scrollX < -38000+320)
    game->scrollX = -38000+320;
}

//useful utility function to see if two rectangles are colliding at all
int collide2d(float x1, float y1, float x2, float y2, float wt1, float ht1, float wt2, float ht2)
{
  return (!((x1 > (x2+wt2)) || (x2 > (x1+wt1)) || (y1 > (y2+ht2)) || (y2 > (y1+ht1))));
}

void collisionDetect(GameState *game)
{
  //Check for collision with enemies
  for(int i = 0; i < NUM_STARS; i++)
  {
    if(collide2d(game->man.x, game->man.y, game->stars[i].x, game->stars[i].y, 48, 48, 32, 32))
    {
      if(!game->man.isDead)
      {
        game->man.isDead = 1;
        Mix_HaltChannel(game->musicChannel);
        Mix_PlayChannel(-1, game->dieSound, 0);
      }
      break;
    }
  }
  
  //Check for falling
  if(game->man.y > 480)
  {
    if(!game->man.isDead)
    {
      game->man.isDead = 1;
      Mix_HaltChannel(game->musicChannel);
      Mix_PlayChannel(-1, game->dieSound, 0);
    }
  }
  
  //Check for collision with any ledges (brick blocks)
  for(int i = 0; i < NUM_LEDGES; i++)
  {
    float mw = 48, mh = 48;
    float mx = game->man.x, my = game->man.y;
    float bx = game->ledges[i].x, by = game->ledges[i].y, bw = game->ledges[i].w, bh = game->ledges[i].h;

    if(mx+mw/2 > bx && mx+mw/2<bx+bw)
    {
      //are we bumping our head?
      if(my < by+bh && my > by && game->man.dy < 0)
      {
        //correct y
        game->man.y = by+bh;
        my = by+bh;
        
        //bumped our head, stop any jump velocity
        game->man.dy = 0;
        game->man.onLedge = 1;
      }
    }
    if(mx+mw > bx && mx<bx+bw)
    {
      //are we landing on the ledge
      if(my+mh > by && my < by && game->man.dy > 0)
      {
        //correct y
        game->man.y = by-mh;
        my = by-mh;
        
        //landed on this ledge, stop any jump velocity
        game->man.dy = 0;
        if(!game->man.onLedge)
        {
          Mix_PlayChannel(-1, game->landSound, 0);
          game->man.onLedge = 1;
        }
      }
    }
  
    if(my+mh > by && my<by+bh)
    {
      //rubbing against right edge
      if(mx < bx+bw && mx+mw > bx+bw && game->man.dx < 0)
      {
        //correct x
        game->man.x = bx+bw;
        mx = bx+bw;
        
        game->man.dx = 0;
      }
      //rubbing against left edge
      else if(mx+mw > bx && mx < bx && game->man.dx > 0)
      {
        //correct x
        game->man.x = bx-mw;
        mx = bx-mw;
        
        game->man.dx = 0;
      }
    }
  }
}

int processEvents(SDL_Window *window, GameState *game)
{
  SDL_Event event;
  int done = 0;
  int joystickLeft = 0, joystickRight = 0, joystickButton1 = 0;
  
  if(game->joystick)
  {
    SDL_JoystickUpdate();
    
    if(SDL_JoystickGetAxis(game->joystick, 0) < -256)
      joystickLeft = 1;
    else if(SDL_JoystickGetAxis(game->joystick, 0) > 256)
      joystickRight = 1;
    
    if(SDL_JoystickGetButton(game->joystick, 0) || SDL_JoystickGetButton(game->joystick, 1) ||
       SDL_JoystickGetButton(game->joystick, 2))
    {
      joystickButton1 = 1;
    }
  }

  while(SDL_PollEvent(&event))
  {
    switch(event.type)
    {
      case SDL_WINDOWEVENT_CLOSE:
      {
        if(window)
        {
          SDL_DestroyWindow(window);
          window = NULL;
          done = 1;
        }
      }
      break;
      case SDL_KEYDOWN:
      {
        switch(event.key.keysym.sym)
        {
          case SDLK_ESCAPE:
            done = 1;
          break;
          case SDLK_UP:
            if(game->man.onLedge)
            {
              game->man.dy = -8;
              game->man.onLedge = 0;
              Mix_PlayChannel(-1, game->jumpSound, 0);
            }
          break;
          case SDLK_m:
            //less annoying mode
            Mix_VolumeChunk(game->dieSound, 0);
            Mix_VolumeChunk(game->bgMusic, 0);
          break;
          case SDLK_RETURN: //alt+enter fullscreen toggle
          {
            if(event.key.keysym.mod & KMOD_ALT)
            {
              if(!(SDL_GetWindowFlags(game->window) & SDL_WINDOW_FULLSCREEN_DESKTOP))
                SDL_SetWindowFullscreen(game->window, SDL_WINDOW_FULLSCREEN_DESKTOP);
              else
                SDL_SetWindowFullscreen(game->window, 0);
            }
          }
          break;
        }
      }
      break;
      case SDL_QUIT:
        //quit out of the game
        done = 1;
      break;
    }
  }
  
  if(game->man.onLedge && joystickButton1)
  {
    game->man.dy = -8;
    game->man.onLedge = 0;
    Mix_PlayChannel(-1, game->jumpSound, 0);
  }

  
  //More jumping
  const Uint8 *state = SDL_GetKeyboardState(NULL);
  if(state[SDL_SCANCODE_UP] || joystickButton1)
  {
    game->man.dy -= 0.2f;
  }
  
  //Walking
  if(state[SDL_SCANCODE_LEFT] || joystickLeft)
  {
    game->man.dx -= 0.5;
    if(game->man.dx < -6)
    {
      game->man.dx = -6;
    }
    game->man.facingLeft = 1;
    game->man.slowingDown = 0;
  }
  else if(state[SDL_SCANCODE_RIGHT] || joystickRight)
  {
    game->man.dx += 0.5;
    if(game->man.dx > 6)
    {
      game->man.dx = 6;
    }
    game->man.facingLeft = 0;
    game->man.slowingDown = 0;
  }
  else
  {
    game->man.animFrame = 0;
    game->man.dx *= 0.8f;
    game->man.slowingDown = 1;
    if(fabsf(game->man.dx) < 0.1f)
    {
      game->man.dx = 0;
    }
  }
  
//  if(state[SDL_SCANCODE_UP])
//  {
//    game->man.y -= 10;
//  }
//  if(state[SDL_SCANCODE_DOWN])
//  {
//    game->man.y += 10;
//  }
  
  return done;
}

void doRender(SDL_Renderer *renderer, GameState *game)
{
  if(game->statusState == STATUS_STATE_LIVES)
  {
    draw_status_lives(game);
  }
  else if(game->statusState == STATUS_STATE_GAMEOVER)
  {
    draw_game_over(game);
  }
  else if(game->statusState == STATUS_STATE_WIN)
  {
    draw_game_win(game);
  }
  else if(game->statusState == STATUS_STATE_GAME)
  {
    //set the drawing color to blue
    SDL_SetRenderDrawColor(renderer, 128, 128, 255, 255);
    
    //Clear the screen (to blue)
    SDL_RenderClear(renderer);
    
    //set the drawing color to white
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    
    for(int i = 0; i < NUM_LEDGES; i++)
    {
      SDL_Rect ledgeRect = { game->scrollX+game->ledges[i].x, game->ledges[i].y, game->ledges[i].w, game->ledges[i].h };
      SDL_RenderCopy(renderer, game->brick, NULL, &ledgeRect);
    }
    
    //draw the star images
    for(int i = 0; i < NUM_STARS; i++)
    {
      SDL_Rect starRect = { game->scrollX+game->stars[i].x, game->stars[i].y, 64, 64 };
      SDL_RenderCopy(renderer, game->star, NULL, &starRect);
    }
    
    //draw a rectangle at man's position
    SDL_Rect rect = { game->scrollX+game->man.x, game->man.y, 48, 48 };
    SDL_RenderCopyEx(renderer, game->manFrames[game->man.animFrame],
                     NULL, &rect, 0, NULL, (game->man.facingLeft == 0));
    
    if(game->man.isDead)
    {
      SDL_Rect rect = { game->scrollX+game->man.x-24+38/2+10, game->man.y-24-83/2, 38, 83 };
      SDL_RenderCopyEx(renderer, game->fire,
                       NULL, &rect, 0, NULL, (game->time%20 < 10));
    }
  }
  
  //We are done drawing, "present" or show to the screen what we've drawn
  SDL_RenderPresent(renderer);
}

int main(int argc, char *argv[])
{
  int mode = 0;
  int flags[] = { 0, SDL_WINDOW_FULLSCREEN, SDL_WINDOW_FULLSCREEN_DESKTOP };
  //argument checking
  for(int i = 1; i < argc; i++)
  {
    if(strcasecmp(argv[i], "-mode") == 0 && i+1 < argc)
    {
      mode = atoi(argv[i+1]);
      if(mode < 0 || mode > 2)
        mode = 0;
    }
  }
  
  
  GameState gameState;
  SDL_Window *window = NULL;                    // Declare a window
  SDL_Renderer *renderer = NULL;                // Declare a renderer
  
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_AUDIO);              // Initialize SDL2
  
  gameState.joystick = SDL_JoystickOpen(0);
  
  srandom((int)time(NULL));
  
  //Create an application window with the following settings:
  window = SDL_CreateWindow("Game",                     // window title
                            SDL_WINDOWPOS_UNDEFINED,           // initial x position
                            SDL_WINDOWPOS_UNDEFINED,           // initial y position
                            640,                               // width, in pixels
                            480,                               // height, in pixels
                            flags[mode]);                      // flags
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  gameState.renderer = renderer;
  gameState.window = window;
  
  //Hide mouse
  SDL_ShowCursor(0);
  
  SDL_RenderSetLogicalSize(renderer, 640, 480);
  
  TTF_Init(); //initialize font system
  
  Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 4096); //initialize sound
  
  loadGame(&gameState);
  
  // The window is open: enter program loop (see SDL_PollEvent)
  int done = 0;
  
  //Event loop
  while(!done)
  {
    //Check for events
    done = processEvents(window, &gameState);
    
    process(&gameState);
    collisionDetect(&gameState);
    
    //Render display
    doRender(renderer, &gameState);
    
    //don't burn up the CPU
    //SDL_Delay(10);
  }
  

  //Shutdown game and unload all memory
  SDL_DestroyTexture(gameState.star);
  SDL_DestroyTexture(gameState.manFrames[0]);
  SDL_DestroyTexture(gameState.manFrames[1]);
  SDL_DestroyTexture(gameState.brick);
  if(gameState.label != NULL)
    SDL_DestroyTexture(gameState.label);
  if(gameState.label2 != NULL)
    SDL_DestroyTexture(gameState.label2);
  if(gameState.joystick)
    SDL_JoystickClose(gameState.joystick);
  TTF_CloseFont(gameState.font);

  Mix_FreeChunk(gameState.bgMusic);
  Mix_FreeChunk(gameState.dieSound);
  Mix_FreeChunk(gameState.jumpSound);
  Mix_FreeChunk(gameState.landSound);

  // Close and destroy the window
  SDL_DestroyWindow(window);
  SDL_DestroyRenderer(renderer);
  
  TTF_Quit();
  Mix_CloseAudio();
  
  // Clean up
  SDL_Quit();
  return 0;
}
