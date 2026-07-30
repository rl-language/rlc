# RUN: python %pyscript/solve.py %s --stdlib %stdlib --rlc rlc

import action
import collections.vector
import bounded_arg
import machine_learning
import algorithms.diff
import python

enum Suit:
    red
    yellow
    green
    blue
    white

    fun name() -> String:
        if self.value == Suit::red.value:
            return "red"s
        if self.value == Suit::yellow.value:
            return "yellow"s
        if self.value == Suit::green.value:
            return "green"s
        if self.value == Suit::blue.value:
            return "blue"s
        return "white"s

using CardValueType = BInt<1, 6>
using MaxCardValueType = BInt<0, 6>

cls Card:
    Suit suit
    CardValueType value

    fun init():
        self.suit = Suit::red
        self.value = 1

cls SlotKnowledge:
    Bool color_known
    Suit known_color
    Bool number_known
    CardValueType known_number

    fun init():
        self.color_known = false
        self.known_color = Suit::white
        self.number_known = false
        self.known_number = 1


cls ColorClue:
    Suit suit

cls NumberClue:
    CardValueType value

fun clue_matches(ColorClue | NumberClue clue, Card card) -> Bool:
    if clue is ColorClue:
        using T = type(clue)
        let c: T
        c = clue
        return c.suit.value == card.suit.value
    if clue is NumberClue:
        using T = type(clue)
        let n: T
        n = clue
        return n.value == card.value
    return false

fun clue_text(ColorClue | NumberClue clue) -> String:
    if clue is ColorClue:
        using T = type(clue)
        let c: T
        c = clue
        return "color "s.add(c.suit.name())
    if clue is NumberClue:
        using T = type(clue)
        let n: T
        n = clue
        return "number "s.add(to_string(n.value.value))
    return "unknown"s

fun log_clue(ColorClue | NumberClue clue, PlayerHand hand, Int giver, Int receiver) -> String:
    let msg = "p"s.add(to_string(giver + 1)).add(" to p"s).add(to_string(receiver + 1)).add(": card "s)
    let first = true
    let i = 0
    while i != hand.size():
        if clue_matches(clue, hand.get(i)):
            if !first:
                msg = msg.add(","s)
            msg = msg.add(to_string(i + 1))
            first = false
        i = i + 1
    msg = msg.add(" is "s).add(clue_text(clue))
    return msg



fun apply_clue_to_slot(ColorClue | NumberClue clue, SlotKnowledge slot):
    if clue is ColorClue:
        using T = type(clue)
        let c: T
        c = clue
        slot.color_known = true
        slot.known_color = c.suit
        return
    if clue is NumberClue:
        using T = type(clue)
        let n: T
        n = clue
        slot.number_known = true
        slot.known_number = n.value

using CardIndex = BInt<0, 51>
using HandCardIndex = BInt<0, 6>
using DeckType = BoundedVector<Card, 50>
using PlayerHand = BoundedVector<Card, 5>
using PlayerObtainedInformations = SlotKnowledge[5]
using Fuses = BInt<0, 4>
using InfoToken = BInt<0, 9>
using CurrentPlayerIndex = BInt<0, 3>

cls HanabiCallbacks:
    PyObject on_changed

cls Board:
    HiddenInformation<PlayerHand>[2] player_hands
    MaxCardValueType[5] highest_card_played
    PlayerObtainedInformations[2] player_infos
    BoundedVector<Card, 50> discard_pile
    Fuses fuses
    CurrentPlayerIndex current_player
    BInt<0, 9> info_token
    Int selected
    Bool selected_is_own
    Vector<String> info_box
    HanabiCallbacks callback

    fun log(String entry):
        self.info_box.append(entry)

    fun notify():
        let args : PyObject[1]
        args[0] = to_pyobject(self)
        self.callback.on_changed.call("on_changed", args)

fun make_deck() -> DeckType:
    let to_return : DeckType
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

fun other_player(Int current_player) -> Int:
    if current_player == 0:
        return 1
    return 0

fun erase_slot_knowledge(SlotKnowledge[5] slots, Int index, Int hand_size_before_erase):
    let counter = index
    while counter < hand_size_before_erase - 1:
        slots[counter] = slots[counter + 1]
        counter = counter + 1
    slots[hand_size_before_erase - 1].init()

fun reset_slot_knowledge(SlotKnowledge[5] slots, Int index):
    slots[index].init()

fun clue_matches_any(ColorClue | NumberClue clue, PlayerHand hand) -> Bool:
    let i = 0
    while i != hand.size():
        if clue_matches(clue, hand.get(i)):
            return true
        i = i + 1
    return false

@classes
act play() -> Game:
    frm deck : Hidden<DeckType>
    deck = make_deck()
    frm board : Board
    board.info_token.value = 8
    board.fuses.value = 3
    board.current_player.value = 0
    board.selected = -1
    board.selected_is_own = false

    board.player_hands[0].owner = 1
    board.player_hands[1].owner = 0

    while board.current_player != 2:
        frm current_card = 0
        while current_card != 5:
            act draw_random_card(CardIndex index) {index < deck.value.size()}
            board.player_hands[board.current_player.value].value.append(deck.value.get(index.value))
            deck.value.erase(index.value)
            current_card = current_card + 1
        board.current_player = board.current_player + 1

    board.current_player = 0
    while board.fuses != 0 and !have_all_5s_been_played(board.highest_card_played) and !deck.value.empty():
        frm turn_done = false
        while !turn_done:
            actions:
                act select_own(HandCardIndex index) {index < board.player_hands[board.current_player.value].value.size()}
                    board.selected = index.value
                    board.selected_is_own = true
                    board.notify()

                act select_opponent(HandCardIndex index) {board.info_token.value > 0 and index < board.player_hands[other_player(board.current_player.value)].value.size()}
                    board.selected = index.value
                    board.selected_is_own = false
                    board.notify()

                act deselect() {board.selected >= 0}
                    board.selected = -1
                    board.notify()

                act play_selected() {board.selected >= 0 and board.selected_is_own}
                    let card = board.player_hands[board.current_player.value].value.get(board.selected)
                    ref suit_pile = board.highest_card_played[card.suit.value]
                    if suit_pile.value == card.value.value - 1:
                        suit_pile.value = suit_pile.value + 1
                        if suit_pile.value == 5 and board.info_token.value < 8:
                            board.info_token.value = board.info_token.value + 1
                    else:
                        board.discard_pile.append(card)
                        board.fuses.value = board.fuses.value - 1

                    let hand_size_before = board.player_hands[board.current_player.value].value.size()
                    board.player_hands[board.current_player.value].value.erase(board.selected)
                    erase_slot_knowledge(board.player_infos[board.current_player.value], board.selected, hand_size_before)
                    act draw_random_card(CardIndex index) {index < deck.value.size()}
                    board.player_hands[board.current_player.value].value.append(deck.value.get(index.value))
                    deck.value.erase(index.value)
                    board.selected = -1
                    turn_done = true
                    board.notify()

                act discard_selected() {board.selected >= 0 and board.selected_is_own and board.info_token.value < 8}
                    let hand_size_before = board.player_hands[board.current_player.value].value.size()
                    board.discard_pile.append(board.player_hands[board.current_player.value].value.get(board.selected))
                    board.player_hands[board.current_player.value].value.erase(board.selected)
                    erase_slot_knowledge(board.player_infos[board.current_player.value], board.selected, hand_size_before)
                    board.info_token.value = board.info_token.value + 1
                    act draw_random_card(CardIndex index) {index < deck.value.size()}
                    board.player_hands[board.current_player.value].value.append(deck.value.get(index.value))
                    deck.value.erase(index.value)
                    board.selected = -1
                    turn_done = true
                    board.notify()

                act give_info(ColorClue | NumberClue clue) {board.selected >= 0 and !board.selected_is_own and board.info_token.value > 0 and clue_matches_any(clue, board.player_hands[other_player(board.current_player.value)].value)}
                    board.info_token.value = board.info_token.value - 1
                    let opponent = other_player(board.current_player.value)
                    ref opponent_hand = board.player_hands[opponent].value
                    let i = 0
                    while i != opponent_hand.size():
                        if clue_matches(clue, opponent_hand.get(i)):
                            apply_clue_to_slot(clue, board.player_infos[opponent][i])
                        i = i + 1
                    board.log(log_clue(clue, opponent_hand, board.current_player.value, opponent))
                    board.selected = -1
                    turn_done = true
                    board.notify()

        if board.current_player == 0:
            board.current_player = 1
        else:
            board.current_player = 0
        board.notify()

fun get_current_player(Game g) -> Int:
    if g.is_done():
        return -4
    let index : CardIndex
    if can g.draw_random_card(index):
        return -1
    return g.board.current_player.value

fun score(Game g, Int player_id) -> Float:
    let sum = 0.0
    let i = 0
    while i != 5:
        sum = sum + float(g.board.highest_card_played[i].value)
        i = i + 1
    return sum / 25.0

fun get_num_players() -> Int:
    return 2

fun max_game_lenght() -> Int:
    return 1000

fun fuzz(Vector<Byte> input):
    if input.size() == 0:
        return
    let state = play()
    let action : AnyGameAction
    parse_and_execute(state, action, input)

fun pretty_print(Game g):
    print(g.board.player_hands)
    print(g.board.highest_card_played)

fun game_diff(Game before, Game after, Vector<String> out):
    diff(before, after, out)