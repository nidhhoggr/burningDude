//
//  status.h
//  sdl2_testing
//
//  Created by Mike Farrell on 10/9/13.
//  Copyright (c) 2013 Mike Farrell. All rights reserved.
//

#ifndef sdl2_testing_status_h
#define sdl2_testing_status_h

void init_status_lives(GameState *game);
void draw_status_lives(GameState *game);
void shutdown_status_lives(GameState *game);

void init_game_over(GameState *game);
void draw_game_over(GameState *game);
void shutdown_game_over(GameState *game);

void init_game_win(GameState *game);
void draw_game_win(GameState *game);
void shutdown_game_win(GameState *game);


#endif
