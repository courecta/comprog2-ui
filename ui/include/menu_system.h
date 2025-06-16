/**
 * Menu System - GUI Start Screen and Menu Management
 * 
 * Replaces terminal input with proper SDL2-based menu system
 */

#ifndef MENU_SYSTEM_H
#define MENU_SYSTEM_H

#include <SDL2/SDL.h>
#include <stdbool.h>
#include "renderer.h"  // Include renderer for access to Renderer struct

// Menu states
typedef enum {
    MENU_START_SCREEN,
    MENU_GAME_MODE_SELECT,
    MENU_CHARACTER_SELECT,
    MENU_IN_GAME,
    MENU_EXIT
} MenuState;

// Game mode options
typedef enum {
    GAME_MODE_1V1 = 0,
    GAME_MODE_BOT_EASY,
    GAME_MODE_BOT_MEDIUM,
    GAME_MODE_BOT_HARD,
    GAME_MODE_EXIT,
    GAME_MODE_COUNT
} GameMode;

// Menu item structure
typedef struct {
    const char* text;
    SDL_Rect bounds;
    bool highlighted;
    bool enabled;
} MenuItem;

// Menu system structure
typedef struct {
    MenuState current_state;
    MenuState previous_state;
    
    // Start screen items
    MenuItem start_items[4];  // "1v1", "Bot Mode", "Exit"
    int start_item_count;
    int start_selected_index;
    
    // Game mode items (for bot difficulty)
    MenuItem mode_items[5];   // Easy, Medium, Hard, Back, Exit
    int mode_item_count;
    int mode_selected_index;
    
    // Character selection
    char* available_characters[10];
    int character_count;
    int player1_character_index;
    int player2_character_index;
    int character_selected_index;
    bool player1_selected;
    MenuItem character_items[10];
    
    // Selected game configuration
    GameMode selected_game_mode;
    bool p1_is_bot;
    bool p2_is_bot;
    int bot_difficulty;
    
    // Animation and timing
    Uint32 last_update_time;
    float highlight_alpha;
    bool fade_in;
} MenuSystem;

// Function declarations

/**
 * Initialize menu system
 */
bool menu_system_init(MenuSystem* menu);

/**
 * Cleanup menu system
 */
void menu_system_cleanup(MenuSystem* menu);

/**
 * Handle SDL events for menu
 */
void menu_system_handle_event(MenuSystem* menu, SDL_Event* event);

/**
 * Update menu state and animations
 */
void menu_system_update(MenuSystem* menu, Uint32 current_time);

/**
 * Render current menu screen
 */
void menu_system_render(MenuSystem* menu, Renderer* renderer);

/**
 * Check if menu wants to start game
 */
bool menu_system_should_start_game(MenuSystem* menu);

/**
 * Check if menu wants to exit
 */
bool menu_system_should_exit(MenuSystem* menu);

/**
 * Get game configuration from menu selections
 */
void menu_system_get_game_config(MenuSystem* menu, bool* p1_bot, bool* p2_bot, 
                                int* bot_difficulty, char** p1_char, char** p2_char);

/**
 * Reset menu to start screen
 */
void menu_system_reset(MenuSystem* menu);

#endif // MENU_SYSTEM_H
