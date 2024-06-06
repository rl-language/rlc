import collections.vector
import machine_learning 
import action

# A card is a integer from
# 0 to 13 included, where 
# 0 rappresents a jolly
# suits don't matter for
# back jack
using Card = BInt<0, 14>

# A card Index is the index
# of a card in the deck
# so a number from 0 to 52
using CardIndex = BInt<0, 52>

# A deck is a class, called
# entities in RL. It has 
# fields and methods
ent Deck:
    Vector<Card> cards

    # called every time
    # a deck object is created
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

    # Rearranges the location of two 
    # cards. It has some preconditions
    # to make sure the inputs are valid
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

# returns the score of a set of cards
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

# Here is the main implementation of the game
# the play function must be called play and 
# must return a Game, otherwise the machine
# learning components will not know what
# to look for.
act play() -> Game:

    # allocates a deck and initializes it
    # the deck is marked hidden, so that 
    # the content is not shown to the machine
    # learning
    frm deck : Hidden<Deck>

    # the player hands has a maximal size
    # and is not marked hidden, because the player
    # wants to see their own hand
    frm player_hand : BoundedVector<Card, 20>

    # the dealer hand is instead hidden.
    frm dealer_hand : Hidden<BoundedVector<Card, 20>>

    # shuffle the deck, we will see later 
    # how this is implemented
    subaction*(deck) shuffling = shuffle(deck.value)

    # deal the initial cards
    deal(deck.value, player_hand)
    deal(deck.value, dealer_hand.value)

    frm player_passed = false
    frm player_bust = false

    while !player_bust and !player_passed:
        actions:
            act hit()
            # declares the action hit, that allow the 
            # player to draw a card
              draw(deck.value, player_hand)
              if calculate_points(player_hand) > 21:
                player_bust = true

            act stand()
            # declares the action stand that ends the game
              player_passed = true

    # after the player has draw their cards, the
    # delear draws their automatically.
    while calculate_points(dealer_hand.value) <= 16:
        draw(deck.value, dealer_hand.value)

# this is action that implements
# the concept of shuffling a deck by
# rearranging two cards at random 100
# times. Of course this is not efficient
# it is implemented this way for
# teaching purposes.
act shuffle(ctx Deck deck) -> Shuffle:
    frm to_shuffle = 100
    while to_shuffle != 0:
        act shuffle_source(frm CardIndex a) 
        act shuffle_target(CardIndex b) {a.value != b.value}
        deck.switch_cards(a.value, b.value)
        to_shuffle = to_shuffle - 1

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
    if !g.shuffling.is_done():
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
   let deck = "deck: "s
   deck.append(to_string(g.deck))
   print(deck)
   let hand = "player hand: "s
   hand.append(to_string(g.player_hand))
   print(hand)
