/**
 * Core Game Logic Module - Pure Logic Without UI Dependencies
 * 
 * This module extracts the core game logic from main_curses.c and makes it
 * UI-independent, returning structured data instead of printing directly.
 * Based on groupmate's improved main.c with enhanced shopping and card handling.
 */

#define _GNU_SOURCE  // For strdup function
#include "tf.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

// Game action types
typedef enum {
    GAME_ACTION_NONE = 0,
    GAME_ACTION_CONTINUE,      // Press any key to continue (non-activation phase)
    GAME_ACTION_CHOOSE,        // Make a choice from available options
    GAME_ACTION_SHOP,          // Shopping mode active
    GAME_ACTION_PLAY_CARD,     // Can play cards or use actions
    GAME_ACTION_GAME_OVER      // Game has ended
} GameActionType;

// Game state information structure
typedef struct {
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

/**
 * Format card information into a buffer (improved from groupmate's version)
 */
int format_card_info(card_t* card, char* buffer, size_t buffer_size) {
    if (!card || !buffer) {
        snprintf(buffer, buffer_size, "None");
        return 0;
    }
    
    int32_t id = info_card_id(card);  // Using groupmate's working function
    const char* name = info_card_name(card);
    cardtype_t type = info_card_type(card);
    int32_t lvl = info_card_lvl(card);
    int32_t cost = info_card_cost(card);
    const char* attr = info_card_attr(card);
    card_t* sub = info_card_sub(card);
    const char* sub_name = (sub) ? info_card_name(sub) : "None";
    
    return snprintf(buffer, buffer_size, "%d - %s - %d/%d/%d - '%s' - %s",
                   id, name, type, lvl, cost, attr, sub_name);
}

/**
 * Get comprehensive player information
 */
void get_player_info(game_t* game, int32_t player_id, GameLogicState* state) {
    if (!game || !state || player_id < 1 || player_id > 2) return;
    
    int idx = player_id - 1;
    state->players[idx].name = info_name(game, player_id);
    state->players[idx].hp = info_hp(game, player_id);
    state->players[idx].hp_max = info_hp_max(game, player_id);
    state->players[idx].def = info_def(game, player_id);
    state->players[idx].def_max = info_def_max(game, player_id);
    state->players[idx].power = info_power(game, player_id);
    state->players[idx].power_max = info_power_max(game, player_id);
    state->players[idx].threshold = info_threshold(game, player_id);
    state->players[idx].trace = info_trace(game, player_id);
}

/**
 * Get shopping information (based on groupmate's improved shop logic)
 */
void get_shop_info(game_t* game, int32_t turn, GameLogicState* state) {
    if (!game || !state) return;
    
    state->shop_info.available = true;
    
    // Player-specific cards (0-2)
    for (int32_t i = 0; i < 3; i++) {
        pile_t* shop = info_shop_pile(game, SP_P1 - 4 + 4 * turn + 1 + i);
        card_t* card = info_pile_card(shop, 0);
        char card_buffer[256];
        format_card_info(card, card_buffer, sizeof(card_buffer));
        state->shop_info.shop_cards[i] = strdup(card_buffer);
    }
    
    // Attack cards (3-5)
    for (int32_t i = 0; i < 3; i++) {
        pile_t* shop = info_shop_pile(game, SP_ATK1 + i);
        card_t* card = info_pile_card(shop, 0);
        char card_buffer[256];
        format_card_info(card, card_buffer, sizeof(card_buffer));
        state->shop_info.shop_cards[i + 3] = strdup(card_buffer);
    }
    
    // Defense cards (6-8)
    for (int32_t i = 0; i < 3; i++) {
        pile_t* shop = info_shop_pile(game, SP_DEF1 + i);
        card_t* card = info_pile_card(shop, 0);
        char card_buffer[256];
        format_card_info(card, card_buffer, sizeof(card_buffer));
        state->shop_info.shop_cards[i + 6] = strdup(card_buffer);
    }
    
    // Movement cards (9, a, b)
    pile_t* shop = info_shop_pile(game, SP_MOVE1);
    card_t* card = info_pile_card(shop, 0);
    char card_buffer[256];
    format_card_info(card, card_buffer, sizeof(card_buffer));
    state->shop_info.shop_cards[9] = strdup(card_buffer);
    
    for (int32_t i = 0; i < 2; i++) {
        shop = info_shop_pile(game, SP_MOVE2 + i);
        card = info_pile_card(shop, 0);
        format_card_info(card, card_buffer, sizeof(card_buffer));
        state->shop_info.shop_cards[10 + i] = strdup(card_buffer);
    }
    
    // Wild card (c)
    shop = info_shop_pile(game, SP_WILD);
    card = info_pile_card(shop, 0);
    format_card_info(card, card_buffer, sizeof(card_buffer));
    state->shop_info.shop_cards[12] = strdup(card_buffer);
}

/**
 * Initialize game with enhanced character selection (from groupmate's update)
 */
game_t* game_logic_init(const char* player1_char, const char* player2_char) {
    if (!player1_char || !player2_char) {
        // Default to groupmate's improved pairing
        player1_char = "red_riding_hood";
        player2_char = "snow_white";
    }
    
    return game_init(player1_char, player2_char);
}

/**
 * Get current game state without UI dependencies
 */
void game_logic_get_state(game_t* game, bool shopping_mode, GameLogicState* state) {
    if (!game || !state) return;
    
    memset(state, 0, sizeof(GameLogicState));
    
    // Basic game information
    state->current_turn = info_turn(game);
    state->is_activation_phase = info_activation(game);
    state->is_shopping_mode = shopping_mode;
    
    // Get player information
    get_player_info(game, 1, state);
    get_player_info(game, 2, state);
    
    // Get hand count
    pile_t* hand = info_hand_pile(game, state->current_turn);
    state->hand_count = 0;
    foreachcard(card, hand) {
        (void)card;
        state->hand_count++;
    }
    
    // Get choices
    state->choices = info_choice(game);
    state->choice_count = 0;
    if (state->choices) {
        for (; state->choices[state->choice_count]; state->choice_count++);
    }
    
    // Determine action type
    if (!state->is_activation_phase) {
        state->action_type = GAME_ACTION_CONTINUE;
        strcpy(state->status_message, "Press any to continue...");
    } else if (state->choice_count > 0) {
        state->action_type = GAME_ACTION_CHOOSE;
        strcpy(state->status_message, "Choose an option...");
    } else if (state->is_shopping_mode) {
        state->action_type = GAME_ACTION_SHOP;
        strcpy(state->status_message, "Shopping mode - choose item to buy");
        get_shop_info(game, state->current_turn, state);
    } else {
        state->action_type = GAME_ACTION_PLAY_CARD;
        strcpy(state->status_message, "Choose a card to use...");
    }
    
    // Check for game over
    state->game_over = (state->players[0].hp <= 0 || state->players[1].hp <= 0);
    if (state->game_over) {
        state->action_type = GAME_ACTION_GAME_OVER;
        if (state->players[0].hp <= 0) {
            strcpy(state->status_message, "Player 2 Wins!");
        } else {
            strcpy(state->status_message, "Player 1 Wins!");
        }
    }
}

/**
 * Process game input and return result (based on groupmate's input handling)
 */
typedef enum {
    INPUT_RESULT_CONTINUE = 0,
    INPUT_RESULT_SUCCESS,
    INPUT_RESULT_INVALID,
    INPUT_RESULT_EXIT
} InputResult;

InputResult game_logic_process_input(game_t* game, int input_char, bool* shopping_mode) {
    if (!game || !shopping_mode) return INPUT_RESULT_INVALID;
    
    GameLogicState state;
    game_logic_get_state(game, *shopping_mode, &state);
    
    // Exit condition
    if (input_char == 27) { // ESC
        return INPUT_RESULT_EXIT;
    }
    
    // Handle different game states
    if (!state.is_activation_phase) {
        // Any key continues in non-activation phase
        int result = game_next(game);
        return (result != 0) ? INPUT_RESULT_SUCCESS : INPUT_RESULT_INVALID;
    }
    
    if (state.choice_count > 0) {
        // Handle choices
        if (input_char >= '0' && input_char - '0' < state.choice_count) {
            int result = game_choose(game, input_char - '0');
            return (result != 0) ? INPUT_RESULT_SUCCESS : INPUT_RESULT_INVALID;
        }
        return INPUT_RESULT_INVALID;
    }
    
    if (*shopping_mode) {
        // Shopping mode (enhanced from groupmate's logic)
        if (input_char >= '0' && input_char <= '2') {
            int result = game_buy(game, SP_P11 - 4 + 4 * state.current_turn + input_char - '0');
            return (result != 0) ? INPUT_RESULT_SUCCESS : INPUT_RESULT_INVALID;
        }
        if (input_char >= '3' && input_char <= '5') {
            int result = game_buy(game, SP_ATK + input_char - '2');
            return (result != 0) ? INPUT_RESULT_SUCCESS : INPUT_RESULT_INVALID;
        }
        if (input_char >= '6' && input_char <= '8') {
            int result = game_buy(game, SP_DEF + input_char - '5');
            return (result != 0) ? INPUT_RESULT_SUCCESS : INPUT_RESULT_INVALID;
        }
        if (input_char == '9') {
            int result = game_buy(game, SP_MOVE1);
            return (result != 0) ? INPUT_RESULT_SUCCESS : INPUT_RESULT_INVALID;
        }
        if (input_char >= 'a' && input_char <= 'b') {
            int result = game_buy(game, SP_MOVE2 + input_char - 'a');
            return (result != 0) ? INPUT_RESULT_SUCCESS : INPUT_RESULT_INVALID;
        }
        if (input_char == 'c') {
            int result = game_buy(game, SP_WILD);
            return (result != 0) ? INPUT_RESULT_SUCCESS : INPUT_RESULT_INVALID;
        }
        if (input_char == 'd') {
            *shopping_mode = false;
            return INPUT_RESULT_SUCCESS;
        }
        return INPUT_RESULT_INVALID;
    }
    
    // Regular card playing mode
    if (input_char >= '0' && input_char - '0' < state.hand_count) {
        pile_t* hand = info_hand_pile(game, state.current_turn);
        card_t* selected_card = info_pile_card(hand, input_char - '0');
        int result = game_use(game, selected_card);
        return (result != 0) ? INPUT_RESULT_SUCCESS : INPUT_RESULT_INVALID;
    }
    
    if (input_char == 'f') {
        int result = game_focus(game);
        return (result != 0) ? INPUT_RESULT_SUCCESS : INPUT_RESULT_INVALID;
    }
    
    if (input_char == '\n' || input_char == '\r') {
        int result = game_next(game);
        return (result != 0) ? INPUT_RESULT_SUCCESS : INPUT_RESULT_INVALID;
    }
    
    if (input_char == 'b') {
        *shopping_mode = true;
        return INPUT_RESULT_SUCCESS;
    }
    
    return INPUT_RESULT_INVALID;
}

/**
 * Cleanup shop info memory
 */
void game_logic_cleanup_state(GameLogicState* state) {
    if (!state) return;
    
    if (state->shop_info.available) {
        for (int i = 0; i < 13; i++) {
            if (state->shop_info.shop_cards[i]) {
                free((void*)state->shop_info.shop_cards[i]);
                state->shop_info.shop_cards[i] = NULL;
            }
        }
    }
}
