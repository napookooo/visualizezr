#include <stdio.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_audio.h"
#include "SDL2/SDL_mixer.h"
#include <time.h>

typedef struct{
  int x, y;
} Man;

typedef struct{
  int space;
} Keyboard;

typedef struct {
  Man man;

  Mix_Music *music;
  Mix_Chunk *chunk;

  SDL_AudioStream *stream;

  SDL_Renderer *renderer;

  Keyboard keyState;

} GameState;

int processEvents(SDL_Window *window, GameState *game);
void doRender(SDL_Renderer *renderer, GameState *game);
void loadGame(GameState *game);
void spaceFunctions(GameState *game);


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

  if (game.stream != NULL){
    SDL_FreeAudioStream(game.stream);
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
          case SDLK_t:
            // for (int i=0; i<10; i++){
            //   printf("audio device name: %s\n", SDL_GetAudioDeviceName(i, SDL_TRUE));
            // }
            int avail = SDL_AudioStreamAvailable(game->stream);  // this is in bytes, not samples!
            if (avail < 100) {
              printf("I'm still waiting on %d bytes of data!\n", 100 - avail);
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
  if(state[SDL_SCANCODE_A]){game->man.x -= 5;}
  if(state[SDL_SCANCODE_D]){game->man.x += 5;}
  if(state[SDL_SCANCODE_W]){game->man.y -= 5;}
  if(state[SDL_SCANCODE_S]){game->man.y += 5;}
  if (state[SDL_SCANCODE_SPACE]){
    spaceFunctions(game);
    game->keyState.space = 1;
  }
  else{
    game->keyState.space = 0;
  }

  return done;
}


void doRender(SDL_Renderer *renderer, GameState *game){
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
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

  game->music = Mix_LoadMUS("a little remedy.mp3");
  if (game->music == NULL){
    printf("error load a little remedy.mp3: %s\n", Mix_GetError());
  }
  
  // You put data at Sint16/mono/22050Hz, you get back data at Float32/stereo/48000Hz
  game->stream = SDL_NewAudioStream(AUDIO_S16, 1, 22050, AUDIO_F32, 2, 48000);
  if (game->stream == NULL) {
    printf("Uhoh, stream failed to create: %s\n", SDL_GetError());
  }

  printf("%d\n", Mix_GetNumMusicDecoders());

  game->keyState.space = 0;


  game->man.x = 900;
  game->man.y = 500;

}

void spaceFunctions(GameState *game){

    printf("Playing music %d\n", Mix_PlayingMusic());
    if (!game->keyState.space){
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
    }

  Sint16 samples[1024];
  int num_samples = sizeof(int) * 100;
  // you tell it the number of _bytes_, not samples, you're putting!
  int rc = SDL_AudioStreamPut(game->stream, samples, num_samples * sizeof (Sint16));
  if (rc == -1) {
    printf("Uhoh, failed to put samples in stream: %s\n", SDL_GetError());
    return;
  }

}
