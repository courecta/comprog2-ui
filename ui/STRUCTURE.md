Define Items

```
// this is the game structure
typedef /* ... */ game_t;
// this is a card pile, a link-list for short
typedef /* ... */ pile_t;
typedef /* ... */ pilenode_t;
// this stores a card's info
typedef /* ... */ card_t;
// these are enums only for readibility
typedef /* ... */ action_t;
typedef /* ... */ cardtype_t;
typedef /* ... */ shop_t;
```

---

Here are some useful stuff that I define for readibility. Here are some of them.

Card Type (cardtype_t)

CT_ATK, CT_DEF, CT_MOVE, CT_WILD, CT_SKILL, CT_TWIST, CT_NONE
These macros allow you to apply OR operation (|) on them with the information preserved.
CARD_TYPE(a, b)
This macro checks whether a is of the same type of b
That is, b belongs to a

---

Action (action_t)

ACT_FOCUS, ACT_USE, ACT_CHOOSE, ACT_BUY, ACT_PLAYER
These macros allow you to apply OR operation (|) on them with the information preserved.
ACTION(a, b)
This macro checks whether a allows us to do b action.

---

Shop (shop_t)

SP_ATK, SP_ATK1, SP_ATK2, SP_ATK3,
SP_DEF, SP_DEF1, SP_DEF2, SP_DEF3,
SP_MOVE, SP_MOVE1, SP_MOVE2, SP_MOVE3,
SP_WILD,
SP_P1, SP_P11, SP_P12, SP_SP13
SP_P2, SP_P21, SP_P22, SP_SP23
SP_P3, SP_P31, SP_P32, SP_SP33
SP_P4, SP_P41, SP_P42, SP_SP43
The ones with numbers are that much more then the ones without numbers. For example, SP_ATK2 = SP_ATK + 2
And the one indicates players are 4 more then the previous. For example, SP_P2 = SP_P1 + 4

---

For loop

foreachcard(card, pile)
This macro expand to a for loop that loop through all the card(s) in the pile.

---

Initialization

```
// initialize a game with characters' names regardless of case.
// p1 goes first.
game_t * game_init(const char * p1, const char * p2);
// initialize a game with characters' names. ODD and EVEN are two teams.
game_t * game_init4(const char * p1, const char * p2,
                    const char * p3, const char * p4);
// free the memories.
void game_free(game_t * game);

// initialize a card pile with cards, from the button to the top. null-terminating.
pile_t * pile_init(const char * card, ...);
// initialize a card pile with `count` same cards.
pile_t * pile_init_count(const char * card, int32_t count);
// add cards on top of the pile by the name of the card.
int32_t pile_add_top(pile_t * pile, const char * card);
// free the memories.
void pile_free(pile_t * pile);
// initialize a card by it's name.

card_t * card_init(const char * card);
// initialize a card by it's value.
card_t * card_creat(const char * name, cardtype_t type, int32_t lvl,
                    int32_t cost, const char * func, const char * attr);
// free the memories.
void card_free(card_t * card);
```

---

Info Related

```
// return whose turn is this (1 to 4). if fail, a 0 is returned.  
int32_t info_turn(const game_t * game);
// return 0 if no showcase now.
int32_t info_showcase(const game_t * game);
// return 0 if it's not at activation phase
// use game_next() to continue if it's not at activation phase
int32_t info_activation(const game_t * game);
// return what actions can be done now.
action_t info_act(const game_t * game);
int32_t info_hp(const game_t * game, int32_t player);
int32_t info_hp_max(const game_t * game, int32_t player);
int32_t info_def(const game_t * game, int32_t player);
int32_t info_def_max(const game_t * game, int32_t player);
int32_t info_threshold(const game_t * game, int32_t player);
// return the count of the token of the player, -1 if no token skill
int32_t info_token(const game_t * game, int32_t player);
int32_t info_token_max(const game_t * game, int32_t player);
// return the power of the player
int32_t info_power(const game_t * game, int32_t player);
int32_t info_power_max(const game_t * game, int32_t player);
// two lanes, positive and negative
// the absolute value is the index of the trace
// return 0 if fail
int32_t info_trace(const game_t * game, int32_t player);
// return the name of the choices
// this string array is null-terminated
const char ** info_choice(const game_t * game);
// return the number of the choices
int32_t info_choice_count(const game_t * game);

// return the hand pile of the player.
pile_t * info_hand_pile(const game_t * game, int32_t player);
pile_t * info_deck_pile(const game_t * game, int32_t player);
pile_t * info_used_pile(const game_t * game, int32_t player);
// return the discard pile of the player.
pile_t * info_disc_pile(const game_t * game, int32_t player);
// return the remove pile of the player.
pile_t * info_remv_pile(const game_t * game, int32_t player);
// return the epic pile of the player.
pile_t * info_epic_pile(const game_t * game, int32_t player);
// return the shop pile of the shop.
pile_t * info_shop_pile(const game_t * game, shop_t shop);

// return the card on the showcase now.
card_t * info_show_card(const game_t * game);

// return the size of the pile
int32_t info_pile_size(const pile_t * pile);
// return the `index`-th card of the pile
card_t * info_pile_card(const pile_t * pile, int32_t index);

// return the name of the card.
const char * info_card_name(const card_t * card);
// return the type of the card.
cardtype_t info_card_type(const card_t * card);
// return the level of the card.
int32_t info_card_lvl(const card_t * card);
// return the cost of the card.
int32_t info_card_cost(const card_t * card);
// return the function description of the card. remember to free the str.
const char * info_card_func(const card_t * card);
// return the attribute description of the card. remember to free the str.
const char * info_card_attr(const card_t * card);
card_t * info_card_sub(const card_t * card);
```

---

Game Related

These functions return 0 if failed.

```
// going forward to the next step.
// this is for showcasing perpose.
// return 0 if it's at activation phase.
int32_t game_next(game_t * game);
// use the `index`-th card of the player's hand pile.
int32_t game_use(game_t * game, const card_t * card);
// choose from a range given by info_choice() and info_choice_count()
// n is the index from 0 to info_choice_count()
int32_t game_choose(game_t * game, int32_t n)
// buy the first card of on `shop`
int32_t game_buy(game_t * game, shop_t shop);
// choose a player
int32_t game_player(game_t * game, int32_t player);
// focus
int32_t game_focus(game_t * game);
```

---

Card File Structure

```
<id>
<name>
<type> (the enum cardtype_t is available)
<level>
<cost>
<attributes> (characters)
<description>
<function line>
<function line>
...
<function line>
```

<attribute> is a string that contain different characters:

    e inexhaustible (won't be moved to used pile after the usage)
    f freezed (stay across round)
    p purge (removed after use)
    P completely purge (purge and won't be moved to remove pile)
    r unremovable
    u unplayable
    w wither (completely purge after discard)

---

Card Function Line (Command)

```
print <string>
    space is allowed to be inside the <string>
base_atk <amount: int>
    base attack add <amount>
base_def <amount: int>
    base defense add <amount>
base_move <amount: int>
    base movement add <amount>
base_wild <amount: int>
    base wildcard add <amount>
with <cardtype>
    ask for a card for sacrifice and store it's level into the escape code $l
aim <range: value> <any | self | ally | team | enemy>
    aim the player in <range> to $p
atk <amount: value>
    attack <amount> to the player aimed $p
def <amount: value>
    increase defense <amount>
move <amount: value>
    move <amount> tiles
card_transform <card name>
    change the card pointed to by the escape code $c to <card name>
sub_to_hand
    release the data stored in the card $c as a card name into hand
hand_to_sub
    store the data into the card $c by choosing a card in hand (can be cancled)
hand_to_sub_no <no: value>
    store the n-th card in hand to the card $c
discard <cardtype> <damage | defense | range> <count: value>
    discard upto <count> <cardtype> cards and add there level to the next
    attack or reduce next attack taked
knockback <amount: value>
    knockback the player aimed $p in <range> upto <amount> tiles
repeat <cardtype>
    if the previous action is of <cardtype> repeat it
move_card <from: pile> <to: pile>
    move the card on the top of <from> to <to>
force_discard <amount: value>
    the player aimed $p must discard <amount> cards in their hand
sp_add <amount: value> <pile: string>
    move <amount> cards from your own special deck
    into the <pile> of the player aimed $p
poison <amount: value>
    raise a choice, move up to <amount> cards from your own special deck
    into the discard pile of the player aimed $p
teleport <range: value>
    move yourself into the <range> spaces of the player aimed $p.
red_card <count: value>
    the player aimed will discard all of her hand, and draw <count> cards.
send_back <count: value>
    send up to <count> cards of the player aimed back into their deck pile.
matching <count: value>
    raise a choice, move up to <count> cards from your own special deck
    onto the deck of the player aimed $p

[trigger condition (this need to be at the beginning of the line)]
ongoing
    trigger at the start of the round themselives
at_start
    trigger at the start of everyone's round
at_end
defeated <any | self | ally | team | enemy> <functions>
    trigger when the corresponding player is defeated
obtain <any | card name> <functions>
    trigger when buy a card with the corresponding name.
    this point the escape code $c to the card obtained.
use
    trigger when use a card
lose_hp
    active when losing hp
do_damage <amount: value> <card type>
    when ever a <card type> card do at least <amount> damage to
    the player aimed $p, do the function.
hurt
    hp reduced by being attacked.

[if <condition> <function>]
data
    if the card contain
2v2
    if it's 2v2 mode
cardtype <cardtype>
    if the card type of card $c is <cardtype>
> <a: value> <b: value>
    if a > b
<
= or ==
>=
<=
!=
```

Note: if it's a base_ use NULL to point the end of a round.

---

Card Function Line Escape Code

```
<int>
    the literal value
$l
    the level of the card witch is sacrifice with the 'with' function
$+<a: value>&<b: value>
    the sum of <a> and <b>
$*<a: value>&<b: value>
    the product of <a> and <b>
$/<a: value>&<b: value>
    the qoution of <a> / <b>
$c
    the card that trigger the function
$d
    the defense value of yourself
$D
    the defense value of the player
$p
    the player aimed
$s
    your self
$i<id: int32_t>
    count the cards of <id> in your own discard pile
$I<id: int32_t>
    count the cards of <id> in the discard pile of the player aimed $p
$m
    number of match card retuned to Match Deck
$M
    number of match card discarded from the deck of the player aimed $p
$o
    the power of yourself
$O
    the power of the player aimed $p
$h
    the hp traded
$w
    the power traded
$t
    token spent.
```

---

Character File Structure

```
<name>
<max hp>
<max def>
<threshold>
<shop1>
<shop2>
<shop3>
<epic>
<special>
<function line>
...
<function line>
```

<shop-n> and <epic> and <special> are multiline data. each line is a card's name, and end with NULL.
<function line> are card functions with trigger condition.

*NOTE:

- There is a card of the match girl that has no card face! (Currently numbered 1000) because of the action related to her special rules. This is a reminder, otherwise it won't be able to be read and it may crash.