#ifndef SIMPLE_BOT_H
#define SIMPLE_BOT_H

#include "tf.h"
#include <stdbool.h>

typedef enum {
    SIMPLE_BOT_RANDOM,
    SIMPLE_BOT_AI
} SimpleBotType;

typedef struct {
    int player_id;
    SimpleBotType bot_type;
    game_t* game;
} SimpleBot;

// Initialize a simple bot
bool simple_bot_init(SimpleBot* bot, int player_id, SimpleBotType bot_type, game_t* game);

// Cleanup bot resources
void simple_bot_cleanup(SimpleBot* bot);

// Get the bot's next action as a string
char* simple_bot_get_action(SimpleBot* bot);

// Execute one bot turn using game_bot()
bool simple_bot_take_turn(SimpleBot* bot);

#endif // SIMPLE_BOT_H
