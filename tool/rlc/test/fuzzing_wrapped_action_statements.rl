import fuzzer.cpp_functions
import fuzzer.utils
import collections.vector

fun crash() {1 == 0}:
    return

ent Deck:
    Vector<Int> cards
    
    fun init():
        self.cards.init()
        let i = 1
        while i <= 13 :
            # once for each suit.
            self.cards.append(i)
            self.cards.append(i)
            self.cards.append(i)
            self.cards.append(i)
            i = i + 1
    
    fun switch_cards(Int a, Int b) {a >= 0, a < 52, b >= 0, b < 52}:
        let temp = self.cards.get(a)
        self.cards.set(a, self.cards.get(b))
        self.cards.set(b, temp)

fun draw(Deck deck, Vector<Int> hand):
    let card = deck.cards.pop()
    hand.append(card)

fun deal(Deck deck, Vector<Int> hand):
    let i = 0
    while i < 2:
        draw(deck, hand)
        i = i + 1

fun calculate_points(Vector<Int> hand) -> Int:
    let total = 0
    let num_ones = 0
    
    let i = 0
    while i < hand.size():
        let card = hand.get(i)
        if card <= 10:
            total = total + card
        else:
            total = total + 10
        
        if card == 1:
            num_ones = num_ones + 1
        i = i + 1

    while num_ones > 0 and total + 10 < 21:
        num_ones = num_ones - 1
        total = total + 10

    if num_ones == 2:
        crash()
    
    return total

act shuffle(ctx Deck deck) -> Shuffle:
    frm done = false
    #TODO delet dis. It's only here to showcase being able to use values from the underlying op's frame.
    frm last_picked_a = 0
    while !done:
        actions:
            act stop_shuffle()
            done = true

            act switch(Int a, Int b) {a >= last_picked_a, a < deck.cards.size(), b >= 0, b < 52}
            deck.switch_cards(a, b)
            last_picked_a = last_picked_a + 1

act play() -> Blackjack:
    frm deck : Deck
    frm player_hand : Vector<Int>
    frm dealer_hand : Vector<Int>

    subaction*(deck) s = shuffle(deck)

    deal(deck, player_hand)
    deal(deck, dealer_hand)

    frm player_passed = false
    frm player_bust = false

    while !player_bust and !player_passed:
        actions:
            act hit()
            draw(deck, player_hand)
            if calculate_points(player_hand) > 21:
                player_bust = true

            act stand()
            player_passed = true
    
    while calculate_points(dealer_hand) <= 16:
        draw(deck, dealer_hand)

#fun main() -> Int:
#    let game = play()
#    game.switch(0, 51)
#    game.switch(1, 50)
#    game.stop_shuffle()
#    if(game.player_hand.size() != 2):
#        return 1
#    if(game.dealer_hand.size() != 2):
#        return 1
#    game.hit()
#
#    if(!game.is_done()):
#        return 1
#    return 0
