#include "tf.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* generate_prompt(game_t *g) {
    char *prompt = malloc(8192);
    if (!prompt) return NULL;
    prompt[0] = '\0';

    char buffer[512];
    int32_t turn = info_turn(g);
    snprintf(buffer, sizeof(buffer), "[Your Turn] Player %d\n", turn);
    strcat(prompt, buffer);

    const char *name = info_name(g, turn);
    int32_t hp = info_hp(g, turn);
    int32_t maxhp = info_hp_max(g, turn);
    int32_t def = info_def(g, turn);
    int32_t maxdef = info_def_max(g, turn);
    int32_t power = info_power(g, turn);
    int32_t maxpower = info_power_max(g, turn);
    int32_t threshold = info_threshold(g, turn);
    int32_t trace = info_trace(g, turn);

    snprintf(buffer, sizeof(buffer),
             "NAME: %s\nHP: %d/%d  DEF: %d/%d  PWR: %d/%d  TRE: %d  TRA: %d\n",
             name, hp, maxhp, def, maxdef, power, maxpower, threshold, trace);
    strcat(prompt, buffer);

    strcat(prompt, "P1 hand\n");
    pile_t *hand = info_hand_pile(g, turn);
    for (int32_t i = 0; i < info_pile_size(hand); ++i) {
        card_t *card = info_pile_card(hand, i);
        int32_t id = info_card_id(card);
        const char *name = info_card_name(card);
        cardtype_t type = info_card_type(card);
        int32_t lvl = info_card_lvl(card);
        int32_t cost = info_card_cost(card);
        const char *attr = info_card_attr(card);
        card_t *sub = info_card_sub(card);
        const char *sub_name = sub ? info_card_name(sub) : "None";
        snprintf(buffer, sizeof(buffer),
                 "%d) %d - %s - %d/%d/%d - '%s' - %s\n",
                 i, id, name, type, lvl, cost, attr ? attr : "", sub_name);
        strcat(prompt, buffer);
    }

    strcat(prompt, "P1 used\n\n");
    pile_t *used = info_used_pile(g, turn);
    for (int32_t i = 0; i < info_pile_size(used); ++i) {
        card_t *card = info_pile_card(used, i);
        int32_t id = info_card_id(card);
        const char *name = info_card_name(card);
        cardtype_t type = info_card_type(card);
        int32_t lvl = info_card_lvl(card);
        int32_t cost = info_card_cost(card);
        const char *attr = info_card_attr(card);
        card_t *sub = info_card_sub(card);
        const char *sub_name = sub ? info_card_name(sub) : "None";
        snprintf(buffer, sizeof(buffer),
                 "%d - %s - %d/%d/%d - '%s' - %s\n",
                 id, name, type, lvl, cost, attr ? attr : "", sub_name);
        strcat(prompt, buffer);
    }

    int32_t opp = (turn == 1) ? 2 : 1;
    name = info_name(g, opp);
    hp = info_hp(g, opp);
    maxhp = info_hp_max(g, opp);
    def = info_def(g, opp);
    maxdef = info_def_max(g, opp);
    power = info_power(g, opp);
    maxpower = info_power_max(g, opp);
    threshold = info_threshold(g, opp);
    trace = info_trace(g, opp);
    strcat(prompt, "Player 2\n");
    snprintf(buffer, sizeof(buffer),
             "NAME: %s\nHP: %d/%d  DEF: %d/%d  PWR: %d/%d  TRE: %d  TRA: %d\n",
             name, hp, maxhp, def, maxdef, power, maxpower, threshold, trace);
    strcat(prompt, buffer);

    strcat(prompt, "\nShop Info\n");

    // 玩家專屬商店 (3 個)
    for (int i = 0; i < 3; i++) {
        int32_t idx = SP_P11 - 4 + 4 * turn + i;
        pile_t *shop = info_shop_pile(g, idx);
        card_t *card = info_pile_card(shop, 0);
        if (card) {
            int32_t id = info_card_id(card);
            const char *name = info_card_name(card);
            cardtype_t type = info_card_type(card);
            int32_t lvl = info_card_lvl(card);
            int32_t cost = info_card_cost(card);
            const char *attr = info_card_attr(card);
            card_t *sub = info_card_sub(card);
            const char *sub_name = sub ? info_card_name(sub) : "None";
            snprintf(buffer, sizeof(buffer),
                     "%d) %d - %s - %d/%d/%d - '%s' - %s\n",
                     i, id, name, type, lvl, cost, attr ? attr : "", sub_name);
            strcat(prompt, buffer);
        }
    }

    // 基本牌商店 (從 SP_ATK1 開始)
    const int base_shop[] = {
        SP_ATK1, SP_ATK2, SP_ATK3,
        SP_DEF1, SP_DEF2, SP_DEF3,
        SP_MOVE1, SP_MOVE2, SP_MOVE3,
        SP_WILD
    };

    const char *shop_index = "3456789abc";  // 符合你給的 a~c 編碼

    for (int i = 0; i < 10; i++) {
        pile_t *shop = info_shop_pile(g, base_shop[i]);
        card_t *card = info_pile_card(shop, 0);
        if (card) {
            int32_t id = info_card_id(card);
            const char *name = info_card_name(card);
            cardtype_t type = info_card_type(card);
            int32_t lvl = info_card_lvl(card);
            int32_t cost = info_card_cost(card);
            const char *attr = info_card_attr(card);
            card_t *sub = info_card_sub(card);
            const char *sub_name = sub ? info_card_name(sub) : "None";
            snprintf(buffer, sizeof(buffer),
                     "%c) %d - %s - %d/%d/%d - '%s' - %s\n",
                     shop_index[i], id, name, type, lvl, cost, attr ? attr : "", sub_name);
            strcat(prompt, buffer);
        }
    }

    strcat(prompt, "d) cancel\n");

    const char **choices = info_choice(g);
    if (choices && choices[0]) {
        strcat(prompt, "\nChoices:\n");
        for (int i = 0; choices[i]; ++i) {
            snprintf(buffer, sizeof(buffer), "%d) %s\n", i, choices[i]);
            strcat(prompt, buffer);
        }
    }

    return prompt;
}
