#include <stdio.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_mixer.h"
#include <time.h>

typedef struct{
  int x, y;
} Man;

typedef struct {
  Man man;

  Mix_Music *music;
  Mix_Chunk *chunk;

  SDL_Renderer *renderer;

} GameState;

int processEvents(SDL_Window *window, GameState *game);
void doRender(SDL_Renderer *renderer, GameState *game);
void loadGame(GameState *game);


// MAIN FUNCTION

int main(int argc, char *argv[]){
  GameState game;
  SDL_Window *window = NULL;
  SDL_Renderer *renderer = NULL;

  if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO ) < 0 ){
    printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
  }

  if (Mix_Init(MIX_INIT_WAVPACK) != 0){
    printf( "Mixer could not initialize! Mix init Error: %s\n", Mix_GetError() );
  }

  window = SDL_CreateWindow("Game Window",
                            SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED,
                            1920,
                            1080,
                            0
                            );
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

  game.renderer = renderer;
  loadGame(&game);

  int done = 0;

  while(!done){
    done = processEvents(window, &game);

    doRender(renderer, &game);

    // SDL_Delay(20);
  }

  if (game.music != NULL){
    Mix_FreeMusic(game.music);
  }
  if (game.chunk != NULL){
    Mix_FreeChunk(game.chunk);
  }

  SDL_DestroyWindow(window);
  SDL_DestroyRenderer(renderer);

  SDL_Quit();
  return 0;
}


int processEvents(SDL_Window *window, GameState *game){
  SDL_Event event;
  int done = 0;

  while(SDL_PollEvent(&event)){
    switch(event.type){
      case SDL_WINDOWEVENT_CLOSE:{
        if(window){
          SDL_DestroyWindow(window);
          window = NULL;
          done = 1;
        }
      }
      break;
      case SDL_KEYDOWN:{
        switch (event.key.keysym.sym) {
          case SDLK_ESCAPE:
            done = 1;
          case SDLK_SPACE:
            printf("Playing music %d\n", Mix_PlayingMusic());
            if (Mix_PlayingMusic()){
              if (Mix_PausedMusic()){
                Mix_ResumeMusic();
              }else {
                Mix_PauseMusic();
              }
            }
            else if (!Mix_PlayingMusic()){
              Mix_PlayMusic(game->music, -1);
            }
          break;
        }
      }
    break;
    case SDL_QUIT:
      done = 1;
      break;
    }
  }

  const Uint8 *state = SDL_GetKeyboardState(NULL);
  if(state[SDL_SCANCODE_LEFT]){game->man.x -= 5;}
  if(state[SDL_SCANCODE_RIGHT]){game->man.x += 5;}
  if(state[SDL_SCANCODE_UP]){game->man.y -= 5;}
  if(state[SDL_SCANCODE_DOWN]){game->man.y += 5;}

  return done;
}


void doRender(SDL_Renderer *renderer, GameState *game){
  SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
  SDL_RenderClear(renderer);
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

  SDL_Rect rect = { game->man.x, game->man.y, 200, 200 };
  SDL_RenderFillRect(renderer, &rect);

  SDL_RenderPresent(renderer);
}

void loadGame(GameState *game){

  if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2024 ) < 0 ){
    printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
  }

  // game->chunk = Mix_LoadWAV("a little remedy.wav");
  // if (game->chunk == NULL){
  //   printf("error load a little remedy.mp3: %s\n", Mix_GetError());
  // }
  game->music = Mix_LoadMUS("a little remedy.mp3");
  if (game->music == NULL){
    printf("error load a little remedy.mp3: %s\n", Mix_GetError());
  }

  game->man.x = 900;
  game->man.y = 500;

}

