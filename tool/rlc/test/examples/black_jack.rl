import collections.vector
import machine_learning 
import action

using Card = BInt<0, 14>
using CardIndex = BInt<0, 52>

ent Deck:
    Vector<Card> cards

    fun init():
        self.cards.init()
        let i = 1
        while i <= 13 :
            let card : Card 
            card.value = i
            # once for each suit.
            self.cards.append(card)
            self.cards.append(card)
            self.cards.append(card)
            self.cards.append(card)
            i = i + 1

    fun switch_cards(Int a, Int b) {a >= 0, a < 52, b >= 0, b < 52}:
        let temp = self.cards.get(a)
        self.cards.set(a, self.cards.get(b))
        self.cards.set(b, temp)

fun draw(Deck deck, BoundedVector<Card, 20> hand):
    let card = deck.cards.pop()
    hand.append(card)

fun deal(Deck deck, BoundedVector<Card, 20> hand):
    let i = 0
    while i < 2:
        draw(deck, hand)
        i = i + 1

fun calculate_points(BoundedVector<Card, 20> hand) -> Int:
    let total = 0
    let num_ones = 0

    let i = 0
    while i < hand.size():
        let card = hand.get(i)
        if card <= 10:
            total = total + card.value
        else:
            total = total + 10

        if card == 1:
            num_ones = num_ones + 1
        i = i + 1

    while num_ones > 0 and total + 10 < 21:
        num_ones = num_ones - 1
        total = total + 10

    return total

act shuffle(ctx Deck deck) -> Shuffle:
    frm to_shuffle = 100
    while to_shuffle != 0:
        act shuffle_source(frm CardIndex a) 
        act shuffle_target(CardIndex b) {a.value != b.value}
        deck.switch_cards(a.value, b.value)
        to_shuffle = to_shuffle - 1

act play() -> Game:
    frm deck : Hidden<Deck>
    frm player_hand : BoundedVector<Card, 20>
    frm dealer_hand : Hidden<BoundedVector<Card, 20>>
    frm done_shuffling = false

    subaction*(deck) shuffling = shuffle(deck.value)
    done_shuffling = true

    deal(deck.value, player_hand)
    deal(deck.value, dealer_hand.value)

    frm player_passed = false
    frm player_bust = false

    while !player_bust and !player_passed:
        actions:
            act hit()
            draw(deck.value, player_hand)
            if calculate_points(player_hand) > 21:
                player_bust = true

            act stand()
            player_passed = true

    while calculate_points(dealer_hand.value) <= 16:
        draw(deck.value, dealer_hand.value)


# When using the machine learning layer
# delivered by the rlc package you need
# to provide a function that give a game
# returns the current player.
fun get_current_player(Game g) -> Int:
    # -4 is a special number to specify
    # that the game is done.
    if g.is_done():
        return -4
    # -1 is a special number to specify
    # that the next action must be taken
    # randomly
    if !g.done_shuffling:
        return -1 
    
    # the first player has id 0.
    return 0

# We need to specify the maximal number
# of players, so that the machine learning
# can configure itself with the correct
# number of agents
fun get_num_players() -> Int:
    return 1

# To avoid games that never terminate, we provide 
# a function that suggest a maximal number of actions
# 200 is greatly overestimated.
fun max_game_lenght() -> Int:
    return 300

# For the machine learning to learn we need 
# to specify the score of the given player, 
# at a given game state.
# 
# For the current game we return 1.0 if it
# solved the board, 0.0 if it placed no number
# and we interpolate between the two depending 
# on how many number it has managed to write 
# otherwise
fun score(Game g, Int player_id) ->  Float:
    if !g.is_done():
        return 0.0
    let points = calculate_points(g.player_hand)
    if points > 21:
        return 0.0
    return float(points) / 21.0

# This function must be present just as written
# to make sure that some functions used by 
# machine learning are available.
fun gen_printer_parser():
    let state : Game
    let any_action :  AnyGameAction
    gen_python_methods(state, any_action)

# this function is looked for by the machine
# learning components when they wish to print
# a human readable version of the game, so 
# that the user may understand what is going on
fun pretty_print(Game g):
   let i = 0
   let to_print : String
   while i != g.player_hand.size():
      to_print.append(to_string(g.player_hand.get(i).value))
      i = i + 1
   print(to_print)
