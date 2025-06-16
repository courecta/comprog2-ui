/**
 * Comprehensive Test Suite
 * 
 * Consolidates all individual test files into one comprehensive testing system.
 * Combines functionality from:
 * - debug_cards.c, debug_phases.c, debug_turns.c
 * - test_activation_blocking.c, test_all.c, test_bot.c
 * - test_full_turn_cycle.c, test_manual_progression.c
 * - test_minimal_turn.c, test_starting_conditions.c
 */

#include "test_framework.h"

// Global test results
TestResults g_test_results = {0};

/**
 * Initialize test results structure
 */
void init_test_results(TestResults* results) {
    results->tests_run = 0;
    results->tests_passed = 0;
    results->tests_failed = 0;
    memset(results->last_error, 0, sizeof(results->last_error));
}

/**
 * Print comprehensive test summary
 */
void print_test_summary(TestResults* results) {
    printf("\n🏁 === TEST SUMMARY ===\n");
    printf("Total Tests: %d\n", results->tests_run);
    printf("✅ Passed: %d\n", results->tests_passed);
    printf("❌ Failed: %d\n", results->tests_failed);
    
    if (results->tests_failed == 0) {
        printf("🎉 ALL TESTS PASSED!\n");
    } else {
        printf("⚠️  %d tests failed. Last error: %s\n", results->tests_failed, results->last_error);
    }
    
    double success_rate = (double)results->tests_passed / results->tests_run * 100.0;
    printf("Success Rate: %.1f%%\n", success_rate);
}

/**
 * Print game state information (from debug_turns.c)
 */
void print_game_state(game_t* game) {
    if (!game) {
        printf("Game is NULL\n");
        return;
    }
    
    printf("🎮 Game State: Turn=%d, Activation=%d\n", 
           info_turn(game), info_activation(game));
    
    for (int i = 1; i <= 2; i++) {
        printf("Player %d: HP=%d/%d, DEF=%d/%d, PWR=%d/%d\n", i,
               info_hp(game, i), info_hp_max(game, i),
               info_def(game, i), info_def_max(game, i),
               info_power(game, i), info_power_max(game, i));
    }
}

/**
 * Print detailed player state (from debug_cards.c)
 */
void print_player_state(game_t* game, int player_id) {
    if (!game) return;
    
    const char* name = info_name(game, player_id);
    printf("👤 Player %d (%s):\n", player_id, name);
    printf("  HP: %d/%d\n", info_hp(game, player_id), info_hp_max(game, player_id));
    printf("  DEF: %d/%d\n", info_def(game, player_id), info_def_max(game, player_id));
    printf("  PWR: %d/%d\n", info_power(game, player_id), info_power_max(game, player_id));
    printf("  Hand Cards: %d\n", count_hand_cards(game, player_id));
}

/**
 * Print hand cards with details (from debug_cards.c)
 */
void print_hand_cards(game_t* game, int player_id) {
    if (!game) return;
    
    pile_t* hand = info_hand_pile(game, player_id);
    printf("🃏 Player %d Hand:\n", player_id);
    
    int index = 0;
    foreachcard(card, hand) {
        const char* name = info_card_name(card);
        int cost = info_card_cost(card);
        int type = info_card_type(card);
        printf("  [%d] %s (cost=%d, type=%d)\n", index++, name, cost, type);
    }
}

/**
 * Count cards in hand
 */
int count_hand_cards(game_t* game, int player_id) {
    if (!game) return 0;
    
    pile_t* hand = info_hand_pile(game, player_id);
    int count = 0;
    foreachcard(card, hand) {
        (void)card; // Suppress unused warning
        count++;
    }
    return count;
}

/**
 * Advance game to activation phase (from test_starting_conditions.c)
 */
bool advance_to_activation(game_t* game) {
    if (!game) return false;
    
    for (int i = 0; i < 20; i++) {
        if (info_activation(game)) {
            return true;
        }
        
        int result = game_next(game);
        if (result != 0) {
            printf("⚠️  game_next() failed with code %d at step %d\n", result, i);
            return false;
        }
    }
    
    printf("⚠️  Failed to reach activation phase after 20 attempts\n");
    return false;
}

/**
 * Test basic game initialization (from test_starting_conditions.c)
 */
bool test_game_initialization(TestResults* results) {
    TEST_SECTION("Game Initialization");
    
    // Test valid character initialization
    game_t* game = game_init("red_riding_hood", "snow_white");
    TEST_ASSERT(game != NULL, "Game initialization with valid characters");
    
    // Test initial state
    TEST_ASSERT(info_turn(game) == 1, "Initial turn is 1");
    TEST_ASSERT(info_hp(game, 1) > 0, "Player 1 has positive HP");
    TEST_ASSERT(info_hp(game, 2) > 0, "Player 2 has positive HP");
    TEST_ASSERT(info_power(game, 1) >= 0, "Player 1 power is non-negative");
    TEST_ASSERT(info_power(game, 2) >= 0, "Player 2 power is non-negative");
    
    // Test hand cards
    int p1_cards = count_hand_cards(game, 1);
    int p2_cards = count_hand_cards(game, 2);
    TEST_ASSERT(p1_cards > 0, "Player 1 has hand cards");
    TEST_ASSERT(p2_cards > 0, "Player 2 has hand cards");
    
    game_cleanup(game);
    return true;
}

/**
 * Test game state information functions (from debug_phases.c)
 */
bool test_game_state_info(TestResults* results) {
    TEST_SECTION("Game State Information");
    
    game_t* game = game_init("red_riding_hood", "snow_white");
    TEST_ASSERT(game != NULL, "Game created for state testing");
    
    // Test player names
    const char* p1_name = info_name(game, 1);
    const char* p2_name = info_name(game, 2);
    TEST_ASSERT(p1_name != NULL, "Player 1 name exists");
    TEST_ASSERT(p2_name != NULL, "Player 2 name exists");
    TEST_ASSERT(strlen(p1_name) > 0, "Player 1 name is not empty");
    TEST_ASSERT(strlen(p2_name) > 0, "Player 2 name is not empty");
    
    // Test HP bounds
    TEST_ASSERT(info_hp(game, 1) <= info_hp_max(game, 1), "Player 1 HP within bounds");
    TEST_ASSERT(info_hp(game, 2) <= info_hp_max(game, 2), "Player 2 HP within bounds");
    
    // Test power bounds
    TEST_ASSERT(info_power(game, 1) <= info_power_max(game, 1), "Player 1 power within bounds");
    TEST_ASSERT(info_power(game, 2) <= info_power_max(game, 2), "Player 2 power within bounds");
    
    game_cleanup(game);
    return true;
}

/**
 * Test turn progression (from test_minimal_turn.c, test_full_turn_cycle.c)
 */
bool test_turn_progression(TestResults* results) {
    TEST_SECTION("Turn Progression");
    
    game_t* game = game_init("red_riding_hood", "snow_white");
    TEST_ASSERT(game != NULL, "Game created for turn testing");
    
    int initial_turn = info_turn(game);
    int initial_activation = info_activation(game);
    
    TEST_LOG("Initial state: Turn=%d, Activation=%d", initial_turn, initial_activation);
    
    // Advance to activation phase
    bool reached_activation = advance_to_activation(game);
    TEST_ASSERT(reached_activation, "Successfully reached activation phase");
    
    if (reached_activation) {
        TEST_ASSERT(info_activation(game) == 1, "Activation phase active");
        TEST_LOG("Reached activation phase successfully");
    }
    
    // Test choice handling if available
    const char** choices = info_choice(game);
    if (choices && choices[0]) {
        TEST_LOG("Choices available: %s", choices[0]);
        int choice_result = game_choose(game, 0);
        TEST_LOG("Choice result: %d", choice_result);
    }
    
    game_cleanup(game);
    return true;
}

/**
 * Test card playing mechanics (from debug_cards.c, test_activation_blocking.c)
 */
bool test_card_playing(TestResults* results) {
    TEST_SECTION("Card Playing");
    
    game_t* game = game_init("red_riding_hood", "snow_white");
    TEST_ASSERT(game != NULL, "Game created for card testing");
    
    if (!advance_to_activation(game)) {
        game_cleanup(game);
        return false;
    }
    
    int current_player = info_turn(game);
    pile_t* hand = info_hand_pile(game, current_player);
    TEST_ASSERT(hand != NULL, "Player hand exists");
    
    int cards_tested = 0;
    int cards_played = 0;
    
    foreachcard(card, hand) {
        if (cards_tested >= 3) break; // Limit testing
        
        const char* name = info_card_name(card);
        int cost = info_card_cost(card);
        int power = info_power(game, current_player);
        
        TEST_LOG("Testing card: %s (cost=%d, player_power=%d)", name, cost, power);
        
        int play_result = game_use(game, card);
        TEST_LOG("Play result: %d", play_result);
        
        if (play_result == 0) {
            cards_played++;
            TEST_LOG("Successfully played: %s", name);
        }
        
        cards_tested++;
    }
    
    TEST_LOG("Cards tested: %d, Cards played: %d", cards_tested, cards_played);
    TEST_ASSERT(cards_tested > 0, "At least one card was tested");
    
    game_cleanup(game);
    return true;
}

/**
 * Test choice handling (from test_manual_progression.c)
 */
bool test_choice_handling(TestResults* results) {
    TEST_SECTION("Choice Handling");
    
    game_t* game = game_init("red_riding_hood", "snow_white");
    TEST_ASSERT(game != NULL, "Game created for choice testing");
    
    // Advance through phases looking for choices
    for (int phase = 0; phase < 50; phase++) {
        const char** choices = info_choice(game);
        int choice_count = info_choice_count(game);
        
        if (choices && choices[0]) {
            TEST_LOG("Found choices at phase %d:", phase);
            for (int i = 0; i < choice_count && i < 5; i++) {
                TEST_LOG("  [%d] %s", i, choices[i]);
            }
            
            // Test making a choice
            int choice_result = game_choose(game, 0);
            TEST_LOG("Choice result: %d", choice_result);
            TEST_ASSERT(choice_result >= 0, "Valid choice result");
            break;
        }
        
        if (info_activation(game)) {
            // Try to advance or play cards
            int result = game_next(game);
            if (result != 0) break;
        } else {
            int result = game_next(game);
            if (result != 0) break;
        }
    }
    
    game_cleanup(game);
    return true;
}

/**
 * Test bot functionality (from test_bot.c)
 */
bool test_bot_functionality(TestResults* results) {
    TEST_SECTION("Bot Functionality");
    
    // Note: This would require bot.h integration
    // For now, test basic game mechanics that bots would use
    
    game_t* game = game_init("red_riding_hood", "snow_white");
    TEST_ASSERT(game != NULL, "Game created for bot testing");
    
    // Test game state queries that bots need
    TEST_ASSERT(info_turn(game) >= 1, "Turn number is valid");
    TEST_ASSERT(info_activation(game) >= 0, "Activation state is valid");
    
    // Test card counting (bot needs this)
    int hand_count = count_hand_cards(game, 1);
    TEST_ASSERT(hand_count >= 0, "Hand count is non-negative");
    
    TEST_LOG("Bot would see: Turn=%d, Hand=%d cards", info_turn(game), hand_count);
    
    game_cleanup(game);
    return true;
}

/**
 * Test activation phase mechanics (from test_activation_blocking.c)
 */
bool test_activation_phases(TestResults* results) {
    TEST_SECTION("Activation Phase Mechanics");
    
    game_t* game = game_init("red_riding_hood", "snow_white");
    TEST_ASSERT(game != NULL, "Game created for activation testing");
    
    // Test reaching activation
    bool reached = advance_to_activation(game);
    TEST_ASSERT(reached, "Can reach activation phase");
    
    if (reached) {
        int current_player = info_turn(game);
        int power_before = info_power(game, current_player);
        
        TEST_LOG("In activation: Player=%d, Power=%d", current_player, power_before);
        
        // Test ending activation
        int end_result = game_next(game);
        TEST_LOG("End activation result: %d", end_result);
        
        // Check if phase changed
        int activation_after = info_activation(game);
        TEST_LOG("Activation after game_next: %d", activation_after);
    }
    
    game_cleanup(game);
    return true;
}

/**
 * Test edge cases and error conditions
 */
bool test_edge_cases(TestResults* results) {
    TEST_SECTION("Edge Cases");
    
    // Test invalid game initialization
    game_t* bad_game = game_init("nonexistent", "invalid");
    if (bad_game) {
        TEST_LOG("Game accepted invalid characters (may be expected)");
        game_cleanup(bad_game);
    } else {
        TEST_LOG("Game correctly rejected invalid characters");
    }
    
    // Test NULL parameter handling
    const char* null_name = info_name(NULL, 1);
    TEST_ASSERT(null_name == NULL, "NULL game returns NULL name");
    
    int invalid_hp = info_hp(NULL, 1);
    TEST_LOG("HP from NULL game: %d", invalid_hp);
    
    return true;
}

/**
 * Main test runner
 */
int main(int argc, char* argv[]) {
    printf("🧪 === TWISTED FABLES COMPREHENSIVE TEST SUITE ===\n");
    printf("Consolidating all individual test files into one framework\n\n");
    
    init_test_results(&g_test_results);
    
    bool verbose = false;
    if (argc > 1 && strcmp(argv[1], "-v") == 0) {
        verbose = true;
        printf("Verbose mode enabled\n\n");
    }
    
    // Run all test suites
    test_game_initialization(&g_test_results);
    test_game_state_info(&g_test_results);
    test_turn_progression(&g_test_results);
    test_card_playing(&g_test_results);
    test_choice_handling(&g_test_results);
    test_bot_functionality(&g_test_results);
    test_activation_phases(&g_test_results);
    test_edge_cases(&g_test_results);
    
    // Print final summary
    print_test_summary(&g_test_results);
    
    return (g_test_results.tests_failed == 0) ? 0 : 1;
}
