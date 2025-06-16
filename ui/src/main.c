#define _GNU_SOURCE
#include "renderer.h"
#include "input.h"
#include "game_state.h"
#include "bot.h"
#include "menu_system.h"
#include "backend_logic.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

// Application states
typedef enum {
    APP_MENU,
    APP_GAME,
    APP_QUIT
} AppState;

int main(int argc, char* argv[]) {
    printf("Starting Twisted Fables GUI...\n");
    
    // Initialize random seed for character selection
    srand((unsigned int)time(NULL));
    
    // Check for test mode
    bool test_mode = false;
    if (argc > 1 && strcmp(argv[1], "--test") == 0) {
        test_mode = true;
        printf("Running in test mode (no GUI)\n");
    }
    
    // Initialize systems
    Renderer renderer;
    InputHandler input;
    GameState game_state;
    MenuSystem menu_system;
    
    if (!test_mode && !renderer_init(&renderer)) {
        printf("Failed to initialize renderer!\n");
        return 1;
    }
    
    if (!test_mode) {
        input_init(&input);
        if (!menu_system_init(&menu_system)) {
            printf("Failed to initialize menu system!\n");
            renderer_cleanup(&renderer);
            return 1;
        }
    }
    
    AppState app_state = test_mode ? APP_GAME : APP_MENU;
    bool game_initialized = false;
    
    // For test mode, initialize game immediately
    if (test_mode) {
        if (!game_state_init_with_bot(&game_state, "red_riding_hood", "snow_white", true, true, 1)) {
            printf("Failed to initialize game state!\n");
            return 1;
        }
        game_initialized = true;
        app_state = APP_GAME;
        
        // Test mode: run a few turns and exit
        printf("Running bot test for 10 turns...\n");
        for (int i = 0; i < 10 && !game_state.game_over; i++) {
            printf("\n=== Turn %d ===\n", i+1);
            game_state_update(&game_state);
            printf("Player 1 HP: %d, Player 2 HP: %d\n", 
                   info_hp(game_state.game, 1), info_hp(game_state.game, 2));
            usleep(500000); // 0.5 second delay
        }
        
        printf("\nTest completed!\n");
        game_state_cleanup(&game_state);
        return 0;
    }
    
    printf("GUI initialized successfully!\n");
    printf("Use WASD or Arrow Keys to navigate, Enter/Space to select, ESC to exit\n");
    
    // Main application loop
    SDL_Event event;
    Uint32 current_time = SDL_GetTicks();
    
    while (app_state != APP_QUIT) {
        current_time = SDL_GetTicks();
        
        // Handle events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                app_state = APP_QUIT;
                break;
            }
            
            if (app_state == APP_MENU) {
                menu_system_handle_event(&menu_system, &event);
                
                // Check menu state transitions
                if (menu_system_should_exit(&menu_system)) {
                    app_state = APP_QUIT;
                } else if (menu_system_should_start_game(&menu_system)) {
                    // Get game configuration from menu
                    bool p1_bot, p2_bot;
                    int bot_difficulty;
                    char* p1_char;
                    char* p2_char;
                    
                    menu_system_get_game_config(&menu_system, &p1_bot, &p2_bot, 
                                               &bot_difficulty, &p1_char, &p2_char);
                    
                    printf("Starting game: %s vs %s\n", p1_char, p2_char);
                    printf("Player 1: %s, Player 2: %s\n", 
                           p1_bot ? "Bot" : "Human", 
                           p2_bot ? "Bot" : "Human");
                    
                    // Initialize game with selected configuration
                    if (game_state_init_with_bot(&game_state, p1_char, p2_char, p1_bot, p2_bot, bot_difficulty)) {
                        game_initialized = true;
                        app_state = APP_GAME;
                        
                        printf("Game Controls:\n");
                        printf("  Left click: Select card\n");
                        printf("  Right click: Next turn\n");
                        printf("  ESC: Return to menu\n");
                        printf("  Number keys: Make choices/Shop purchases\n");
                        printf("  B: Enter shopping mode\n");
                        printf("  F: Focus action\n");
                        printf("  D: Exit shopping mode\n");
                        printf("  Space: Skip turn (debug)\n");
                        printf("  S/K: Skip bot turn (when bot is stuck)\n");
                    } else {
                        printf("Failed to initialize game!\n");
                        // Stay in menu
                    }
                }
            } else if (app_state == APP_GAME && game_initialized) {
                input_handle_events(&input, &event);
                
                // Handle game-specific events
                if (input_key_pressed(&input, SDL_SCANCODE_ESCAPE)) {
                    // Return to menu
                    game_state_cleanup(&game_state);
                    game_initialized = false;
                    menu_system_reset(&menu_system);
                    app_state = APP_MENU;
                    continue;
                }
                
                // Only process human input if current player is not a bot
                bool current_player_is_bot = (game_state.current_player == 1 && game_state.player1_is_bot) ||
                                           (game_state.current_player == 2 && game_state.player2_is_bot);
                
                if (!current_player_is_bot) {
                    // Handle mouse events for card selection and gameplay
                    game_state_handle_mouse_event(&game_state, &event);
                }
                
                // Handle number key choices
                if (event.type == SDL_KEYDOWN) {
                    if (event.key.keysym.sym >= SDLK_0 && event.key.keysym.sym <= SDLK_9) {
                        int choice_num = event.key.keysym.sym - SDLK_0;
                        
                        if (game_state.shopping_mode) {
                            // Handle shop purchases (0-9)
                            if (choice_num <= 9) {
                                if (game_state_handle_shop_purchase(&game_state, choice_num)) {
                                    printf("Purchased shop item %d\n", choice_num);
                                }
                            }
                        } else {
                            // Handle regular choices
                            const char** choices = info_choice(game_state.game);
                            if (choices && choice_num < info_choice_count(game_state.game)) {
                                game_state_handle_choice(&game_state, choice_num);
                            }
                        }
                    }
                    
                    // Shopping mode letter keys (a, b, c)
                    if (game_state.shopping_mode) {
                        if (event.key.keysym.sym == SDLK_a) {
                            game_state_handle_shop_purchase(&game_state, 10); // Move2
                        } else if (event.key.keysym.sym == SDLK_b) {
                            game_state_handle_shop_purchase(&game_state, 11); // Move3
                        } else if (event.key.keysym.sym == SDLK_c) {
                            game_state_handle_shop_purchase(&game_state, 12); // Wild
                        } else if (event.key.keysym.sym == SDLK_d) {
                            game_state.shopping_mode = false; // Cancel shopping
                            printf("Exited shopping mode\n");
                        }
                    }
                    
                    // Game control keys
                    if (event.key.keysym.sym == SDLK_b && !game_state.shopping_mode) {
                        // Enter shopping mode
                        game_state.shopping_mode = true;
                        printf("Entered shopping mode\n");
                    }
                    
                    if (event.key.keysym.sym == SDLK_f) {
                        // Focus action
                        game_state_handle_focus(&game_state);
                    }
                    
                    // Debug keys
                    if (event.key.keysym.sym == SDLK_SPACE) {
                        // Skip turn
                        printf("Skipping turn...\n");
                        game_next(game_state.game);
                    }
                    
                    if (event.key.keysym.sym == SDLK_s || event.key.keysym.sym == SDLK_k) {
                        // Manual bot skip - force bot to end turn
                        printf("🛑 Manual bot skip requested\n");
                        game_state.manual_bot_skip = true;
                    }
                }
            }
        }
        
        // Update systems
        if (app_state == APP_MENU) {
            menu_system_update(&menu_system, current_time);
        } else if (app_state == APP_GAME && game_initialized) {
            // Update input states
            input_update(&input);
            
            // Update game state (this will handle bot turns automatically)
            game_state_update(&game_state);
            
            // Check for game over
            if (game_state.game_over) {
                printf("Game Over! Returning to menu...\n");
                game_state_cleanup(&game_state);
                game_initialized = false;
                menu_system_reset(&menu_system);
                app_state = APP_MENU;
                continue;
            }
        }
        
        // Render
        if (app_state == APP_MENU) {
            menu_system_render(&menu_system, &renderer);
        } else if (app_state == APP_GAME && game_initialized) {
            game_state_render(&game_state, &renderer);
        }
        
        renderer_present(&renderer);
        
        // Small delay to prevent excessive CPU usage
        SDL_Delay(16); // ~60 FPS
    }
    
    printf("Shutting down...\n");
    
    // Cleanup
    if (game_initialized) {
        game_state_cleanup(&game_state);
    }
    if (!test_mode) {
        menu_system_cleanup(&menu_system);
        renderer_cleanup(&renderer);
    }
    
    printf("Goodbye!\n");
    return 0;
}
