/**
 * Core Game Logic Module Header - Pure Logic Without UI Dependencies
 * 
 * This module provides a clean interface to the tf.h game engine without
 * any UI dependencies, returning structured data instead of printing directly.
 * Based on groupmate's improved main.c with enhanced shopping and card handling.
 */

#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include "tf.h"
#include <stdbool.h>
#include <stdint.h>

// Forward declarations
typedef struct GameLogicState GameLogicState;

// Game action types
typedef enum {
    GAME_ACTION_NONE = 0,
    GAME_ACTION_CONTINUE,      // Press any key to continue (non-activation phase)
    GAME_ACTION_CHOOSE,        // Make a choice from available options
    GAME_ACTION_SHOP,          // Shopping mode active
    GAME_ACTION_PLAY_CARD,     // Can play cards or use actions
    GAME_ACTION_GAME_OVER      // Game has ended
} GameActionType;

// Input processing results
typedef enum {
    INPUT_RESULT_CONTINUE = 0,
    INPUT_RESULT_SUCCESS,
    INPUT_RESULT_INVALID,
    INPUT_RESULT_EXIT
} InputResult;

// Game state information structure
typedef struct GameLogicState {
    GameActionType action_type;
    int32_t current_turn;
    bool is_activation_phase;
    bool is_shopping_mode;
    int32_t hand_count;
    int32_t choice_count;
    
    // Player information
    struct {
        const char* name;
        int32_t hp, hp_max;
        int32_t def, def_max;
        int32_t power, power_max;
        int32_t threshold, trace;
    } players[2];
    
    // Available choices (if any)
    const char** choices;
    
    // Shopping information (if in shop mode)
    struct {
        bool available;
        const char* shop_cards[13]; // 0-9, a, b, c
    } shop_info;
    
    // Last input processed
    int32_t last_input;
    
    // Status messages
    char status_message[256];
    bool game_over;
} GameLogicState;

// Core Functions

/**
 * Initialize game with enhanced character selection (from groupmate's update)
 * @param player1_char Character name for player 1 (default: "red_riding_hood")
 * @param player2_char Character name for player 2 (default: "snow_white")
 * @return Initialized game instance
 */
game_t* game_logic_init(const char* player1_char, const char* player2_char);

/**
 * Get current game state without UI dependencies
 * @param game Active game instance
 * @param shopping_mode Whether currently in shopping mode
 * @param state Output structure to fill with game state
 */
void game_logic_get_state(game_t* game, bool shopping_mode, GameLogicState* state);

/**
 * Process game input and return result (based on groupmate's input handling)
 * @param game Active game instance
 * @param input_char Character input from user
 * @param shopping_mode Pointer to shopping mode flag (may be modified)
 * @return Result of input processing
 */
InputResult game_logic_process_input(game_t* game, int input_char, bool* shopping_mode);

/**
 * Cleanup shop info memory allocated in state
 * @param state State structure to cleanup
 */
void game_logic_cleanup_state(GameLogicState* state);

// Utility Functions

/**
 * Format card information into a buffer (improved from groupmate's version)
 * @param card Card to format
 * @param buffer Output buffer for formatted string
 * @param buffer_size Size of output buffer
 * @return Number of characters written
 */
int format_card_info(card_t* card, char* buffer, size_t buffer_size);

/**
 * Get comprehensive player information
 * @param game Active game instance
 * @param player_id Player ID (1 or 2)
 * @param state State structure to update with player info
 */
void get_player_info(game_t* game, int32_t player_id, GameLogicState* state);

/**
 * Get shopping information (based on groupmate's improved shop logic)
 * @param game Active game instance
 * @param turn Current turn number
 * @param state State structure to update with shop info
 */
void get_shop_info(game_t* game, int32_t turn, GameLogicState* state);

// Game State Helper Macros

/**
 * Check if game is waiting for any key to continue
 */
#define GAME_LOGIC_IS_WAITING(state) ((state)->action_type == GAME_ACTION_CONTINUE)

/**
 * Check if game has choices available
 */
#define GAME_LOGIC_HAS_CHOICES(state) ((state)->action_type == GAME_ACTION_CHOOSE && (state)->choice_count > 0)

/**
 * Check if game is in shopping mode
 */
#define GAME_LOGIC_IS_SHOPPING(state) ((state)->action_type == GAME_ACTION_SHOP)

/**
 * Check if game can play cards
 */
#define GAME_LOGIC_CAN_PLAY_CARDS(state) ((state)->action_type == GAME_ACTION_PLAY_CARD)

/**
 * Check if game is over
 */
#define GAME_LOGIC_IS_GAME_OVER(state) ((state)->action_type == GAME_ACTION_GAME_OVER || (state)->game_over)

/**
 * Get current player (1-based)
 */
#define GAME_LOGIC_CURRENT_PLAYER(state) ((state)->current_turn)

/**
 * Get opponent player (1-based)
 */
#define GAME_LOGIC_OPPONENT_PLAYER(state) (((state)->current_turn % 2) + 1)

#endif // GAME_LOGIC_H
