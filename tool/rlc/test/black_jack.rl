# RUN: python %pyscript/solve.py %s --stdlib %stdlib --rlc rlc
import collections.vector
import machine_learning
import action
import algorithms.diff
import python


using Card = BInt<0, 14>
using CardIndex = BInt<0, 52>

cls Hit:
    Int hit

cls Stand:
    Int stand

cls Deck:
    Vector<Card> cards

    fun init():
        self.cards.init()
        let i = 1
        while i <= 13:
            let card : Card
            card.value = i
            self.cards.append(card)
            self.cards.append(card)
            self.cards.append(card)
            self.cards.append(card)
            i = i + 1

    fun switch_cards(Int a, Int b) {a >= 0, a < 52, b >= 0, b < 52}:
        let temp = self.cards.get(a)
        self.cards.set(a, self.cards.get(b))
        self.cards.set(b, temp)

cls PlayerHandCallbacks:
    PyObject on_changed

cls PlayerHand:
    BoundedVector<Card, 20> cards
    PlayerHandCallbacks callback

fun draw(Deck deck, PlayerHand hand):
    let card = deck.cards.pop()
    hand.cards.append(card)

fun draw(Deck deck, BoundedVector<Card, 20> hand):
    let card = deck.cards.pop()
    hand.append(card)

fun deal(Deck deck, PlayerHand hand):
    let i = 0
    while i < 2:
        draw(deck, hand)
        i = i + 1

fun deal(Deck deck, BoundedVector<Card, 20> hand):
    let i = 0
    while i < 2:
        let card = deck.cards.pop()
        hand.append(card)
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

fun calculate_points(PlayerHand hand) -> Int:
    return calculate_points(hand.cards)

@classes
act play() -> Game:
    frm hit_button : Hit
    frm stand_button : Stand
    frm player_hand : PlayerHand
    frm deck : Hidden<Deck>
    frm dealer_hand : Hidden<BoundedVector<Card, 20>>

    subaction*(deck.value) shuffling = shuffle(deck.value)

    deal(deck.value, player_hand)
    deal(deck.value, dealer_hand.value)

    frm player_passed = false
    frm player_bust = false
    frm set_args : PyObject

    while !player_bust and !player_passed:
        actions:
            act hit()
              draw(deck.value, player_hand)
              set_args = to_pyobject(player_hand)
              player_hand.callback.on_changed.call("on_changed", set_args)
              if calculate_points(player_hand) > 21:
                player_bust = true
              else if calculate_points(player_hand) == 21:
                player_passed = true

            act stand()
              player_passed = true
              set_args = to_pyobject(player_hand)
              player_hand.callback.on_changed.call("on_changed", set_args)

    while calculate_points(dealer_hand.value) <= 16:
        draw(deck.value, dealer_hand.value)
    set_args = to_pyobject(player_hand)
    player_hand.callback.on_changed.call("on_changed", set_args)

act shuffle(ctx Deck deck) -> Shuffle:
    frm to_shuffle = 100
    while to_shuffle != 0:
        act shuffle_source(frm CardIndex a)
        act shuffle_target(CardIndex b) {a.value != b.value}
        deck.switch_cards(a.value, b.value)
        to_shuffle = to_shuffle - 1

fun get_current_player(Game g) -> Int:
    if g.is_done():
        return -4
    if !g.shuffling.is_done():
        return -1
    return 0

fun get_num_players() -> Int:
    return 1

fun score(Game g, Int player_id) -> Float:
    if !g.is_done():
        return 0.0
    let points = calculate_points(g.player_hand)
    if points > 21:
        return 0.0
    return float(points) / 21.0

fun pretty_print(Game g):
    let hand = "player hand: "s
    hand.append(to_string(g.player_hand))
    print(hand)

fun game_diff(Game before, Game after, Vector<String> out):
    diff(before, after, out)
