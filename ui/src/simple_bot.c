#define _GNU_SOURCE
#include "simple_bot.h"
#include "bot.h"  // For the AI bot functions
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

bool simple_bot_init(SimpleBot* bot, int player_id, SimpleBotType bot_type, game_t* game) {
    if (!bot || !game || player_id < 1 || player_id > 2) return false;
    
    memset(bot, 0, sizeof(SimpleBot));
    
    bot->player_id = player_id;
    bot->bot_type = bot_type;
    bot->game = game;
    
    printf("🤖 Initialized Simple Bot for Player %d (Type: %s)\n", 
           player_id, (bot_type == SIMPLE_BOT_AI) ? "AI" : "Random");
    
    return true;
}

void simple_bot_cleanup(SimpleBot* bot) {
    if (!bot) return;
    // Nothing to clean up currently
}

char* simple_bot_get_action(SimpleBot* bot) {
    if (!bot || !bot->game) return strdup("q");
    
    if (bot->bot_type == SIMPLE_BOT_AI) {
        // Try to use the AI bot system
        char* action = NULL;
        
        // Only try AI if we have the bot function available
        #ifdef USE_AI_BOT
        action = ai_bot(bot->game);
        #endif
        
        if (action && strlen(action) > 0) {
            printf("🧠 AI Bot decision: '%s'\n", action);
            return action;
        } else {
            printf("🔄 AI Bot failed, falling back to rule-based bot\n");
            if (action) free(action);
            // Fall through to rule-based bot
        }
    }
    
    // Simple rule-based bot fallback
    pile_t* hand = info_hand_pile(bot->game, bot->player_id);
    if (!hand) {
        printf("🎲 Rule Bot: No hand, ending turn\n");
        return strdup("q");
    }
    
    int hand_size = 0;
    foreachcard(card, hand) {
        if (card) hand_size++;
    }
    
    if (hand_size == 0) {
        printf("🎲 Rule Bot: Empty hand, ending turn\n");
        return strdup("q");
    }
    
    // Try to play first affordable card
    int my_power = info_power(bot->game, bot->player_id);
    
    printf("🎲 Rule Bot: Hand size=%d, Power=%d\n", hand_size, my_power);
    
    // Check if we have any choices pending (which might be blocking card play)
    const char** choices = info_choice(bot->game);
    int choice_count = info_choice_count(bot->game);
    
    if (choices && choice_count > 0) {
        printf("🎲 Rule Bot: Found %d pending choices, making choice 0\n", choice_count);
        return strdup("0"); // Always choose first option when there are choices
    }
    
    for (int i = 0; i < hand_size; i++) {
        card_t* card = info_pile_card(hand, i);
        if (card) {
            int cost = info_card_cost(card);
            const char* card_name = info_card_name(card);
            printf("   Card %d: %s (cost=%d)\n", i, card_name, cost);
            
            if (cost <= my_power) {
                char* action = malloc(16);
                snprintf(action, 16, "%d", i);
                printf("🎲 Rule Bot decision: Play card %d (%s, cost=%d)\n", 
                       i, card_name, cost);
                return action;
            }
        }
    }
    
    printf("🎲 Rule Bot: No affordable cards, ending turn\n");
    return strdup("q");
}

bool simple_bot_take_turn(SimpleBot* bot) {
    if (!bot || !bot->game) return false;
    
    // Get the bot's decision
    char* action = simple_bot_get_action(bot);
    if (!action) {
        printf("❌ Bot failed to get action\n");
        return false;
    }
    
    printf("🎮 Bot (Player %d) executing action: '%s'\n", bot->player_id, action);
    
    // Use the new game_bot() function!
    int result = game_bot(bot->game, action);
    
    printf("   game_bot() returned: %d (%s)\n", result, (result != 0) ? "SUCCESS" : "FAILURE");
    
    free(action);
    
    // Return true if successful (non-zero result)
    return (result != 0);
}
