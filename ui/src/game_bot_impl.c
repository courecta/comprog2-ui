#include "tf.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Simple implementation of game_bot that the tf.o library expects
int32_t game_bot(game_t * game, const char * line) {
    if (!game || !line) return 0;
    
    printf("🎮 game_bot called with command: '%s'\n", line);
    
    // Handle quit command
    if (strcmp(line, "q") == 0) {
        printf("   Executing: game_next()\n");
        return game_next(game);
    }
    
    // Handle card selection (numbers)
    char *endptr;
    long card_index = strtol(line, &endptr, 10);
    
    if (*endptr == '\0' && card_index >= 0) {
        // It's a valid card index
        printf("   Executing: play card %ld\n", card_index);
        
        pile_t* hand = info_hand_pile(game, info_turn(game));
        if (!hand) {
            printf("   No hand available\n");
            return 0;
        }
        
        card_t* card = info_pile_card(hand, (int)card_index);
        if (!card) {
            printf("   Card %ld not found in hand\n", card_index);
            return 0;
        }
        
        printf("   Playing card: %s\n", info_card_name(card));
        return game_use(game, card);
    }
    
    // Handle choice selection
    if (strlen(line) == 1 && line[0] >= '0' && line[0] <= '9') {
        int choice = line[0] - '0';
        printf("   Executing: make choice %d\n", choice);
        return game_choose(game, choice);
    }
    
    // Unknown command
    printf("   Unknown command: '%s'\n", line);
    return 0;
}
