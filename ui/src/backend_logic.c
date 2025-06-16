/**
 * Backend Logic Module - Bridge between Twisted Fables game engine and GUI
 * 
 * This module extracts useful functions from backup files and provides a clean
 * interface between the tf.h game engine and the SDL2 GUI layer.
 * 
 * Key Functions Extracted from backup_files/:
 * - Game state printing and diagnostics (from fixed_main.c, working_main.c)
 * - Safe card playing strategies (avoiding type 128 problematic cards)
 * - Proper phase management and turn advancement
 * - Choice handling and game flow control
 */

#include "tf.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Include our header after tf.h to avoid type conflicts
#include "backend_logic.h"

/**
 * Print card information (extracted from backup files)
 */
void backend_print_card(card_t* card, char* buffer, size_t buffer_size) {
    if (!card || !buffer) {
        snprintf(buffer, buffer_size, "None");
        return;
    }
    
    const char* name = info_card_name(card);
    cardtype_t type = info_card_type(card);
    int32_t lvl = info_card_lvl(card);
    int32_t cost = info_card_cost(card);
    const char* attr = info_card_attr(card);
    card_t* sub = info_card_sub(card);
    const char* sub_name = (sub) ? info_card_name(sub) : "None";
    
    snprintf(buffer, buffer_size, "%s - %d/%d/%d - '%s' - %s",
             name, type, lvl, cost, attr, sub_name);
}

/**
 * Print player state information (extracted and enhanced from backup files)
 */
void backend_print_player(game_t* game, int32_t player_id, char* buffer, size_t buffer_size) {
    if (!game || !buffer) return;
    
    const char* name = info_name(game, player_id);
    int32_t hp = info_hp(game, player_id);
    int32_t hp_max = info_hp_max(game, player_id);
    int32_t def = info_def(game, player_id);
    int32_t def_max = info_def_max(game, player_id);
    int32_t power = info_power(game, player_id);
    int32_t power_max = info_power_max(game, player_id);
    int32_t threshold = info_threshold(game, player_id);
    int32_t trace = info_trace(game, player_id);
    
    int written = snprintf(buffer, buffer_size, 
                          "NAME: %s\nHP: %d/%d  DEF: %d/%d  PWR: %d/%d  TRE: %d  TRA: %d\n",
                          name, hp, hp_max, def, def_max, power, power_max, threshold, trace);
    
    if (written < 0 || (size_t)written >= buffer_size) return;
    
    // Add hand information
    pile_t* hand = info_hand_pile(game, player_id);
    char card_buffer[256];
    int32_t handC = 0;
    
    size_t remaining = buffer_size - written;
    char* current_pos = buffer + written;
    
    written += snprintf(current_pos, remaining, "P%d hand:\n", player_id);
    if (written >= (int)buffer_size) return;
    
    remaining = buffer_size - written;
    current_pos = buffer + written;
    
    foreachcard(card, hand) {
        backend_print_card(card, card_buffer, sizeof(card_buffer));
        int card_written = snprintf(current_pos, remaining, "%d) %s\n", handC++, card_buffer);
        if (card_written < 0 || (size_t)card_written >= remaining) break;
        written += card_written;
        remaining -= card_written;
        current_pos += card_written;
    }
}

/**
 * Get game state summary (extracted from headless_main.c)
 */
void backend_get_game_state(game_t* game, GameStateInfo* state_info) {
    if (!game || !state_info) return;
    
    state_info->turn = info_turn(game);
    state_info->phase = 0; // Phase not available in tf.h
    state_info->activation = info_activation(game);
    state_info->is_game_over = (info_hp(game, 1) <= 0 || info_hp(game, 2) <= 0);
    
    // Get choice information
    const char** choices = info_choice(game);
    state_info->choice_count = 0;
    for (; choices && choices[state_info->choice_count] && state_info->choice_count < MAX_CHOICES; 
         state_info->choice_count++) {
        strncpy(state_info->choices[state_info->choice_count], 
                choices[state_info->choice_count], 
                MAX_CHOICE_LENGTH - 1);
        state_info->choices[state_info->choice_count][MAX_CHOICE_LENGTH - 1] = '\0';
    }
}

/**
 * Safe card playing strategy (extracted from working_main.c)
 * Avoids problematic type 128 cards that can cause issues
 */
bool backend_can_play_card_safely(card_t* card) {
    if (!card) return false;
    
    cardtype_t type = info_card_type(card);
    int32_t cost = info_card_cost(card);
    
    // Avoid type 128 cards (System Hack, etc.) as they can cause issues
    if (type == 128) return false;
    
    // Basic safety check - ensure we have enough power
    // Note: This should be checked against current player power in actual implementation
    return true;
}

/**
 * Advance game state safely (extracted from fixed_main.c pattern)
 */
GameAdvanceResult backend_advance_game(game_t* game) {
    GameAdvanceResult result = {0};
    
    if (!game) {
        result.action_taken = ACTION_ERROR;
        result.success = false;
        result.message = "Game is NULL";
        return result;
    }
    
    // Check if we need to advance through setup phases
    if (!info_activation(game)) {
        result.action_taken = ACTION_NEXT_PHASE;
        result.success = (game_next(game) == 0);
        result.message = result.success ? "Advanced to next phase" : "Failed to advance phase";
        return result;
    }
    
    // Check for choices first
    const char** choices = info_choice(game);
    int32_t choiceC = 0;
    for (; choices && choices[choiceC]; choiceC++);
    
    if (choiceC > 0) {
        result.action_taken = ACTION_CHOICE_AVAILABLE;
        result.choice_count = choiceC;
        result.success = true;
        result.message = "Choices available for player";
        return result;
    }
    
    // In activation phase with no choices - player can play cards or end turn
    result.action_taken = ACTION_PLAYER_TURN;
    result.success = true;
    result.message = "Player turn - can play cards or end turn";
    return result;
}

/**
 * Initialize game with proper error handling (extracted pattern from backup files)
 */
game_t* backend_init_game(const char* player1_character, const char* player2_character) {
    if (!player1_character || !player2_character) return NULL;
    
    game_t* game = game_init(player1_character, player2_character);
    if (!game) return NULL;
    
    // Advance through initial setup phases
    for (int i = 0; i < 10 && !info_activation(game); i++) {
        if (game_next(game) != 0) {
            printf("Warning: Setup phase %d failed\n", i);
            break;
        }
    }
    
    return game;
}

/**
 * Get hand card count for player
 */
int32_t backend_get_hand_count(game_t* game, int32_t player_id) {
    if (!game) return 0;
    
    pile_t* hand = info_hand_pile(game, player_id);
    int32_t count = 0;
    foreachcard(card, hand) {
        count++;
    }
    return count;
}

/**
 * Get specific card from hand by index
 */
card_t* backend_get_hand_card(game_t* game, int32_t player_id, int32_t card_index) {
    if (!game) return NULL;
    
    pile_t* hand = info_hand_pile(game, player_id);
    int32_t current_index = 0;
    foreachcard(card, hand) {
        if (current_index == card_index) {
            return card;
        }
        current_index++;
    }
    return NULL;
}

/**
 * Debug function to print full game state (useful for troubleshooting)
 */
void backend_debug_game_state(game_t* game) {
    if (!game) {
        printf("DEBUG: Game is NULL\n");
        return;
    }
    
    printf("=== DEBUG GAME STATE ===\n");
    printf("Turn: %d, Activation: %d\n", 
           info_turn(game), info_activation(game));
    
    for (int i = 1; i <= 2; i++) {
        printf("\n--- Player %d ---\n", i);
        printf("HP: %d/%d, DEF: %d/%d, PWR: %d/%d\n",
               info_hp(game, i), info_hp_max(game, i),
               info_def(game, i), info_def_max(game, i),
               info_power(game, i), info_power_max(game, i));
        
        printf("Hand cards: %d\n", backend_get_hand_count(game, i));
    }
    
    // Check for choices
    const char** choices = info_choice(game);
    if (choices && choices[0]) {
        printf("\nAvailable choices:\n");
        for (int i = 0; choices[i]; i++) {
            printf("  %d) %s\n", i, choices[i]);
        }
    }
    
    printf("========================\n");
}
