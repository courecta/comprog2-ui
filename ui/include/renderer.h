#ifndef RENDERER_H
#define RENDERER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>

// Screen dimensions
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 800

// Colors (RGBA)
typedef struct {
    Uint8 r, g, b, a;
} Color;

// Common colors
extern const Color COLOR_BLACK;
extern const Color COLOR_WHITE;
extern const Color COLOR_RED;
extern const Color COLOR_GREEN;
extern const Color COLOR_BLUE;
extern const Color COLOR_YELLOW;
extern const Color COLOR_GRAY;
extern const Color COLOR_DARK_GRAY;
extern const Color COLOR_CARD_BG;
extern const Color COLOR_HEALTH;
extern const Color COLOR_POWER;

// Rectangle structure
typedef struct {
    int x, y, w, h;
} Rect;

// Renderer structure
typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* font_small;
    TTF_Font* font_medium;
    TTF_Font* font_large;
    bool initialized;
    
    // Asset textures
    SDL_Texture* heart_icon;
    SDL_Texture* defense_icon;
    SDL_Texture* field_piece;
    SDL_Texture* gate;
    SDL_Texture* logo;
    SDL_Texture* wallpaper;
    
    // Card textures cache (for performance)
    SDL_Texture* card_textures[300]; // Support up to 300 different cards
    bool card_textures_loaded[300];
} Renderer;

// Function declarations
bool renderer_init(Renderer* renderer);
void renderer_cleanup(Renderer* renderer);
void renderer_clear(Renderer* renderer, Color color);
void renderer_present(Renderer* renderer);

// Drawing functions
void renderer_draw_rect(Renderer* renderer, Rect rect, Color color, bool filled);
void renderer_draw_text(Renderer* renderer, const char* text, int x, int y, Color color, TTF_Font* font);
void renderer_draw_card(Renderer* renderer, Rect rect, const char* name, const char* type, int level, int cost);
void renderer_draw_card_with_image(Renderer* renderer, Rect rect, int card_number, const char* name, int level, int cost, bool selected);
void renderer_draw_texture(Renderer* renderer, SDL_Texture* texture, Rect dest_rect);
void renderer_draw_background_wallpaper(Renderer* renderer);
void renderer_draw_health_bar(Renderer* renderer, Rect rect, int current_hp, int max_hp);
void renderer_draw_power_indicator(Renderer* renderer, int x, int y, int current_power, int max_power);

// Asset loading
SDL_Texture* renderer_load_texture(Renderer* renderer, const char* path);
SDL_Texture* renderer_get_card_texture(Renderer* renderer, int card_number);

// Text measurement
void renderer_text_size(TTF_Font* font, const char* text, int* w, int* h);

#endif // RENDERER_H
