/**
 * Backend Logic Module Header
 * 
 * Provides clean interface between tf.h game engine and SDL2 GUI layer.
 * Extracts and organizes useful functions from backup files.
 */

#ifndef BACKEND_LOGIC_H
#define BACKEND_LOGIC_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

// Forward declarations - compatible with tf.h
struct _game_t;
struct _card_t;

// Constants
#define MAX_CHOICES 10
#define MAX_CHOICE_LENGTH 256

// Action types that can be taken in game advancement
typedef enum {
    ACTION_NEXT_PHASE,
    ACTION_CHOICE_AVAILABLE,
    ACTION_PLAYER_TURN,
    ACTION_GAME_OVER,
    ACTION_ERROR
} GameActionType;

// Result structure for game advancement operations
typedef struct {
    GameActionType action_taken;
    bool success;
    const char* message;
    int32_t choice_count;
} GameAdvanceResult;

// Game state information structure
typedef struct {
    int32_t turn;
    int32_t phase;
    int32_t activation;
    bool is_game_over;
    int32_t choice_count;
    char choices[MAX_CHOICES][MAX_CHOICE_LENGTH];
} GameStateInfo;

// Core functions extracted from backup files

/**
 * Print card information to buffer (from fixed_main.c, working_main.c)
 */
void backend_print_card(struct _card_t* card, char* buffer, size_t buffer_size);

/**
 * Print comprehensive player state to buffer (from backup files)
 */
void backend_print_player(struct _game_t* game, int32_t player_id, char* buffer, size_t buffer_size);

/**
 * Get structured game state information (from headless_main.c)
 */
void backend_get_game_state(struct _game_t* game, GameStateInfo* state_info);

/**
 * Check if card can be played safely (from working_main.c strategy)
 */
bool backend_can_play_card_safely(struct _card_t* card);

/**
 * Advance game state with proper error handling (from fixed_main.c pattern)
 */
GameAdvanceResult backend_advance_game(struct _game_t* game);

/**
 * Initialize game with proper setup phase advancement (from backup patterns)
 */
struct _game_t* backend_init_game(const char* player1_character, const char* player2_character);

/**
 * Get number of cards in player's hand
 */
int32_t backend_get_hand_count(struct _game_t* game, int32_t player_id);

/**
 * Get specific card from hand by index
 */
struct _card_t* backend_get_hand_card(struct _game_t* game, int32_t player_id, int32_t card_index);

/**
 * Debug function to print comprehensive game state
 */
void backend_debug_game_state(struct _game_t* game);

#endif // BACKEND_LOGIC_H
