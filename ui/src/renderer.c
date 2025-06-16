#include "renderer.h"
#include <stdio.h>
#include <stdlib.h>

// Define colors
const Color COLOR_BLACK = {0, 0, 0, 255};
const Color COLOR_WHITE = {255, 255, 255, 255};
const Color COLOR_RED = {255, 0, 0, 255};
const Color COLOR_GREEN = {0, 255, 0, 255};
const Color COLOR_BLUE = {0, 0, 255, 255};
const Color COLOR_YELLOW = {255, 255, 0, 255};
const Color COLOR_GRAY = {128, 128, 128, 255};
const Color COLOR_DARK_GRAY = {64, 64, 64, 255};
const Color COLOR_CARD_BG = {245, 245, 220, 255};  // Beige for cards
const Color COLOR_HEALTH = {220, 20, 60, 255};     // Crimson for health
const Color COLOR_POWER = {30, 144, 255, 255};     // DodgerBlue for power

bool renderer_init(Renderer* renderer) {
    if (!renderer) return false;
    
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return false;
    }
    
    // Initialize SDL_ttf
    if (TTF_Init() == -1) {
        printf("SDL_ttf could not initialize! TTF_Error: %s\n", TTF_GetError());
        SDL_Quit();
        return false;
    }
    
    // Initialize SDL_image
    int img_flags = IMG_INIT_PNG;
    if (!(IMG_Init(img_flags) & img_flags)) {
        printf("SDL_image could not initialize! IMG_Error: %s\n", IMG_GetError());
        TTF_Quit();
        SDL_Quit();
        return false;
    }
    
    // Create window
    renderer->window = SDL_CreateWindow("Twisted Fables",
                                       SDL_WINDOWPOS_UNDEFINED,
                                       SDL_WINDOWPOS_UNDEFINED,
                                       SCREEN_WIDTH,
                                       SCREEN_HEIGHT,
                                       SDL_WINDOW_SHOWN);
    if (!renderer->window) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        IMG_Quit();
        TTF_Quit();
        SDL_Quit();
        return false;
    }
    
    // Create renderer
    renderer->renderer = SDL_CreateRenderer(renderer->window, -1, 
                                          SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer->renderer) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(renderer->window);
        IMG_Quit();
        TTF_Quit();
        SDL_Quit();
        return false;
    }
    
    // Load fonts (using system font for now, could be improved)
    renderer->font_small = TTF_OpenFont("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf", 14);
    renderer->font_medium = TTF_OpenFont("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf", 18);
    renderer->font_large = TTF_OpenFont("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf", 24);
    
    // If system fonts not available, try to create a simple font
    if (!renderer->font_small || !renderer->font_medium || !renderer->font_large) {
        printf("Warning: Could not load fonts. Text rendering may not work properly.\n");
    }
    
    // Initialize texture cache
    for (int i = 0; i < 300; i++) {
        renderer->card_textures[i] = NULL;
        renderer->card_textures_loaded[i] = false;
    }
    
    // Load common assets
    renderer->heart_icon = renderer_load_texture(renderer, "../picture/heart.png");
    renderer->defense_icon = renderer_load_texture(renderer, "../picture/defense.png");
    renderer->field_piece = renderer_load_texture(renderer, "../picture/fieldPiece.png");
    renderer->gate = renderer_load_texture(renderer, "../picture/gate.png");
    renderer->logo = renderer_load_texture(renderer, "tf_logo.jpeg");
    renderer->wallpaper = renderer_load_texture(renderer, "tf_wallpaper.jpg");
    
    renderer->initialized = true;
    return true;
}

void renderer_cleanup(Renderer* renderer) {
    if (!renderer) return;
    
    // Clean up fonts
    if (renderer->font_small) TTF_CloseFont(renderer->font_small);
    if (renderer->font_medium) TTF_CloseFont(renderer->font_medium);
    if (renderer->font_large) TTF_CloseFont(renderer->font_large);
    
    // Clean up card texture cache
    for (int i = 0; i < 300; i++) {
        if (renderer->card_textures[i]) {
            SDL_DestroyTexture(renderer->card_textures[i]);
        }
    }
    
    // Clean up common assets
    if (renderer->heart_icon) SDL_DestroyTexture(renderer->heart_icon);
    if (renderer->defense_icon) SDL_DestroyTexture(renderer->defense_icon);
    if (renderer->field_piece) SDL_DestroyTexture(renderer->field_piece);
    if (renderer->gate) SDL_DestroyTexture(renderer->gate);
    if (renderer->logo) SDL_DestroyTexture(renderer->logo);
    if (renderer->wallpaper) SDL_DestroyTexture(renderer->wallpaper);
    
    if (renderer->renderer) SDL_DestroyRenderer(renderer->renderer);
    if (renderer->window) SDL_DestroyWindow(renderer->window);
    
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
    
    renderer->initialized = false;
}

void renderer_clear(Renderer* renderer, Color color) {
    if (!renderer || !renderer->initialized) return;
    
    SDL_SetRenderDrawColor(renderer->renderer, color.r, color.g, color.b, color.a);
    SDL_RenderClear(renderer->renderer);
}

void renderer_present(Renderer* renderer) {
    if (!renderer || !renderer->initialized) return;
    SDL_RenderPresent(renderer->renderer);
}

void renderer_draw_rect(Renderer* renderer, Rect rect, Color color, bool filled) {
    if (!renderer || !renderer->initialized) return;
    
    SDL_SetRenderDrawColor(renderer->renderer, color.r, color.g, color.b, color.a);
    
    SDL_Rect sdl_rect = {rect.x, rect.y, rect.w, rect.h};
    
    if (filled) {
        SDL_RenderFillRect(renderer->renderer, &sdl_rect);
    } else {
        SDL_RenderDrawRect(renderer->renderer, &sdl_rect);
    }
}

void renderer_draw_text(Renderer* renderer, const char* text, int x, int y, Color color, TTF_Font* font) {
    if (!renderer || !renderer->initialized || !text || !font) return;
    
    SDL_Color sdl_color = {color.r, color.g, color.b, color.a};
    SDL_Surface* text_surface = TTF_RenderText_Solid(font, text, sdl_color);
    if (!text_surface) return;
    
    SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer->renderer, text_surface);
    if (!text_texture) {
        SDL_FreeSurface(text_surface);
        return;
    }
    
    SDL_Rect dest_rect = {x, y, text_surface->w, text_surface->h};
    SDL_RenderCopy(renderer->renderer, text_texture, NULL, &dest_rect);
    
    SDL_DestroyTexture(text_texture);
    SDL_FreeSurface(text_surface);
}

void renderer_draw_card(Renderer* renderer, Rect rect, const char* name, const char* type, int level, int cost) {
    if (!renderer || !renderer->initialized) return;
    
    // Draw card background
    renderer_draw_rect(renderer, rect, COLOR_WHITE, true);
    renderer_draw_rect(renderer, rect, COLOR_BLACK, false);
    
    // Draw card info
    if (name && renderer->font_small) {
        renderer_draw_text(renderer, name, rect.x + 5, rect.y + 5, COLOR_BLACK, renderer->font_small);
    }
    
    if (type && renderer->font_small) {
        renderer_draw_text(renderer, type, rect.x + 5, rect.y + 25, COLOR_BLUE, renderer->font_small);
    }
    
    // Draw level and cost
    char info[32];
    snprintf(info, sizeof(info), "Lv%d Cost:%d", level, cost);
    if (renderer->font_small) {
        renderer_draw_text(renderer, info, rect.x + 5, rect.y + rect.h - 20, COLOR_DARK_GRAY, renderer->font_small);
    }
}

SDL_Texture* renderer_load_texture(Renderer* renderer, const char* path) {
    if (!renderer || !renderer->initialized || !path) return NULL;
    
    SDL_Surface* surface = IMG_Load(path);
    if (!surface) {
        printf("Unable to load image %s! IMG_Error: %s\n", path, IMG_GetError());
        return NULL;
    }
    
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer->renderer, surface);
    if (!texture) {
        printf("Unable to create texture from %s! SDL_Error: %s\n", path, SDL_GetError());
    }
    
    SDL_FreeSurface(surface);
    return texture;
}

SDL_Texture* renderer_get_card_texture(Renderer* renderer, int card_number) {
    if (!renderer || !renderer->initialized || card_number < 1 || card_number >= 300) {
        return NULL;
    }
    
    // Check if already loaded
    if (renderer->card_textures_loaded[card_number]) {
        return renderer->card_textures[card_number];
    }
    
    // Load the texture
    char path[256];
    snprintf(path, sizeof(path), "../picture/cards/%d.png", card_number);
    
    renderer->card_textures[card_number] = renderer_load_texture(renderer, path);
    renderer->card_textures_loaded[card_number] = true;
    
    return renderer->card_textures[card_number];
}

void renderer_draw_texture(Renderer* renderer, SDL_Texture* texture, Rect dest_rect) {
    if (!renderer || !renderer->initialized || !texture) return;
    
    SDL_Rect sdl_rect = {dest_rect.x, dest_rect.y, dest_rect.w, dest_rect.h};
    SDL_RenderCopy(renderer->renderer, texture, NULL, &sdl_rect);
}

void renderer_draw_card_with_image(Renderer* renderer, Rect rect, int card_number, const char* name, int level, int cost, bool selected) {
    if (!renderer || !renderer->initialized) return;
    
    // Draw selection border if selected
    if (selected) {
        Rect border_rect = {rect.x - 3, rect.y - 3, rect.w + 6, rect.h + 6};
        renderer_draw_rect(renderer, border_rect, COLOR_YELLOW, false);
        renderer_draw_rect(renderer, (Rect){rect.x - 2, rect.y - 2, rect.w + 4, rect.h + 4}, COLOR_YELLOW, false);
    }
    
    // Try to draw card image
    SDL_Texture* card_texture = renderer_get_card_texture(renderer, card_number);
    if (card_texture) {
        renderer_draw_texture(renderer, card_texture, rect);
    } else {
        // Fallback to old card drawing style
        renderer_draw_rect(renderer, rect, COLOR_CARD_BG, true);
        renderer_draw_rect(renderer, rect, COLOR_BLACK, false);
    }
    
    // Draw overlay information on top of the card image
    if (name && renderer->font_small) {
        // Draw text with background for better readability
        Rect text_bg = {rect.x, rect.y + rect.h - 40, rect.w, 40};
        Color semi_black = {0, 0, 0, 180};
        renderer_draw_rect(renderer, text_bg, semi_black, true);
        
        renderer_draw_text(renderer, name, rect.x + 5, rect.y + rect.h - 35, COLOR_WHITE, renderer->font_small);
    }
    
    // Draw level and cost in top corners
    char level_str[16], cost_str[16];
    snprintf(level_str, sizeof(level_str), "Lv%d", level);
    snprintf(cost_str, sizeof(cost_str), "%d", cost);
    
    if (renderer->font_small) {
        // Level in top-left
        Rect level_bg = {rect.x, rect.y, 30, 20};
        renderer_draw_rect(renderer, level_bg, COLOR_GREEN, true);
        renderer_draw_text(renderer, level_str, rect.x + 2, rect.y + 2, COLOR_WHITE, renderer->font_small);
        
        // Cost in top-right
        Rect cost_bg = {rect.x + rect.w - 25, rect.y, 25, 20};
        renderer_draw_rect(renderer, cost_bg, COLOR_BLUE, true);
        renderer_draw_text(renderer, cost_str, rect.x + rect.w - 20, rect.y + 2, COLOR_WHITE, renderer->font_small);
    }
}

void renderer_draw_health_bar(Renderer* renderer, Rect rect, int current_hp, int max_hp) {
    if (!renderer || !renderer->initialized || max_hp <= 0) return;
    
    // Draw background
    renderer_draw_rect(renderer, rect, COLOR_DARK_GRAY, true);
    
    // Draw health bar
    float health_ratio = (float)current_hp / max_hp;
    if (health_ratio < 0) health_ratio = 0;
    if (health_ratio > 1) health_ratio = 1;
    
    Rect health_rect = {rect.x, rect.y, (int)(rect.w * health_ratio), rect.h};
    
    // Color based on health percentage
    Color health_color;
    if (health_ratio > 0.6f) {
        health_color = COLOR_GREEN;
    } else if (health_ratio > 0.3f) {
        health_color = COLOR_YELLOW;
    } else {
        health_color = COLOR_HEALTH; // Red
    }
    
    renderer_draw_rect(renderer, health_rect, health_color, true);
    
    // Draw border
    renderer_draw_rect(renderer, rect, COLOR_BLACK, false);
    
    // Draw heart icon if available
    if (renderer->heart_icon) {
        Rect icon_rect = {rect.x - 25, rect.y, 20, 20};
        renderer_draw_texture(renderer, renderer->heart_icon, icon_rect);
    }
    
    // Draw health text
    char health_text[32];
    snprintf(health_text, sizeof(health_text), "%d/%d", current_hp, max_hp);
    if (renderer->font_small) {
        int text_w, text_h;
        renderer_text_size(renderer->font_small, health_text, &text_w, &text_h);
        int text_x = rect.x + (rect.w - text_w) / 2;
        int text_y = rect.y + (rect.h - text_h) / 2;
        renderer_draw_text(renderer, health_text, text_x, text_y, COLOR_WHITE, renderer->font_small);
    }
}

void renderer_draw_power_indicator(Renderer* renderer, int x, int y, int current_power, int max_power) {
    if (!renderer || !renderer->initialized) return;
    
    // Draw power orbs/gems
    int orb_size = 20;
    int orb_spacing = 25;
    
    for (int i = 0; i < max_power; i++) {
        Rect orb_rect = {x + i * orb_spacing, y, orb_size, orb_size};
        
        if (i < current_power) {
            // Filled power orb
            renderer_draw_rect(renderer, orb_rect, COLOR_POWER, true);
        } else {
            // Empty power orb
            renderer_draw_rect(renderer, orb_rect, COLOR_GRAY, true);
        }
        
        renderer_draw_rect(renderer, orb_rect, COLOR_BLACK, false);
    }
    
    // Draw power text
    char power_text[32];
    snprintf(power_text, sizeof(power_text), "Power: %d/%d", current_power, max_power);
    if (renderer->font_small) {
        renderer_draw_text(renderer, power_text, x, y + orb_size + 5, COLOR_WHITE, renderer->font_small);
    }
}

void renderer_text_size(TTF_Font* font, const char* text, int* w, int* h) {
    if (!font || !text) {
        if (w) *w = 0;
        if (h) *h = 0;
        return;
    }
    
    TTF_SizeText(font, text, w, h);
}

void renderer_draw_background_wallpaper(Renderer* renderer) {
    if (!renderer || !renderer->initialized) return;
    
    // First clear with dark background as fallback
    renderer_clear(renderer, COLOR_DARK_GRAY);
    
    // Draw wallpaper if available, scaled to fit screen
    if (renderer->wallpaper) {
        Rect wallpaper_rect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        renderer_draw_texture(renderer, renderer->wallpaper, wallpaper_rect);
        
        // Add subtle overlay to ensure text remains readable
        Color overlay = {0, 0, 0, 60}; // Semi-transparent black
        renderer_draw_rect(renderer, wallpaper_rect, overlay, true);
    }
}
