/*
 * Snake game with SDL2 graphics library
 */
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define GRID_SIZE 20
#define SNAKE_INITIAL_LENGTH 5

typedef struct {
  int x, y;
} Point;

typedef struct {
  Point body[SCREEN_WIDTH * SCREEN_HEIGHT / (GRID_SIZE * GRID_SIZE)];
  int length;
  int dx, dy;
} Snake;

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
TTF_Font *font = NULL;
Snake snake;
Point food;
bool running = true;
int score = 0;

void place_food() {
  food.x = (rand() % (SCREEN_WIDTH / GRID_SIZE)) * GRID_SIZE;
  food.y = (rand() % (SCREEN_HEIGHT / GRID_SIZE)) * GRID_SIZE;
}

void init_snake() {
  snake.length = SNAKE_INITIAL_LENGTH;
  snake.dx = GRID_SIZE;
  snake.dy = 0;
  for (int i = 0; i < snake.length; i++) {
    snake.body[i].x = SCREEN_WIDTH / 2 - i * GRID_SIZE;
    snake.body[i].y = SCREEN_HEIGHT / 2;
  }
  score = 0;
}

void move_snake() {
  for (int i = snake.length - 1; i > 0; i--) {
    snake.body[i] = snake.body[i - 1];
  }
  snake.body[0].x += snake.dx;
  snake.body[0].y += snake.dy;

  if (snake.body[0].x == food.x && snake.body[0].y == food.y) {
    snake.length++;
    place_food();
    score++;
  }

  if (snake.body[0].x < 0 || snake.body[0].x >= SCREEN_WIDTH ||
      snake.body[0].y < 0 || snake.body[0].y >= SCREEN_HEIGHT) {
    running = false;
  }

  for (int i = 1; i < snake.length; i++) {
    if (snake.body[0].x == snake.body[i].x &&
        snake.body[0].y == snake.body[i].y) {
      running = false;
    }
  }
}

void handle_input(SDL_Event *event) {
  if (event->type == SDL_QUIT) {
    running = false;
  } else if (event->type == SDL_KEYDOWN) {
    switch (event->key.keysym.sym) {
    case SDLK_UP:
      if (snake.dy == 0) {
        snake.dx = 0;
        snake.dy = -GRID_SIZE;
      }
      break;
    case SDLK_DOWN:
      if (snake.dy == 0) {
        snake.dx = 0;
        snake.dy = GRID_SIZE;
      }
      break;
    case SDLK_LEFT:
      if (snake.dx == 0) {
        snake.dx = -GRID_SIZE;
        snake.dy = 0;
      }
      break;
    case SDLK_RIGHT:
      if (snake.dx == 0) {
        snake.dx = GRID_SIZE;
        snake.dy = 0;
      }
      break;
    }
  }
}

void render_snake() {
  SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
  for (int i = 0; i < snake.length; i++) {
    SDL_Rect rect = {snake.body[i].x, snake.body[i].y, GRID_SIZE, GRID_SIZE};
    SDL_RenderFillRect(renderer, &rect);
  }
}

void render_food() {
  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
  SDL_Rect rect = {food.x, food.y, GRID_SIZE, GRID_SIZE};
  SDL_RenderFillRect(renderer, &rect);
}

void render_message(const char *message) {
  SDL_Color color = {255, 255, 255, 255};
  SDL_Surface *surface = TTF_RenderText_Solid(font, message, color);
  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

  SDL_Rect message_rect = {(SCREEN_WIDTH - surface->w) / 2,
                           (SCREEN_HEIGHT - surface->h) / 2, surface->w,
                           surface->h};
  SDL_RenderCopy(renderer, texture, NULL, &message_rect);

  SDL_FreeSurface(surface);
  SDL_DestroyTexture(texture);
}

void render_score() {
  char score_text[50];
  sprintf(score_text, "Score: %d", score);
  SDL_Color color = {255, 255, 255, 255};
  SDL_Surface *surface = TTF_RenderText_Solid(font, score_text, color);
  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

  SDL_Rect score_rect = {10, 10, surface->w, surface->h};
  SDL_RenderCopy(renderer, texture, NULL, &score_rect);

  SDL_FreeSurface(surface);
  SDL_DestroyTexture(texture);
}

void render() {
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);
  render_snake();
  render_food();
  render_score();
  SDL_RenderPresent(renderer);
}

int main() {
  srand(time(NULL));
  SDL_Init(SDL_INIT_VIDEO);
  TTF_Init();

  window =
      SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  font = TTF_OpenFont("JetBrainsMonoNerdFontMono-Regular.ttf", 24);
  if (!font) {
    fprintf(stderr, "Error loading font: %s\n", TTF_GetError());
    return 1;
  }

  SDL_Event event;

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);
  render_message("Press Enter to start the game.");
  SDL_RenderPresent(renderer);

  bool waiting = true;
  while (waiting) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_RETURN) {
          waiting = false;
        } else if (event.key.keysym.sym == SDLK_q) {
          TTF_CloseFont(font);
          TTF_Quit();
          SDL_DestroyRenderer(renderer);
          SDL_DestroyWindow(window);
          SDL_Quit();
          return 0;
        }
      }
    }
  }

  do {
    running = true;
    init_snake();
    place_food();

    while (running) {
      while (SDL_PollEvent(&event)) {
        handle_input(&event);
      }
      move_snake();
      render();
      SDL_Delay(100);
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    render_message("Game Over! Press 'r' to retry or 'q' to quit.");
    SDL_RenderPresent(renderer);

    printf("Game over! Press 'r' to try again or 'q' to quit.\n");
    waiting = true;
    while (waiting) {
      while (SDL_PollEvent(&event)) {
        if (event.type == SDL_KEYDOWN) {
          if (event.key.keysym.sym == SDLK_r) {
            waiting = false;
          } else if (event.key.keysym.sym == SDLK_q) {
            TTF_CloseFont(font);
            TTF_Quit();
            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
            SDL_Quit();
            return 0;
          }
        }
      }
    }
  } while (true);

  TTF_CloseFont(font);
  TTF_Quit();
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
