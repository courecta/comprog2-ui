/**
 * Menu System Implementation - GUI Start Screen and Menu Management
 */

#include "menu_system.h"
#include "renderer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

// Menu configuration constants
#define MENU_ITEM_HEIGHT 60
#define MENU_ITEM_SPACING 20
#define MENU_START_Y 200

// Colors for menu rendering
static const SDL_Color COLOR_MENU_BG = {20, 30, 50, 255};
static const SDL_Color COLOR_MENU_TEXT = {220, 220, 220, 255};
static const SDL_Color COLOR_MENU_HIGHLIGHT = {100, 150, 255, 255};
static const SDL_Color COLOR_MENU_DISABLED = {100, 100, 100, 255};

/**
 * Format character names: replace underscores with spaces and capitalize words
 */
static void format_character_name(const char* input, char* output, size_t output_size) {
    if (!input || !output || output_size == 0) return;
    
    size_t len = strlen(input);
    if (len >= output_size) len = output_size - 1;
    
    bool capitalize_next = true;
    size_t j = 0;
    
    for (size_t i = 0; i < len && j < output_size - 1; i++) {
        if (input[i] == '_') {
            output[j++] = ' ';
            capitalize_next = true;
        } else {
            if (capitalize_next && input[i] >= 'a' && input[i] <= 'z') {
                output[j++] = input[i] - 'a' + 'A';
                capitalize_next = false;
            } else if (!capitalize_next && input[i] >= 'A' && input[i] <= 'Z') {
                output[j++] = input[i] - 'A' + 'a';
            } else {
                output[j++] = input[i];
                if (input[i] != ' ') capitalize_next = false;
            }
        }
    }
    output[j] = '\0';
}

/**
 * Initialize menu system
 */
bool menu_system_init(MenuSystem* menu) {
    if (!menu) return false;
    
    memset(menu, 0, sizeof(MenuSystem));
    
    menu->current_state = MENU_START_SCREEN;
    menu->previous_state = MENU_START_SCREEN;
    
    // Initialize start screen items - simplified to 3 items only
    menu->start_item_count = 3;
    menu->start_selected_index = 0;
    
    // Setup start screen menu items
    const char* start_texts[] = {"1v1 MODE", "BOT MODE", "EXIT"};
    for (int i = 0; i < menu->start_item_count; i++) {
        menu->start_items[i].text = start_texts[i];
        menu->start_items[i].bounds.x = SCREEN_WIDTH / 2 - 150;
        menu->start_items[i].bounds.y = MENU_START_Y + i * (MENU_ITEM_HEIGHT + MENU_ITEM_SPACING);
        menu->start_items[i].bounds.w = 300;
        menu->start_items[i].bounds.h = MENU_ITEM_HEIGHT;
        menu->start_items[i].highlighted = (i == 0);
        menu->start_items[i].enabled = true;
    }
    
    // Remove bot difficulty menu - no longer needed
    menu->mode_item_count = 0;
    menu->mode_selected_index = 0;
    
    // Initialize character list from actual files
    menu->character_count = 0;
    
    // Load available characters by checking filesystem
    const char* test_characters[] = {
        "red_riding_hood", "snow_white", "match_girl", 
        "sleeping_beauty", "kaguya"
    };
    
    for (int i = 0; i < 5 && menu->character_count < 10; i++) {
        // Test if character file exists by attempting to create a temporary game
        char path[256];
        snprintf(path, sizeof(path), "characters/%s.dat", test_characters[i]);
        
        FILE* test = fopen(path, "r");
        if (test) {
            fclose(test);
            menu->available_characters[menu->character_count] = malloc(strlen(test_characters[i]) + 1);
            if (menu->available_characters[menu->character_count]) {
                strcpy(menu->available_characters[menu->character_count], test_characters[i]);
                menu->character_count++;
            }
        }
    }
    
    // Setup character selection items
    for (int i = 0; i < menu->character_count; i++) {
        menu->character_items[i].text = menu->available_characters[i];
        menu->character_items[i].bounds.x = SCREEN_WIDTH / 2 - 150;
        menu->character_items[i].bounds.y = MENU_START_Y + i * (MENU_ITEM_HEIGHT + MENU_ITEM_SPACING);
        menu->character_items[i].bounds.w = 300;
        menu->character_items[i].bounds.h = MENU_ITEM_HEIGHT;
        menu->character_items[i].highlighted = (i == 0);
        menu->character_items[i].enabled = true;
    }
    
    // Default selections
    menu->player1_character_index = -1; // Not selected yet
    menu->player2_character_index = -1; // Not selected yet
    menu->character_selected_index = 0;
    menu->player1_selected = false;
    menu->selected_game_mode = GAME_MODE_1V1;
    menu->p1_is_bot = false;
    menu->p2_is_bot = false;
    menu->bot_difficulty = 1; // Single bot difficulty
    
    // Animation
    menu->last_update_time = SDL_GetTicks();
    menu->highlight_alpha = 0.0f;
    menu->fade_in = true;
    
    return true;
}

/**
 * Cleanup menu system
 */
void menu_system_cleanup(MenuSystem* menu) {
    if (!menu) return;
    
    for (int i = 0; i < menu->character_count; i++) {
        if (menu->available_characters[i]) {
            free(menu->available_characters[i]);
            menu->available_characters[i] = NULL;
        }
    }
}

/**
 * Handle navigation input
 */
static void handle_menu_navigation(MenuSystem* menu, SDL_Keycode key) {
    if (menu->current_state == MENU_START_SCREEN) {
        // Update highlighting
        for (int i = 0; i < menu->start_item_count; i++) {
            menu->start_items[i].highlighted = false;
        }
        
        if (key == SDLK_UP || key == SDLK_w) {
            menu->start_selected_index = (menu->start_selected_index - 1 + menu->start_item_count) % menu->start_item_count;
        } else if (key == SDLK_DOWN || key == SDLK_s) {
            menu->start_selected_index = (menu->start_selected_index + 1) % menu->start_item_count;
        }
        
        menu->start_items[menu->start_selected_index].highlighted = true;
    }
    else if (menu->current_state == MENU_CHARACTER_SELECT) {
        // Update character highlighting
        for (int i = 0; i < menu->character_count; i++) {
            menu->character_items[i].highlighted = false;
        }
        
        if (key == SDLK_UP || key == SDLK_w) {
            menu->character_selected_index = (menu->character_selected_index - 1 + menu->character_count) % menu->character_count;
        } else if (key == SDLK_DOWN || key == SDLK_s) {
            menu->character_selected_index = (menu->character_selected_index + 1) % menu->character_count;
        }
        
        menu->character_items[menu->character_selected_index].highlighted = true;
    }
    // Remove bot mode selection navigation
}

/**
 * Handle menu selection
 */
static void handle_menu_selection(MenuSystem* menu) {
    if (menu->current_state == MENU_START_SCREEN) {
        switch (menu->start_selected_index) {
            case 0: // 1v1 MODE
                menu->selected_game_mode = GAME_MODE_1V1;
                menu->p1_is_bot = false;
                menu->p2_is_bot = false;
                menu->current_state = MENU_CHARACTER_SELECT;
                menu->player1_selected = false;
                menu->player1_character_index = -1;
                menu->player2_character_index = -1;
                break;
                
            case 1: // BOT MODE - Direct to character select
                menu->selected_game_mode = GAME_MODE_BOT_MEDIUM;
                menu->p1_is_bot = false;
                menu->p2_is_bot = true;
                menu->bot_difficulty = 1;
                menu->current_state = MENU_CHARACTER_SELECT;
                menu->player1_selected = false;
                menu->player1_character_index = -1;
                menu->player2_character_index = -1;
                break;
                
            case 2: // EXIT
                menu->current_state = MENU_EXIT;
                break;
        }
    }
    else if (menu->current_state == MENU_CHARACTER_SELECT) {
        if (!menu->player1_selected) {
            // Player 1 selecting
            menu->player1_character_index = menu->character_selected_index;
            menu->player1_selected = true;
            
            if (menu->p2_is_bot) {
                // Bot mode: randomly select character for bot (excluding P1's choice)
                do {
                    menu->player2_character_index = rand() % menu->character_count;
                } while (menu->player2_character_index == menu->player1_character_index);
                
                menu->current_state = MENU_IN_GAME;
            } else {
                // 1v1 mode: wait for P2 selection
                // Update available characters display
                for (int i = 0; i < menu->character_count; i++) {
                    menu->character_items[i].enabled = (i != menu->player1_character_index);
                }
            }
        } else {
            // Player 2 selecting (1v1 mode only)
            if (menu->character_selected_index != menu->player1_character_index) {
                menu->player2_character_index = menu->character_selected_index;
                menu->current_state = MENU_IN_GAME;
            }
        }
    }
    // Remove bot difficulty selection entirely
}

/**
 * Handle SDL events for menu
 */
void menu_system_handle_event(MenuSystem* menu, SDL_Event* event) {
    if (!menu || !event) return;
    
    if (event->type == SDL_KEYDOWN) {
        SDL_Keycode key = event->key.keysym.sym;
        
        // Navigation
        if (key == SDLK_UP || key == SDLK_DOWN || key == SDLK_w || key == SDLK_s) {
            handle_menu_navigation(menu, key);
        }
        // Selection
        else if (key == SDLK_RETURN || key == SDLK_SPACE) {
            handle_menu_selection(menu);
        }
        // Back/Cancel
        else if (key == SDLK_ESCAPE) {
            if (menu->current_state == MENU_START_SCREEN) {
                menu->current_state = MENU_EXIT;
            } else if (menu->current_state == MENU_CHARACTER_SELECT) {
                if (menu->player1_selected && !menu->p2_is_bot) {
                    // In 1v1 mode, P2 is selecting - go back to P1 selection
                    menu->player1_selected = false;
                    menu->player1_character_index = -1;
                    for (int i = 0; i < menu->character_count; i++) {
                        menu->character_items[i].enabled = true;
                    }
                } else {
                    // Go back to main menu
                    menu->current_state = MENU_START_SCREEN;
                    menu->player1_selected = false;
                    menu->player1_character_index = -1;
                    menu->player2_character_index = -1;
                }
            }
        }
    }
    
    // Mouse input (basic support)
    if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT) {
        int mouse_x = event->button.x;
        int mouse_y = event->button.y;
        
        if (menu->current_state == MENU_START_SCREEN) {
            for (int i = 0; i < menu->start_item_count; i++) {
                SDL_Rect* bounds = &menu->start_items[i].bounds;
                if (mouse_x >= bounds->x && mouse_x < bounds->x + bounds->w &&
                    mouse_y >= bounds->y && mouse_y < bounds->y + bounds->h) {
                    menu->start_selected_index = i;
                    handle_menu_selection(menu);
                    break;
                }
            }
        }
        else if (menu->current_state == MENU_CHARACTER_SELECT) {
            for (int i = 0; i < menu->character_count; i++) {
                SDL_Rect* bounds = &menu->character_items[i].bounds;
                if (mouse_x >= bounds->x && mouse_x < bounds->x + bounds->w &&
                    mouse_y >= bounds->y && mouse_y < bounds->y + bounds->h &&
                    menu->character_items[i].enabled) {
                    menu->character_selected_index = i;
                    handle_menu_selection(menu);
                    break;
                }
            }
        }
    }
    
    // Mouse motion (hover support)
    if (event->type == SDL_MOUSEMOTION) {
        int mouse_x = event->motion.x;
        int mouse_y = event->motion.y;
        
        if (menu->current_state == MENU_START_SCREEN) {
            // Reset all highlights first
            for (int i = 0; i < menu->start_item_count; i++) {
                menu->start_items[i].highlighted = false;
            }
            // Check for hover
            for (int i = 0; i < menu->start_item_count; i++) {
                SDL_Rect* bounds = &menu->start_items[i].bounds;
                if (mouse_x >= bounds->x && mouse_x < bounds->x + bounds->w &&
                    mouse_y >= bounds->y && mouse_y < bounds->y + bounds->h) {
                    menu->start_items[i].highlighted = true;
                    menu->start_selected_index = i;
                    break;
                }
            }
        }
        else if (menu->current_state == MENU_CHARACTER_SELECT) {
            // Reset all highlights first
            for (int i = 0; i < menu->character_count; i++) {
                menu->character_items[i].highlighted = false;
            }
            // Check for hover
            for (int i = 0; i < menu->character_count; i++) {
                SDL_Rect* bounds = &menu->character_items[i].bounds;
                if (mouse_x >= bounds->x && mouse_x < bounds->x + bounds->w &&
                    mouse_y >= bounds->y && mouse_y < bounds->y + bounds->h &&
                    menu->character_items[i].enabled) {
                    menu->character_items[i].highlighted = true;
                    menu->character_selected_index = i;
                    break;
                }
            }
        }
    }
}

/**
 * Update menu state and animations
 */
void menu_system_update(MenuSystem* menu, Uint32 current_time) {
    if (!menu) return;
    
    // Simple pulsing highlight animation
    float delta_time = (current_time - menu->last_update_time) / 1000.0f;
    menu->last_update_time = current_time;
    
    if (menu->fade_in) {
        menu->highlight_alpha += delta_time * 2.0f;
        if (menu->highlight_alpha >= 1.0f) {
            menu->highlight_alpha = 1.0f;
            menu->fade_in = false;
        }
    } else {
        menu->highlight_alpha -= delta_time * 2.0f;
        if (menu->highlight_alpha <= 0.3f) {
            menu->highlight_alpha = 0.3f;
            menu->fade_in = true;
        }
    }
}

/**
 * Render menu text with background
 */
static void render_menu_item(Renderer* renderer, MenuItem* item, float alpha_multiplier) {
    if (!renderer || !item) return;
    
    SDL_Renderer* sdl_renderer = renderer->renderer;
    
    // Background rectangle
    SDL_Color bg_color = COLOR_MENU_HIGHLIGHT;
    if (item->highlighted) {
        bg_color.a = (Uint8)(255 * alpha_multiplier * 0.7f);
        SDL_SetRenderDrawColor(sdl_renderer, bg_color.r, bg_color.g, bg_color.b, bg_color.a);
        SDL_RenderFillRect(sdl_renderer, &item->bounds);
    }
    
    // Border
    SDL_Color border_color = item->highlighted ? COLOR_MENU_HIGHLIGHT : COLOR_MENU_TEXT;
    border_color.a = (Uint8)(255 * alpha_multiplier);
    SDL_SetRenderDrawColor(sdl_renderer, border_color.r, border_color.g, border_color.b, border_color.a);
    SDL_RenderDrawRect(sdl_renderer, &item->bounds);
    
    // TEXT RENDERING - FIXED WITH CHARACTER NAME FORMATTING!
    if (item->text && renderer->font_large) {
        // Format character names: replace underscores with spaces and capitalize
        char formatted_name[128];
        format_character_name(item->text, formatted_name, sizeof(formatted_name));
        
        // Calculate centered position
        int text_w, text_h;
        TTF_SizeText(renderer->font_large, formatted_name, &text_w, &text_h);
        
        int text_x = item->bounds.x + (item->bounds.w - text_w) / 2;
        int text_y = item->bounds.y + (item->bounds.h - text_h) / 2;
        
        // Use the renderer's draw text function
        Color text_color = {COLOR_MENU_TEXT.r, COLOR_MENU_TEXT.g, COLOR_MENU_TEXT.b, (Uint8)(255 * alpha_multiplier)};
        renderer_draw_text(renderer, formatted_name, text_x, text_y, text_color, renderer->font_large);
    }
}

/**
 * Render current menu screen
 */
void menu_system_render(MenuSystem* menu, Renderer* renderer) {
    if (!menu || !renderer) return;
    
    // Clear with wallpaper background
    renderer_draw_background_wallpaper(renderer);
    
    // Title area with logo and text
    SDL_Rect title_rect = {SCREEN_WIDTH / 2 - 200, 50, 400, 80};
    SDL_SetRenderDrawColor(renderer->renderer, 60, 80, 120, 200); // Semi-transparent
    SDL_RenderFillRect(renderer->renderer, &title_rect);
    SDL_SetRenderDrawColor(renderer->renderer, COLOR_MENU_TEXT.r, COLOR_MENU_TEXT.g, COLOR_MENU_TEXT.b, 255);
    SDL_RenderDrawRect(renderer->renderer, &title_rect);
    
    // Draw logo if available (left side of title area)
    if (renderer->logo) {
        Rect logo_rect = {title_rect.x + 10, title_rect.y + 10, 60, 60}; // Smaller logo for menu
        renderer_draw_texture(renderer, renderer->logo, logo_rect);
    }
    
    // Draw the "TWISTED FABLES" title text
    if (renderer->font_large) {
        const char* title_text = "TWISTED FABLES";
        int text_w, text_h;
        TTF_SizeText(renderer->font_large, title_text, &text_w, &text_h);
        
        int title_x = renderer->logo ? title_rect.x + 80 : title_rect.x + (title_rect.w - text_w) / 2; // Offset if logo present
        int title_y = title_rect.y + (title_rect.h - text_h) / 2;
        
        Color title_color = {255, 255, 255, 255}; // White text
        renderer_draw_text(renderer, title_text, title_x, title_y, title_color, renderer->font_large);
    }
    
    // Only render start screen - no bot difficulty menu
    if (menu->current_state == MENU_START_SCREEN) {
        // Render start screen items
        for (int i = 0; i < menu->start_item_count; i++) {
            float alpha = (menu->start_items[i].highlighted) ? menu->highlight_alpha : 0.5f;
            render_menu_item(renderer, &menu->start_items[i], alpha);
        }
        
        // Instructions
        SDL_Rect instr_rect = {50, SCREEN_HEIGHT - 100, SCREEN_WIDTH - 100, 80};
        SDL_SetRenderDrawColor(renderer->renderer, 40, 50, 70, 200);
        SDL_RenderFillRect(renderer->renderer, &instr_rect);
        SDL_SetRenderDrawColor(renderer->renderer, COLOR_MENU_TEXT.r, COLOR_MENU_TEXT.g, COLOR_MENU_TEXT.b, 255);
        SDL_RenderDrawRect(renderer->renderer, &instr_rect);
        
        // Add menu instructions text
        if (renderer->font_small) {
            const char* instructions = "Use ARROW KEYS to navigate, ENTER to select, ESC to quit";
            renderer_draw_text(renderer, instructions, instr_rect.x + 10, instr_rect.y + 10, 
                             (Color){255, 255, 255, 255}, renderer->font_small);
        }
    }
    else if (menu->current_state == MENU_CHARACTER_SELECT) {
        // Character selection screen
        const char* header_text = menu->player1_selected && !menu->p2_is_bot ? "Player 2: Choose Character" : "Player 1: Choose Character";
        
        if (renderer->font_large) {
            int text_w, text_h;
            TTF_SizeText(renderer->font_large, header_text, &text_w, &text_h);
            int header_x = (SCREEN_WIDTH - text_w) / 2;
            int header_y = 150;
            
            Color header_color = {255, 255, 200, 255};
            renderer_draw_text(renderer, header_text, header_x, header_y, header_color, renderer->font_large);
        }
        
        // Render character selection items
        for (int i = 0; i < menu->character_count; i++) {
            float alpha = 0.5f;
            if (menu->character_items[i].highlighted) {
                alpha = menu->highlight_alpha;
            }
            if (!menu->character_items[i].enabled) {
                alpha = 0.2f; // Dimmed for unavailable characters
            }
            render_menu_item(renderer, &menu->character_items[i], alpha);
        }
        
        // Show selected characters with formatted names
        if (menu->player1_character_index >= 0) {
            char p1_text[128];
            char formatted_p1_name[64];
            format_character_name(menu->available_characters[menu->player1_character_index], formatted_p1_name, sizeof(formatted_p1_name));
            snprintf(p1_text, sizeof(p1_text), "Player 1: %s", formatted_p1_name);
            
            if (renderer->font_medium) {
                renderer_draw_text(renderer, p1_text, 50, SCREEN_HEIGHT - 120, 
                                 (Color){200, 255, 200, 255}, renderer->font_medium);
            }
        }
        
        if (menu->player2_character_index >= 0) {
            char p2_text[128];
            char formatted_p2_name[64];
            format_character_name(menu->available_characters[menu->player2_character_index], formatted_p2_name, sizeof(formatted_p2_name));
            const char* p2_name = menu->p2_is_bot ? "Bot" : "Player 2";
            snprintf(p2_text, sizeof(p2_text), "%s: %s", p2_name, formatted_p2_name);
            
            if (renderer->font_medium) {
                renderer_draw_text(renderer, p2_text, 50, SCREEN_HEIGHT - 90, 
                                 (Color){200, 200, 255, 255}, renderer->font_medium);
            }
        }
        
        // Instructions
        SDL_Rect instr_rect = {50, SCREEN_HEIGHT - 60, SCREEN_WIDTH - 100, 50};
        SDL_SetRenderDrawColor(renderer->renderer, 40, 50, 70, 200);
        SDL_RenderFillRect(renderer->renderer, &instr_rect);
        
        if (renderer->font_small) {
            const char* instructions = "Use ARROW KEYS to navigate, ENTER to select, ESC to go back";
            renderer_draw_text(renderer, instructions, instr_rect.x + 10, instr_rect.y + 10, 
                             (Color){255, 255, 255, 255}, renderer->font_small);
        }
    }
}

/**
 * Check if menu wants to start game
 */
bool menu_system_should_start_game(MenuSystem* menu) {
    return menu && menu->current_state == MENU_IN_GAME;
}

/**
 * Check if menu wants to exit
 */
bool menu_system_should_exit(MenuSystem* menu) {
    return menu && menu->current_state == MENU_EXIT;
}

/**
 * Get game configuration from menu selections
 */
void menu_system_get_game_config(MenuSystem* menu, bool* p1_bot, bool* p2_bot, 
                                int* bot_difficulty, char** p1_char, char** p2_char) {
    if (!menu) return;
    
    if (p1_bot) *p1_bot = menu->p1_is_bot;
    if (p2_bot) *p2_bot = menu->p2_is_bot;
    if (bot_difficulty) *bot_difficulty = menu->bot_difficulty;
    
    if (p1_char && menu->player1_character_index >= 0 && menu->player1_character_index < menu->character_count) {
        *p1_char = menu->available_characters[menu->player1_character_index];
    }
    if (p2_char && menu->player2_character_index >= 0 && menu->player2_character_index < menu->character_count) {
        *p2_char = menu->available_characters[menu->player2_character_index];
    }
}

/**
 * Reset menu to start screen
 */
void menu_system_reset(MenuSystem* menu) {
    if (!menu) return;
    
    menu->current_state = MENU_START_SCREEN;
    menu->start_selected_index = 0;
    menu->mode_selected_index = 0;
    menu->player1_character_index = -1;
    menu->player2_character_index = -1;
    menu->character_selected_index = 0;
    menu->player1_selected = false;
    
    // Reset highlighting
    for (int i = 0; i < menu->start_item_count; i++) {
        menu->start_items[i].highlighted = (i == 0);
    }
    for (int i = 0; i < menu->mode_item_count; i++) {
        menu->mode_items[i].highlighted = (i == 0);
    }
    for (int i = 0; i < menu->character_count; i++) {
        menu->character_items[i].highlighted = (i == 0);
        menu->character_items[i].enabled = true;
    }
}
