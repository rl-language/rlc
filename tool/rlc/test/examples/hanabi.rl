# RUN: python %pyscript/solve.py %s --stdlib %stdlib --rlc rlc

import action
import collections.vector
import bounded_arg 
import machine_learning 

enum Suit:
    white
    yellow
    green
    blue
    red

using CardValueType = BInt<1, 6>
using MaxCardValueType = BInt<0, 6>

cls Card:
    Suit suit
    CardValueType value 

cls CardInfo:
    HandCardIndex index
    Suit suit
    CardValueType value 

using CardIndex = BInt<0, 41>
using HandCardIndex = BInt<0, 6>
using DeckType = BoundedVector<Card, 40>
using PlayerHand = BoundedVector<Card, 5>
using PlayerObtainedInformations = BoundedVector<CardInfo, 5>
using Fuses = BInt<0, 4>
using InfoToken = BInt<0, 6>

fun make_deck() -> DeckType:
    let to_return : DeckType 
    let suit : Suit
    let i = 0
    while i != 5:
        let card : Card
        card.suit.value = i
        card.value = 1
        to_return.append(card)
        to_return.append(card)
        to_return.append(card)
        card.value = 2
        to_return.append(card)
        to_return.append(card)
        card.value = 3
        to_return.append(card)
        to_return.append(card)
        card.value = 4
        to_return.append(card)
        to_return.append(card)
        card.value = 5
        to_return.append(card)
        i = i + 1
    return to_return


fun have_all_5s_been_played(MaxCardValueType[5] cards_played) -> Bool:
    let i = 0
    while i != 5:
        if cards_played[i] != 5:
            return false
        i = i + 1 
    return true


using CurrentPlayerIndex = BInt<0, 3>

fun other_player(Int current_player) -> Int:
    if current_player == 0:
        return 1
    return 0

act play() -> Game:
    # hanabi deck is secret to all players
    frm deck : Hidden<DeckType>
    deck = make_deck()
    # hanabi shows your hand to the other player but not yourself
    frm player_hands : HiddenInformation<PlayerHand>[2]
    player_hands[0].owner = 1 
    player_hands[1].owner = 0

    
    frm info_token : InfoToken
    info_token = 8
    frm fuses : Fuses
    fuses = 3
    frm highest_card_played : MaxCardValueType[5] 
    frm player_infos : PlayerObtainedInformations[2]
    
    # randomly draw 5 cards for each player
    frm current_player : CurrentPlayerIndex
    while current_player != 2:
        frm current_card = 0
        while current_card != 5:
            act draw_random_card(CardIndex index) {index < deck.value.size()}
            player_hands[current_player.value].value.append(deck.value.get(index.value))
            deck.value.erase(index.value)
            current_card = current_card + 1 
        current_player = current_player + 1

    current_player = 0
    while fuses != 0 and !have_all_5s_been_played(highest_card_played) and !deck.value.empty():
        
        ## not sure how to implement give_information 
        actions:
            act give_information(HandCardIndex index) {info_token != 0, index < player_hands[other_player(current_player.value)].value.size()}
                ref card = player_hands[other_player(current_player.value)].value.get(index.value)
                let info : CardInfo
                info.suit = card.suit
                info.value = card.value
                info.index = index
                if player_infos[current_player.value].size() == 5:
                    player_infos[current_player.value].erase(0)
                player_infos[current_player.value].append(info)
                info_token = info_token - 1
            act discard_card(HandCardIndex index) {index < player_hands[current_player.value].value.size()}
                player_hands[current_player.value].value.erase(index.value)
                act draw_random_card(CardIndex index) {index < deck.value.size()}
                player_hands[current_player.value].value.append(deck.value.get(index.value))
                info_token.value = info_token.value + 1
                deck.value.erase(index.value)
            act play_card(HandCardIndex index) {index < player_hands[current_player.value].value.size()}
                let card = player_hands[current_player.value].value.get(index.value)
                ref suit_pile = highest_card_played[card.suit.value]
                if suit_pile.value == card.value.value - 1:
                    suit_pile = suit_pile + 1
                    if suit_pile.value == 5:
                        info_token = info_token + 1
                else:
                    fuses = fuses - 1
                player_hands[current_player.value].value.erase(index.value)
                act draw_random_card(CardIndex index) {index < deck.value.size()}
                player_hands[current_player.value].value.append(deck.value.get(index.value))
                deck.value.erase(index.value)
       
        if current_player == 0:
            current_player = 1 
        else:
            current_player = 0

fun get_current_player(Game g) -> Int:
    # if it is done, return -4 to propagate
    # the information
    if g.is_done():
        return -4
    # if you can draw a card, it means
    # that the next action must be random
    # so -1
    let index : CardIndex
    if can g.draw_random_card(index):
        return -1
    #otherwise just return the current player
    return g.current_player.value

# the game is cooperative, player_id is irrelevant
fun score(Game g, Int player_id) -> Float:
    let sum = 0.0
    let i = 0
    while i != 5:
        sum = sum + float(g.highest_card_played[i].value)
        i = i + 1 
    return sum / 25.0

fun get_num_players() -> Int:
    return 2

# i think a game lasts less than 1000 turns 
fun max_game_lenght() -> Int:
    return 1000

fun fuzz(Vector<Byte> input):
    if input.size() == 0:
        return
    let state = play()
    let action : AnyGameAction 
    parse_and_execute(state, action, input) 


fun pretty_print(Game g):
    print(g.player_hands)
    print(g.highest_card_played)
