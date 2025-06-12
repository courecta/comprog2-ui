#pragma once
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#define OUT_STREAM stderr
#ifdef DEBUG
#define debug(fmt, ...) fprintf(OUT_STREAM, "\e[1m%s:%d:%s():\e[0m " fmt "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#define info(fmt, ...) debug("\e[1m[Info]\e[0m " fmt, ##__VA_ARGS__)
#define warning(fmt, ...) debug("\e[38;5;13m\e[1m[Warning]\e[22m " fmt "\e[0m", ##__VA_ARGS__)
#define fatal(fmt, ...) debug("\e[38;5;9m\e[1m[Fatal]\e[22m " fmt "\e[0m", ##__VA_ARGS__)
#else
#define debug(fmt, ...) 
#define info(fmt, ...) fprintf(OUT_STREAM, "\e[1m[Info]\e[0m " fmt "\n", ##__VA_ARGS__)
#define warning(fmt, ...) fprintf(OUT_STREAM, "\e[38;5;13m\e[1m[Warning]\e[22m " fmt "\e[0m\n", ##__VA_ARGS__)
#define fatal(fmt, ...) fprintf(OUT_STREAM, "\e[38;5;9m\e[1m[Fatal]\e[22m " fmt "\e[0m\n", ##__VA_ARGS__)
#endif

#define RUNEQ(rtn, fun, ...) ({\
	if (rtn != fun(__VA_ARGS__))\
		debug("\e[38;5;13m\e[1m[WARNING]\e[22m Wrong Run with %s()\e[0m", #fun);\
})
#define RUNNE(rtn, fun, ...) ({\
	if (rtn == fun(__VA_ARGS__))\
		debug("\e[38;5;13m\e[1m[WARNING]\e[22m Wrong Run with %s()\e[0m", #fun);\
})

#define RUNMEM(fun, dst, ...) ({\
	void * __rtnval = fun(dst, ##__VA_ARGS__);\
	if (NULL == __rtnval)\
		debug("\e[38;5;13m\e[1m[WARNING]\e[22m Wrong Run with %s()\e[0m", #fun);\
	else dst = __rtnval;\
})

#define writestr(fd, str) write(fd, str, strlen(str))
	
#define strend(S) (S[strlen(S) - 1])

// all the char * should be malloc before.

// a k str array is null terminate

// k str copy
// full copy `str` to `dst` and return `dst`
char * kstrcpy(char * dst, const char * str);
// k str interval copy
// full copy `str` until `end` to `dst` and return `dst`
char * kstricpy(char * dst, char * str, char * end);
// k str n copy
// full copy `str` up to `n` char to `dst` and return `dst`
char * kstrncpy(char * dst, const char * str, size_t n);
// k str append
// append `str` to `dst` and return `dst`
char * kstrappend(char * dst, char * str);
// k str array length
ssize_t kstrarrlen(char ** ptr);
// k str array free
void kstrarrfree(char ** ptr);
// k str split
// split `str` by `del` and return a k str array
char ** kstrsplit(char ** ptr, char * str, char * del);
// k str replace
// replace `tar` in `str` by `rep` and return `dst`
char * kstrreplace(char * dst, char * str, char * tar, char * rep);
// k str gets until
// get string from `fd` until any char in `end` or EOF,
// store it into `dst` and return `dst`
// if `fd` can no longer be read, a free is dont for `dst` and return NULL
char * kstrgetsuntil(char * dst, int32_t fd, char * end);
// k str gets
// get string from 'fd' until ' ', '\n', '\t' or EOF,
// store it into `dst` and return `dst`
char * kstrgets(char * dst, int32_t fd);
// k str get line
// get string from `fd` until '\n' or EOF,
// store it into `dst` and return `dst`
char * kstrgetline(char * dst, int32_t fd);
// k str is int
// return 1 if `str` is int, otherwise 0
uint8_t kstrisint(char * str);
// k str to int
// transform `str` to integer and store it into `dst`
// return 1 if success, otherwise 0
uint8_t kstrtoint(int64_t * dst, char * str);
// k str int to
// transform `i` into str, store it into `dst` and return `dst`
char * kstrintto(char * dst, int64_t i);
// k str format time
// transform `t` into str of time, store it into `dst` and return `dst`
char * kstrftime(char * dst, int64_t t);
// k str reverse
// reverse `str`, store it into `dst` and return `dst`
char * kstrrev(char * dst, char * str);
// k str strip
// strip `stp` from `str`, store it into `dst` and return `dst`
char * kstrstrip(char * dst, const char * str, const char * stp);
// k str arr append
// append `str` to `dst` and return `dst`
char ** kstrarrappend(char ** dst, char * str);
// k str reverse span
// count how many continuous bytes of char of `str` is in `accept` and return it
// return -1 when fail
ssize_t kstrrspn(const char * str, const char * accept);
// lower all character in `str` and return `str`
char * kstrlower(char * dst, char * str);
//
char * kstrbind(char * dst, const char * fmt, ...);


typedef enum _cardtype_t
{
	CT_MISC	      = 0b0,
	CT_ATK        = 0b1,
	CT_DEF        = 0b10,
	CT_MOVE       = 0b100,
	CT_WILD       = 0b111,
	CT_SKILL_ATK  = 0b1000,
	CT_SKILL_DEF  = 0b10000,
	CT_SKILL_MOVE = 0b100000,
	CT_SKILL      = 0b111000,
	CT_TWIST      = 0b1000000,
	CT_EPIC       = 0b10000000,
	CT_ALL        = 0b11111111,
	CT_MAX
} cardtype_t;

#define CARD_TYPE(a, b) ((a & b) == b)

typedef struct _card_t card_t;

card_t * card_init(const char * card);
card_t * card_creat(const char * name, cardtype_t type, int32_t lvl, int32_t cost, char * func, char * attr);
void card_free(card_t * card);
void card_print(const card_t * card);

cardtype_t card_type_decode(const char * str);

const char * info_card_name(const card_t * card);
cardtype_t info_card_type(const card_t * card);
int32_t info_card_lvl(const card_t * card);
int32_t info_card_cost(const card_t * card);
const char * info_card_func(const card_t * card);
const char * info_card_attr(const card_t * card);
card_t * info_card_sub(const card_t * card);


#define PRECAT3(a, b, c) (a##b##c)
#define CONCAT3(a, b, c) PRECAT3(a, b, c)

typedef struct _pilenode_t pilenode_t;
typedef struct _pile_t pile_t;

#define foreachcard(name, pile) \
	pilenode_t * CONCAT3(__node, __LINE__, __) = (pile) ? pile_top(pile) : NULL;\
	for (card_t * name = NULL;\
	CONCAT3(__node, __LINE__, __) && (name = pilenode_card(CONCAT3(__node, __LINE__, __)));\
	CONCAT3(__node, __LINE__, __) = pilenode_down(CONCAT3(__node, __LINE__, __)))

pilenode_t * pilenode_init(card_t * card, pilenode_t * up, pilenode_t * down);
void pilenode_free(pilenode_t * node);

pile_t * pile_init(const char * card, ...);
pile_t * pile_init_count(const char * card, int32_t count);
int32_t pile_add_top(pile_t * pile, const char * card);
int32_t pile_move_top(pile_t * from, pile_t * to, int32_t count);
int32_t pile_draw(pile_t * from, pile_t * to, int32_t count);
void pile_print(pile_t * pile);
void pile_free(pile_t * pile);

pilenode_t * pile_top(const pile_t * pile);
pilenode_t * pilenode_down(const pilenode_t * pilenode);
card_t * pilenode_card(const pilenode_t * pilenode);

card_t * info_pile_card(const pile_t * pile, int32_t index);

pilenode_t * pile_find_node(const pile_t * pile, const card_t * card);
int32_t pile_remove_node(pile_t * pile, const pilenode_t * node);

typedef struct _player_t player_t;
typedef struct _game_t game_t;

typedef enum _action_t
{
	ACT_FOCUS  = 0b1,
	ACT_USE    = 0b10,
	ACT_BUY    = 0b100,
	ACT_PLAYER = 0b1000,
	ACT_MAX
} action_t;
typedef enum _state_t
{
	ST_START,
	ST_CLEAR,
	ST_ACT,
	ST_END,
	ST_MAX
} state_t;
typedef enum _usage_t
{
	U_PLAY,
	U_WITH,
	U_BASE,
	U_MAX
} usage_t;

#define ACTION(a, b) ((a & b) == b)

typedef enum _shop_t
{
	SP_ATK,		SP_ATK1,	SP_ATK2,	SP_ATK3,
	SP_DEF,		SP_DEF1,	SP_DEF2,	SP_DEF3,
	SP_MOVE,	SP_MOVE1,	SP_MOVE2,	SP_MOVE3,
	SP_WILD,
	SP_P1,		SP_P11,		SP_P12,		SP_P13,
	SP_P2,		SP_P21,		SP_P22,		SP_P23,
	SP_P3,		SP_P31,		SP_P32,		SP_P33,
	SP_P4,		SP_P41,		SP_P42,		SP_P43,
	SP_MAX
} shop_t;

player_t * player_init(const char * name);
player_t * player_creat(int32_t hp_max, int32_t def_max, int32_t threshold);
void player_free(player_t * player);
void player_print(player_t * player);

game_t * game_init(const char * p1, const char * p2);
game_t * game_inti4(const char * p1, const char * p2, const char * p3, const char * p4);
void game_free(game_t * game);
void game_print(game_t * game);

int32_t info_turn(const game_t * game);
int32_t info_showcase(const game_t * game);
int32_t info_activation(const game_t * game);
action_t info_act(const game_t * game);

int32_t info_hp(const game_t * game, int32_t player);
int32_t info_hp_max(const game_t * game, int32_t player);
int32_t info_def(const game_t * game, int32_t player);
int32_t info_def_max(const game_t * game, int32_t player);
int32_t info_threshold(const game_t * game, int32_t player);
int32_t info_token(const game_t * game, int32_t player);
int32_t info_token_max(const game_t * game, int32_t player);
int32_t info_power(const game_t * game, int32_t player);
int32_t info_power_max(const game_t * game, int32_t player);
int32_t info_trace(const game_t * game, int32_t player);
const char * info_name(const game_t * game, int32_t player);
const char ** info_choice(const game_t * game);
int32_t info_choice_count(const game_t * game);
pile_t * info_hand_pile(const game_t * game, int32_t player);
pile_t * info_deck_pile(const game_t * game, int32_t player);
pile_t * info_used_pile(const game_t * game, int32_t player);
pile_t * info_disc_pile(const game_t * game, int32_t player);
pile_t * info_remv_pile(const game_t * game, int32_t player);
pile_t * info_epic_pile(const game_t * game, int32_t player);
pile_t * info_shop_pile(const game_t * game, shop_t shop);

card_t * info_show_card(const game_t * game);

int32_t game_next(game_t * game);
int32_t game_use(game_t * game, const card_t * card);
int32_t game_choose(game_t * game, int32_t n);
int32_t game_buy(game_t * game, shop_t shop);
int32_t game_player(game_t * game, int32_t player);
int32_t game_focus(game_t * game);
