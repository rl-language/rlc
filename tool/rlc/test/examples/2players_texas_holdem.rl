# RUN: python %pyscript/solve.py %s --stdlib %stdlib --rlc rlc
import collections.vector
import action

enum Suit:
    hearts
    diamonds
    clubs
    spades

    fun equal(Suit other) -> Bool:
        return self.value == other.value 

cls Card:
    BInt<1, 15> rank
    Suit suit

    fun equal(Card other) -> Bool:
        return self.rank == other.rank and self.suit == other.suit

    fun not_equal(Card other) -> Bool:
        return !self.equal(other) 

using Deck = Vector<Card>

cls Player:
    BInt<0, 101> chips
    BInt<0, 101> current_bet 
    BoundedVector<Card, 5> cards
    
    fun hand_value() -> Int:
        # ToDo, it should map the cards
        # to a score 
        return 0

fun make_deck() -> Deck:
    let deck : Deck
    let suits = enumerate(Suit::hearts)
    let index = 0
    while index != suits.size():
        let card_value = 1
        while card_value != 14:
            let card : Card
            card.rank = card_value
            card.suit = suits.get(index)
            deck.append(card)
            card_value = card_value + 1
        index = index + 1
    return deck

using DeckCardIndex = BInt<0, 52>
using Bet = BInt<0, 52>

act deal_cards(ctx Deck deck, ctx BoundedVector<Card, 5> cards, frm Int num_to_deal) -> Deal:
    while num_to_deal != 0:
        act deal(DeckCardIndex card) { card < deck.size() and card >= 0 }
        cards.append(deck.get(card.value))
        deck.erase(card.value) 
        num_to_deal = num_to_deal - 1

act _wrapped_deal_carts(Int num_to_deal) -> WrappedDeal:
    frm deck = make_deck()
    frm cards : BoundedVector<Card, 5>
    subaction*(deck, cards) _ = deal_cards(deck, cards, num_to_deal)

act _multi_deal_wrapped_deal_carts(frm Int num_to_deal) -> MultiDealWrappedDeal:
    frm deck = make_deck()
    frm cards : BoundedVector<Card, 5>
    subaction*(deck, cards) _ = deal_cards(deck, cards, num_to_deal)
    subaction*(deck, cards) _ = deal_cards(deck, cards, num_to_deal)

fun test_deal_cards() -> Bool:
    let dealing = _wrapped_deal_carts(2)
    let expected_dealt_card = dealing.deck.get(0)
    let expected_dealt_card2 = dealing.deck.get(1)

    let card_index : DeckCardIndex
    card_index = 0
    dealing.deal(card_index)
    card_index = 0
    dealing.deal(card_index)

    if dealing.cards.get(0) != expected_dealt_card:
        return false

    if dealing.cards.get(1) != expected_dealt_card2:
        return false

    if !dealing.is_done():
        return false

    if dealing.deck.size() != 50:
        return false

    return true

fun test_multi_deal_cards() -> Bool:
    let dealing = _multi_deal_wrapped_deal_carts(2)
    let expected_dealt_card = dealing.deck.get(0)
    let expected_dealt_card2 = dealing.deck.get(1)
    let expected_dealt_card3 = dealing.deck.get(2)
    let expected_dealt_card4 = dealing.deck.get(3)

    let card_index : DeckCardIndex
    dealing.deal(card_index)
    dealing.deal(card_index)
    dealing.deal(card_index)
    dealing.deal(card_index)

    if dealing.cards.get(0) != expected_dealt_card:
        return false

    if dealing.cards.get(1) != expected_dealt_card2:
        return false

    if dealing.cards.get(2) != expected_dealt_card3:
        return false

    if dealing.cards.get(3) != expected_dealt_card4:
        return false

    if !dealing.is_done():
        return false

    if dealing.deck.size() != 48:
        return false

    return true
    
fun other_player(Int player) -> Int:
    if player == 0:
        return 1
    return 0

act betting_round(ctx Player[2] players) -> BettingRound:
    frm folded = false
    frm current_player = 0
    while !folded:
        # if someone is all in, do nothing
        if players[0].current_bet == players[0].chips or players[1].current_bet == players[1].chips:
            return
        actions:
            act fold()
                folded = true
            act raise_bet(Bet quantity) {
                players[current_player].current_bet + quantity.value >= players[other_player(current_player)].current_bet,
                players[current_player].current_bet + quantity.value <= players[current_player].chips,
                quantity > 0
            }
                players[current_player].current_bet = players[current_player].current_bet + quantity.value
                # if we bet to exactly the other player bet, we are done
                if players[current_player].current_bet == players[other_player(current_player)].current_bet:
                    return
                current_player = other_player(current_player)
    
    # if we are here, it means current player folded 
    players[current_player].chips = players[current_player].chips - players[current_player].current_bet
    let other = other_player(current_player)
    players[other].chips = players[other].chips - players[other].current_bet
            

fun preflop(Player[2] players):
    players[0].current_bet = 1
    players[1].current_bet = 1

fun resolve_winner(Player[2] players):
    if players[0].hand_value() > players[1].hand_value():
        players[0].chips =  players[0].chips + players[1].current_bet
        players[1].chips =  players[1].chips - players[1].current_bet
    else:
        players[0].chips =  players[0].chips - players[1].current_bet
        players[1].chips =  players[1].chips + players[1].current_bet

@classes
act play() -> Game:
    frm players : Player[2] 
    players[0].chips = 100
    players[1].chips = 100 

    frm max_round_count = 5
    while players[0].chips != 0 and players[0].chips != 0 and max_round_count != 0:
        max_round_count = max_round_count - 1
        players[0].cards.clear()
        players[1].cards.clear()
        frm deck = make_deck()
        frm face_up_cards : BoundedVector<Card, 5>
        frm i = 0
        while i != 2:
            subaction*(deck, players[i].cards) _ = deal_cards(deck, players[i].cards, 2)
            i = i + 1

        subaction*(deck, face_up_cards) _ = deal_cards(deck, face_up_cards, 3)

        preflop(players)
        subaction*(players) bet = betting_round(players)
        if bet.folded:
            continue
        subaction*(deck, face_up_cards) _ = deal_cards(deck, face_up_cards, 1)
        subaction*(players) bet = betting_round(players)
        if bet.folded:
            continue
        subaction*(deck, face_up_cards) _ = deal_cards(deck, face_up_cards, 1)
        subaction*(players) bet = betting_round(players)
        if bet.folded:
            continue
        resolve_winner(players) 

fun get_current_player(Game g) -> Int:
    return 0

# the game is cooperative, player_id is irrelevant
fun score(Game g, Int player_id) -> Float:
    return float(g.players[0].chips.value)

fun get_num_players() -> Int:
    return 1

# i think a game lasts less than 1000 turns 
fun max_game_lenght() -> Int:
    return 2000

fun fuzz(Vector<Byte> input):
    if input.size() == 0:
        return
    let state = play()
    let action : AnyGameAction 
    parse_and_execute(state, action, input) 


fun pretty_print(Game g):
    print(g.players)
    print(g.face_up_cards)

fun main() -> Int:
    let game = play()
    let card_to_deal : DeckCardIndex
    game.deal(card_to_deal)
    game.deal(card_to_deal)
    game.deal(card_to_deal)
    game.deal(card_to_deal)
    game.deal(card_to_deal)
    game.deal(card_to_deal)
    game.deal(card_to_deal)
    game.fold()
    game.deal(card_to_deal)
    game.deal(card_to_deal)
    game.deal(card_to_deal)
    game.deal(card_to_deal)
    game.deal(card_to_deal)
    game.deal(card_to_deal)
    game.deal(card_to_deal)
    game.fold()
    game.deal(card_to_deal)
    game.deal(card_to_deal)
    game.deal(card_to_deal)
    game.deal(card_to_deal)
    game.deal(card_to_deal)
    game.deal(card_to_deal)
    game.deal(card_to_deal)
    game.fold()
    game.deal(card_to_deal)
    game.deal(card_to_deal)
    game.deal(card_to_deal)
    game.deal(card_to_deal)
    game.deal(card_to_deal)
    game.deal(card_to_deal)
    game.deal(card_to_deal)
    game.fold()
    game.deal(card_to_deal)
    game.deal(card_to_deal)
    game.deal(card_to_deal)
    game.deal(card_to_deal)
    game.deal(card_to_deal)
    game.deal(card_to_deal)
    game.deal(card_to_deal)
    game.fold()
    print_indented(game)
    return 0
