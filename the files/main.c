#include "tf.h"
#include <curses.h>

int print_card(card_t * card)
{
	const char * name = info_card_name(card);
	cardtype_t type = info_card_type(card);
	int32_t lvl = info_card_lvl(card);
	int32_t cost = info_card_cost(card);
	const char * attr = info_card_attr(card);
	card_t * sub = info_card_sub(card);
	const char * sub_name = (sub) ? info_card_name(sub) : "Nono";
	printw("%s - %d/%d/%d - '%s' - %s\n",
			name, type, lvl, cost, attr, sub_name);
	return 0;
}
int32_t print_player(game_t * game, int32_t id)
{
	const char * name = info_name(game, id);
	printw("NAME: %s\n", name);
	int32_t hp = info_hp(game, id);
	int32_t hp_max = info_hp_max(game, id);
	int32_t def = info_def(game, id);
	int32_t def_max = info_def_max(game, id);
	int32_t power = info_power(game, id);
	int32_t power_max = info_power_max(game, id);
	int32_t threshold = info_threshold(game, id);
	int32_t trace = info_trace(game, id);
	printw("HP: %d/%d  DEF: %d/%d  PWR: %d/%d  TRE: %d  TRA: %d\n",
			hp, hp_max, def, def_max, power, power_max, threshold, trace);
	pile_t * hand = info_hand_pile(game, id);
	int32_t handC = 0;
	printw("P1 hand\n");
	foreachcard(card, hand)
	{
		printw("%d) ", handC++);
		print_card(card);
	}
	pile_t * used = info_used_pile(game, id);
	printw("P1 used\n");
	foreachcard(card, used)
		print_card(card);
	return handC;
}

int main()
{
	game_t * game = game_init("red_riding_hood", "red_riding_hood");
	initscr();
	cbreak();
	keypad(stdscr, TRUE);
	noecho();
	int32_t ch = 0;
	while (1)
	{
		clear();
		printw("INPUT ASCII: %d\n", ch);
		pile_t * hand = info_hand_pile(game, 1);
		int32_t handC = print_player(game, 1);
		print_player(game, 2);
		const char ** choices = info_choice(game);
		printw("Choices:\n");
		int32_t choiceC = 0;
		for (; choices && choices[choiceC]; choiceC++)
			printw("%d) %s\n", choiceC, choices[choiceC]);
		if (!info_activation(game))
		{
			printw("Press any to continue...\n");
			ch = getch();
			RUNNE(0, game_next, game);
		}
		else if (choiceC > 0)
		{
			printw("Choose a option...\n");
			ch = getch();
			if ('0' <= ch && ch - '0' < choiceC)
				RUNNE(0, game_choose, game, ch - '0');
		}
		else
		{
			printw("Choose a card to use...\n");
			ch = getch();
			if ('0' <= ch && ch - '0' < handC)
			{
				card_t * sel = info_pile_card(hand, ch - '0');
				RUNNE(0, game_use, game, sel);
			}
			if (ch == '\n')
				RUNNE(0, game_next, game);
		}
		refresh();
		if (ch == 27)
			break;
	}
	endwin();
	game_free(game);
	return 0;
}
