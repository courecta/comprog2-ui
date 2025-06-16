/**
 * Comprehensive Test Framework Header
 * 
 * Consolidates all testing utilities from individual test files into one framework
 */

#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#include "../the-files/tf.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

// Test result tracking
typedef struct {
    int tests_run;
    int tests_passed;
    int tests_failed;
    char last_error[256];
} TestResults;

// Test utilities
#define TEST_ASSERT(condition, message) \
    do { \
        results->tests_run++; \
        if (!(condition)) { \
            results->tests_failed++; \
            snprintf(results->last_error, sizeof(results->last_error), \
                    "FAIL: %s (line %d)", message, __LINE__); \
            printf("❌ %s\n", results->last_error); \
            return false; \
        } else { \
            results->tests_passed++; \
            printf("✅ %s\n", message); \
        } \
    } while(0)

#define TEST_LOG(format, ...) \
    printf("📝 " format "\n", ##__VA_ARGS__)

#define TEST_SECTION(name) \
    printf("\n🔍 === %s ===\n", name)

// Global test results
extern TestResults g_test_results;

// Test function declarations

/**
 * Test basic game initialization and cleanup
 */
bool test_game_initialization(TestResults* results);

/**
 * Test game state information functions
 */
bool test_game_state_info(TestResults* results);

/**
 * Test turn progression and phase management
 */
bool test_turn_progression(TestResults* results);

/**
 * Test card playing mechanics
 */
bool test_card_playing(TestResults* results);

/**
 * Test choice handling
 */
bool test_choice_handling(TestResults* results);

/**
 * Test bot functionality
 */
bool test_bot_functionality(TestResults* results);

/**
 * Test activation phase mechanics
 */
bool test_activation_phases(TestResults* results);

/**
 * Test edge cases and error conditions
 */
bool test_edge_cases(TestResults* results);

/**
 * Utility functions
 */
void print_game_state(game_t* game);
void print_player_state(game_t* game, int player_id);
void print_hand_cards(game_t* game, int player_id);
int count_hand_cards(game_t* game, int player_id);
bool advance_to_activation(game_t* game);
void init_test_results(TestResults* results);
void print_test_summary(TestResults* results);

#endif // TEST_FRAMEWORK_H
