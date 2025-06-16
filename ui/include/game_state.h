#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "tf.h"
#include "simple_bot.h"
#include <stdbool.h>
#include <SDL2/SDL.h>

// Game state wrapper
typedef struct {
    game_t* game;
    int current_player;
    int selected_card_index;
    bool game_over;
    char* winner;
    
    // Bot support
    bool player1_is_bot;
    bool player2_is_bot;
    SimpleBot* bot1;  // SimpleBot pointer for player 1
    SimpleBot* bot2;  // SimpleBot pointer for player 2
    
    // Bot timing control
    Uint32 last_bot_action_time;
    Uint32 bot_cooldown_ms;
    int bot_stuck_counter;  // Counter to detect when bots get stuck
    bool manual_bot_skip;   // Flag to manually skip bot turn
    
    // UI state
    bool show_discard_pile;
    int discard_player;
    bool show_epic_choice;
    bool shopping_mode;  // New: whether in shopping interface
    
    // Available characters
    char* available_characters[10];
    int num_characters;
} GameState;

// Function declarations
bool game_state_init(GameState* state, const char* player1, const char* player2);
bool game_state_init_with_bot(GameState* state, const char* player1, const char* player2, bool p1_bot, bool p2_bot, int bot_difficulty);
void game_state_cleanup(GameState* state);
void game_state_update(GameState* state);
bool game_state_handle_card_play(GameState* state, int card_index);
bool game_state_handle_choice(GameState* state, int choice_index);
bool game_state_handle_shop_purchase(GameState* state, int shop_index);
bool game_state_handle_focus(GameState* state);
bool game_state_handle_mulligan(GameState* state);  // NEW: Mulligan functionality
bool game_state_handle_mouse_event(GameState* state, SDL_Event* event);  // New mouse handler
bool game_state_skip_bot_turn(GameState* state);  // NEW: Manual bot skip
void game_state_render(GameState* state, void* renderer);
bool game_state_process_bot_turn(GameState* state);

// Character selection
void game_state_load_characters(GameState* state);
bool game_state_is_valid_character(GameState* state, const char* name);

#endif // GAME_STATE_H
