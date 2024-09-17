# RUN: python %pyscript/solve.py %s --stdlib %stdlib --rlc rlc
import action
import machine_learning

using KitesCount = BInt<0, 3>
using TowerValue = BInt<1, 11>
using CardIndex = BInt<0, 82>

cls TowerCard:
    Bool has_flag
    KitesCount kites
    TowerValue value


fun append_to_string(TowerCard to_add, String output):
    output.append("v: ")
    output.append(to_string(to_add.value))
    output.append(" k: ")
    output.append(to_string(to_add.kites))
    if to_add.has_flag:
        output.append(" f") 

using TowerDeck = BoundedVector<TowerCard, 82>

fun make_card(Bool has_flag, Int kites, Int value) -> TowerCard:
    let card : TowerCard
    card.has_flag = has_flag
    card.kites = kites
    card.value = value
    return card

fun make_deck() -> TowerDeck:
    let to_return : TowerDeck
    let a_sheet = 0
    while a_sheet != 4:
        to_return.append(make_card(false, 2, 1))
        to_return.append(make_card(false, 1, 2))
        to_return.append(make_card(false, 1, 3))
        to_return.append(make_card(false, 1, 4))
        to_return.append(make_card(true, 1, 5))
        to_return.append(make_card(false, 1, 6))
        to_return.append(make_card(true, 0, 7))
        to_return.append(make_card(true, 0, 10))
        a_sheet = a_sheet + 1

    # b_sheet
    to_return.append(make_card(false, 2, 1))
    to_return.append(make_card(false, 1, 2))
    to_return.append(make_card(false, 1, 2))
    to_return.append(make_card(false, 1, 3))
    to_return.append(make_card(false, 1, 3))
    to_return.append(make_card(false, 1, 3))
    to_return.append(make_card(false, 0, 8))
    to_return.append(make_card(false, 0, 8))
    to_return.append(make_card(false, 0, 9))

    # c_sheet
    to_return.append(make_card(false, 2, 1))
    to_return.append(make_card(false, 1, 2))
    to_return.append(make_card(false, 0, 9))
    to_return.append(make_card(false, 1, 3))
    to_return.append(make_card(false, 1, 4))
    to_return.append(make_card(true, 1, 5))
    to_return.append(make_card(false, 1, 6))
    to_return.append(make_card(false, 0, 7))
    to_return.append(make_card(false, 0, 10))

    let d_sheet = 0
    while d_sheet != 3:
        to_return.append(make_card(false, 2, 1))
        to_return.append(make_card(false, 1, 2))
        to_return.append(make_card(false, 1, 3))
        to_return.append(make_card(false, 1, 4))
        to_return.append(make_card(true, 1, 5))
        to_return.append(make_card(false, 1, 6))
        to_return.append(make_card(true, 0, 7))
        to_return.append(make_card(true, 0, 10))
        d_sheet = d_sheet + 1

    # e_sheet
    to_return.append(make_card(false, 2, 1))
    to_return.append(make_card(false, 1, 2))
    to_return.append(make_card(false, 1, 2))
    to_return.append(make_card(false, 1, 3))
    to_return.append(make_card(false, 1, 3))
    to_return.append(make_card(false, 1, 3))
    to_return.append(make_card(false, 0, 8))
    to_return.append(make_card(false, 0, 9))

    return to_return

cls ActionCard:
    Bool is_demolish_face_up
    
    fun init():
        self.is_demolish_face_up = true

enum BonusCard:
    sky:
        Int kites = 3
    romance:
        Int kites = 3
    lucky:
        Int kites = 5
    big_ben:
        Int kites = 3
    star:
        Int kites = 3
    perfect:
        Int kites = 3
    triples:
        Int kites = 3
    demolition:
        Int kites = 3
    tycoon:
        Int kites = 3

    fun equal(BonusCard card) -> Bool:
        return self.value == card.value

using PlayerHand = BoundedVector<TowerCard, 82>
using PlayerIndex = BInt<0, 2>
using TurnsBeforeGameEnd = BInt<0, 5>
using TowerPile = BoundedVector<TowerCard, 8>
using Towers = BoundedVector<TowerPile, 6>
using TowerIndex = BInt<0, 7>

# 0 for no controller, player index + 1 otherwise
using BonusController = BInt<0, 5>

using BonusesController = BonusController[10]
    
cls Player:
    HiddenInformation<PlayerHand> hand
    ActionCard action_card
    Towers incomplete_towers 
    Towers complete_towers 
    Int cards_played
    
    fun kites(BonusesController bonuses) -> Int:
        let to_return = 0
        let i = 0
        while i != self.complete_towers.size():
            ref tower = self.complete_towers[i]
            let c = 0
            while c != tower.size():
                to_return = to_return + tower[c].kites.value
                c = c + 1
            i = i + 1

        i = 0
        while i != 10:
            if bonuses[i] == self.hand.owner + 1:
                let card : BonusCard
                card.value = i 
                to_return = to_return + card.kites()
            i = i + 1

        return to_return 

    fun flags() -> Int:
        let flags = 0
        let i = 0
        while i != self.complete_towers.size():
            ref tower = self.complete_towers[i]
            let c = 0
            while c != tower.size():
                flags = flags + int(tower[c].has_flag)
                c = c + 1
            i = i + 1
        return flags

act initialize_game(ctx TowerDeck deck, ctx BoundedVector<Player, 2> players, frm Int players_count) -> InitSequence:
    frm current_player = 0
    while current_player != players_count:
        frm player : Player
        players.append(player)
        players[current_player].hand.owner = current_player
        frm current_card = 0
        while current_card != 5:
            act deal_card(CardIndex index) {index < deck.size()}
            players[current_player].hand.value.append(deck[index.value])
            deck.erase(index.value)
            current_card = current_card + 1
        current_player = current_player + 1

fun tower_sum(TowerPile tower) -> Int:
    let i = 0
    let sum = 0
    while i != tower.size():
        sum = sum + tower[i].value.value
        i = i + 1
    return sum 

fun tower_sum(Player p, TowerIndex tower_index, TowerCard to_play) -> Int:
    if tower_index == p.incomplete_towers.size():
        return 0

    ref tower = p.incomplete_towers[tower_index.value]
    return tower_sum(tower) + to_play.value.value

fun card_is_blocked(BoundedVector<Player, 2> players, Int current_player, TowerCard to_play) -> Bool:
    let right_player_index = current_player - 1
    if right_player_index == -1:
        right_player_index = players.size() - 1
    ref right_player = players[right_player_index]
    if !right_player.incomplete_towers.empty():
        if right_player.incomplete_towers[0].back().value == to_play.value:
            return true

    let left_player_index = (current_player + 1) % players.size()
    ref left_player = players[left_player_index]
    if !left_player.incomplete_towers.empty():
        if left_player.incomplete_towers.back().back().value == to_play.value:
            return true

    return false

fun has_a_10(TowerPile tower) -> Bool:
    let i = 0
    while tower.size() != i:
        if tower[i].value == 10:
            return true 
        i = i + 1
    return false

fun has_only_1_2_3(TowerPile tower) -> Bool:
    let i = 0
    while tower.size() != i:
        if tower[i].value != 1 and tower[i].value != 2 and tower[i].value != 3:
            return false 
        i = i + 1
    return true

fun has_3_towers_with_a_10(Player p) -> Bool:
    let count = 0
    let current = 0
    while current != p.complete_towers.size():
        if has_a_10(p.complete_towers[current]):
            count = count + 1
        current = current + 1
    return count == 3 

fun at_least_four_4_or_5s(TowerPile tower) -> Bool:
    let values_count : Int[10]
    let i = 0
    while tower.size() != i:
        values_count[tower[i].value.value-1] = values_count[tower[i].value.value-1] + 1
        i = i + 1
    return values_count[3] >= 4 or values_count[4] >= 4

fun two_sets_of_tree(TowerPile tower) -> Bool:
    let values_count : Int[10]
    let distinct_value = 0
    let i = 0
    while tower.size() != i:
        ref current_element =  values_count[tower[i].value.value-1]
        if current_element >= 3:
            return false
        if current_element == 0:
            distinct_value = distinct_value + 1
        current_element = current_element + 1
        i = i + 1
    return distinct_value == 2

fun three_7(TowerPile tower) -> Bool:
    if tower.size() != 3:
        return false

    if tower[0].value != 7:
        return false
    if tower[1].value != 7:
        return false
    if tower[2].value != 7:
        return false

    return true

fun descending_6_to_1(TowerPile tower) -> Bool:
    let i = 0
    let current_value = 6
    if tower.size() != 6:
        return false
    while tower.size() != i:
        if tower[i].value.value != current_value:
            return false
        current_value = current_value - 1
        i = i + 1

    return true

fun check_bonuses_controller(TowerDeck deck, BoundedVector<Player, 2> players, PlayerIndex current_player, BonusesController bonuses):
    ref player = players[current_player.value]
    if player.complete_towers.empty():
        return
    if bonuses[0] == 0 and has_3_towers_with_a_10(player):
        bonuses[0] = current_player.value + 1
    if has_only_1_2_3(player.complete_towers.back()):
        bonuses[1] = current_player.value + 1
    if two_sets_of_tree(player.complete_towers.back()):
        bonuses[2] = current_player.value + 1
    if at_least_four_4_or_5s(player.complete_towers.back()):
        bonuses[3] = current_player.value + 1
    if descending_6_to_1(player.complete_towers.back()):
        bonuses[5] = current_player.value + 1
    if three_7(player.complete_towers.back()):
        bonuses[7] = current_player.value + 1
    if player.hand.value.size() == 0:
        bonuses[8] = current_player.value + 1

fun check_foundation(Player player, TowerIndex tower_index, TowerCard card) -> Bool:
    return card.value != 10 or tower_index == player.incomplete_towers.size()
        
# card with value one can only cover cards with value one
fun check_top(Player player, TowerIndex tower_index, TowerCard card) -> Bool:
    if tower_index == player.incomplete_towers.size():
        return true
    ref tower = player.incomplete_towers[tower_index.value]  
    if tower.back().value == 1:
        return card.value == 1
    return true

fun is_there_another_player_with_two_cards(BoundedVector<Player, 2> players, PlayerIndex current_player) -> Bool:
    let i = 0
    while i != players.size():
        if players[i].hand.value.size() >= 2 and current_player != i:
            return true
        i = i + 1
    return false

fun check_mayhem(PlayerIndex current_player, PlayerIndex target_player, TowerCard card) -> Bool:
    if current_player == target_player:
        return true
    if card.value == 8 or card.value == 9:
        return true
    return false

fun tower_index_valid(BoundedVector<Player, 2> players, Int current_player, Int target_player, Int tower_index) -> Bool:
    ref towers = players[target_player].incomplete_towers
    if current_player == target_player:
        return towers.size() >= tower_index
    else:
        return towers.size() > tower_index

fun card_is_selectable(CardIndex index, Bool from_extra_cards, PlayerHand extra_revealed_cards, BoundedVector<Player, 2> players, Int current_player, Bool is_building_4s) -> Bool:
    let card : TowerCard

    if from_extra_cards:
        if extra_revealed_cards.size() <= index.value:
            return false
        card = extra_revealed_cards[index.value]
    else:
        if players[current_player].hand.value.size() <= index.value:
            return false
        card = players[current_player].hand.value[index.value]

    if card_is_blocked(players, current_player, card):
        return false

    if is_building_4s:
        return card.value == 4

    return true

act perform_action(ctx TowerDeck deck, ctx BoundedVector<Player, 2> players, ctx BonusesController bonuses, ctx PlayerHand extra_revealed_cards, frm PlayerIndex current_player) -> ActionSequence:
    frm compleated_towers = 0
    frm extra_rounds = 0
    frm run_out_of_cards = false
    frm is_first_action = true
    frm is_building_4s = false
    while is_first_action or is_building_4s:
      is_first_action = false
      actions:
        act draw() { !deck.empty(), !is_building_4s }
            act deal_card(CardIndex index) {index < deck.size()}
            players[current_player.value].hand.value.append(deck[index.value])
            deck.erase(index.value)
            if deck.empty():
                run_out_of_cards = true
            return
        act demolish(TowerIndex index) {
            players[current_player.value].action_card.is_demolish_face_up,
            players[current_player.value].incomplete_towers.size() > index.value,
            !is_building_4s
        }
            ref player = players[current_player.value]
            player.action_card.is_demolish_face_up = false

            let tower = player.incomplete_towers[index.value]
            if tower.size() >= 5:
                bonuses[4] = current_player.value + 1
            player.incomplete_towers.erase(index.value)

            let i = 0
            while i != tower.size():
                player.hand.value.append(tower[i])               
                i = i + 1
            return
        act do_nothing() { is_building_4s or deck.empty()}
            return
        act select_card(frm CardIndex index, frm Bool from_extra_cards) {
            card_is_selectable(index, from_extra_cards, extra_revealed_cards, players, current_player.value, is_building_4s)
            }
            frm to_play : TowerCard 
            if from_extra_cards:
                to_play = extra_revealed_cards[index.value]
                extra_revealed_cards.erase(index.value)
            else:
                to_play = players[current_player.value].hand.value[index.value]
                players[current_player.value].hand.value.erase(index.value)
            players[current_player.value].cards_played = players[current_player.value].cards_played + 1

            act basics(frm TowerIndex tower_index, frm PlayerIndex target_player) {
              players.size() > target_player.value,
              tower_index_valid(players, current_player.value, target_player.value, tower_index.value),
              tower_sum(players[target_player.value], tower_index, to_play) <= 21,
              check_foundation(players[target_player.value], tower_index, to_play),
              check_top(players[target_player.value], tower_index, to_play),
              check_mayhem(current_player, target_player, to_play)
            }
            ref player = players[target_player.value]
            ref hand_player = players[current_player.value]
            if player.incomplete_towers.size() == tower_index.value:
                if player.incomplete_towers.size() == 6:
                    return
                let new_tower : TowerPile
                player.incomplete_towers.append(new_tower)

            ref tower = player.incomplete_towers[tower_index.value]
            tower.append(to_play)

            if tower_sum(tower) == 21:
                player.complete_towers.append(tower)
                player.incomplete_towers.erase(tower_index.value)
                compleated_towers = compleated_towers + 1
                hand_player.action_card.is_demolish_face_up = true

            check_bonuses_controller(deck, players, current_player, bonuses)             

            if to_play.value == 2 and is_there_another_player_with_two_cards(players, current_player):
                act pick_player(frm PlayerIndex other_player) { 
                    players.size() > other_player.value, 
                    other_player != current_player, 
                    players[other_player.value].hand.value.size() >= 2 
                }
                act opponent_pick_card(CardIndex to_give) {players[other_player.value].hand.value.size() > to_give.value }
                players[current_player.value].hand.value.append(players[other_player.value].hand.value[to_give.value])
                players[other_player.value].hand.value.erase(to_give.value)
                act opponent_pick_card(CardIndex to_give) {players[other_player.value].hand.value.size() > to_give.value }
                players[current_player.value].hand.value.append(players[other_player.value].hand.value[to_give.value])
                players[other_player.value].hand.value.erase(to_give.value)
                act pick_card(CardIndex to_give) {players[current_player.value].hand.value.size() > to_give.value }
                players[other_player.value].hand.value.append(players[current_player.value].hand.value[to_give.value])
                players[current_player.value].hand.value.erase(to_give.value)
                act pick_card(CardIndex to_give) {players[current_player.value].hand.value.size() > to_give.value }
                players[other_player.value].hand.value.append(players[current_player.value].hand.value[to_give.value])
                players[current_player.value].hand.value.erase(to_give.value)
                return
            if to_play.value == 5:
                frm i = 0
                while i != players.size():
                    while players[i].hand.value.size() > 5:
                        act pick_card(CardIndex to_give) {players[i].hand.value.size() > to_give.value }
                        deck.append(players[i].hand.value[to_give.value])
                        players[i].hand.value.erase(to_give.value)
                    i = i + 1
                return
            if to_play.value == 6:
                actions:
                    act draw(){index < deck.size()}
                        act deal_card(CardIndex index) {index < deck.size()}
                        players[current_player.value].hand.value.append(deck[index.value])
                        deck.erase(index.value)
                        if deck.empty():
                            run_out_of_cards = true
                        return
                    act ask_card(frm PlayerIndex to_ask_to, frm TowerValue requested_value) {to_ask_to != current_player, players.size() > to_ask_to.value}
                        let i = 0
                        while i != players[to_ask_to.value].hand.value.size():
                            if players[to_ask_to.value].hand.value[i].value == requested_value:
                                players[current_player.value].hand.value.append(players[to_ask_to.value].hand.value[i])
                                players[to_ask_to.value].hand.value.erase(i)
                                return 
                            i = i + 1
                        return
            if to_play.value == 9 or to_play.value == 8:
                check_bonuses_controller(deck, players, target_player, bonuses)             
                if target_player != current_player:
                    extra_rounds = extra_rounds + 1
                return 
            if to_play.value == 7 and !deck.empty():
                act deal_card(CardIndex index) {index < deck.size()}
                extra_revealed_cards.append(deck[index.value])
                deck.erase(index.value)
                if deck.empty():
                    run_out_of_cards = true
                return
            if to_play.value == 4:
                is_building_4s = true
        

act play() -> Game:
    frm tower_deck = make_deck()
    frm players : BoundedVector<Player, 2> 
    frm bonuses : BonusesController 
    frm extra_revealed_cards : PlayerHand
    frm turn_count : Int

    subaction*(tower_deck, players) init_sequence = initialize_game(tower_deck, players, 2)

    frm current_player : PlayerIndex
    act select_starting_player(PlayerIndex starting_player)
    current_player = starting_player
    frm turns_left : TurnsBeforeGameEnd

    while !tower_deck.empty() or turns_left != 0:
        frm action_count = 2
        if tower_deck.empty():
            action_count = 3
        frm compleated_towers = 0
        frm run_out_of_cards = false
        while action_count != 0:
            subaction*(tower_deck, players, bonuses, extra_revealed_cards) action = perform_action(tower_deck, players, bonuses, extra_revealed_cards, current_player)
            compleated_towers = compleated_towers + action.compleated_towers
            action_count = action_count + action.extra_rounds - 1
            if action.run_out_of_cards:
                run_out_of_cards = true

        if compleated_towers >= 2:
            bonuses[6] = (current_player.value + 1)
        
        current_player = (current_player.value + 1) % players.size()
        turns_left = turns_left - 1
        turn_count = turn_count + 1

        if run_out_of_cards: 
           turns_left = players.size() 
    

fun get_current_player(Game g) -> Int:
    if g.is_done():
        return -4
    let index : CardIndex
    let index_player : PlayerIndex 
    if can g.select_starting_player(index_player):
        return -1
    index = 0
    if can g.deal_card(index):
        return -1
    if can g.opponent_pick_card(index):
        return g.action.other_player.value
    return g.current_player.value

fun other_player(Int i) -> Int:
    if i == 0:
        return 1
    return 0

fun score(Game g, Int player_id) -> Float:
    if !g.is_done():
        return 0.0
    if g.players[player_id].kites(g.bonuses) > g.players[other_player(player_id)].kites(g.bonuses):
        return 1.0
    if g.players[player_id].kites(g.bonuses) < g.players[other_player(player_id)].kites(g.bonuses):
        return -1.0
    if g.players[player_id].flags() > g.players[other_player(player_id)].flags():
        return 1.0
    if g.players[player_id].flags() < g.players[other_player(player_id)].flags():
        return -1.0
    return 0.0

fun get_num_players() -> Int:
    return 2 

fun max_game_lenght() -> Int:
    return 1000

fun pretty_print(Game g):
    print_indented(g.players)
    print(g.action_count)

fun log_bonus0_big_ben(Game g) -> Int:
    return g.bonuses[0].value

fun log_bonus1_sky(Game g) -> Int:
    return g.bonuses[1].value

fun log_bonus2_triples(Game g) -> Int:
    return g.bonuses[2].value

fun log_bonus3_romance(Game g) -> Int:
    return g.bonuses[3].value

fun log_bonus4_demolition(Game g) -> Int:
    return g.bonuses[4].value

fun log_bonus5_star(Game g) -> Int:
    return g.bonuses[5].value

fun log_bonus6_tycoon(Game g) -> Int:
    return g.bonuses[6].value

fun log_bonus7_lucky(Game g) -> Int:
    return g.bonuses[7].value

fun log_bonus8_perfect(Game g) -> Int:
    return g.bonuses[8].value

fun log_game_length(Game g) -> Int:
    return g.turn_count

fun description_game_length() -> String:
    return "The number of times players passed their turn."s

fun log_p1_tower_played(Game g) -> Int:
    return g.players[0].cards_played

fun description_p1_towers_played() -> String:
    return "Number of cards played by p1"s

fun log_p2_tower_played(Game g) -> Int:
    return g.players[1].cards_played

fun description_p2_towers_played() -> String:
    return "Number of cards played by p2"s

fun description_score_p0() -> String:
    return "Number of games lost (first bar) and won (second bar). Very close to 50%. Working as designed."s

fun log_p1_kites(Game g) -> Int:
    return g.players[0].kites(g.bonuses)

fun description_p1_kites() -> String:
    return "quantity of kites accumulated by player 1 at the end of the game"s


fun description_bonus0_big_ben() -> String:
    return "Bonus 0, BIG BEN. requires to have 3 towers with a 10 card. 0 means that the bonus was not taken at the end of the game. 1 that it was taken by player 1. 2 that it was taken by player 2.  SKY has been used by the machine learning agent for the entire training, and its usage has been trending upward for most of it. Only 30% of games have SKY being claimed, but it is seems to be working as intended, and with more time to train would have been used even more"s

fun description_bonus1_sky() -> String:
    return "Bonus 1, SKY. requires to only have 1s 2s or 3s in the tower. 0 means that the bonus was not taken at the end of the game. 1 that it was taken by player 1. 2 that it was taken by player 2. Romance has been consistently used more than 95% of the games for most training, and it is still even trending upward. Working as designed."s

fun description_bonus2_triples() -> String:
    return "Bonus 2, TRIPLES. requires two sents of three cards. 0 means that the bonus was not taken at the end of the game. 1 that it was taken by player 1. The training has been mostly stable. The initial surge shows that the network understand what the bonus is, but does not seems to value it much. When it can occasionaly get it, it will do so, but does not build strategies aroud it."s

fun description_bonus3_romance() -> String:
    return "Bonus 3, ROMANCE. requires four 5 value cards, or four 4 value cards. 0 means that the bonus was not taken at the end of the game. 1 that it was taken by player 1. The training has been stable. The initial surge shows that the network understand what the bonus is, but does not seems to value it at all. It seems that the network really does not wish to invest 4 identical cards in the same tower, or that the opponent can easily counter the strategy when it notices the opponent doing so."s

fun description_bonus4_demolition() -> String:
    return "Bonus 4, DEMOLITION. demolish a tower with 5 or more cards. 0 means that the bonus was not taken at the end of the game. 1 that it was taken by player 1. The training has been mostly stable. The initial surge shows that the network understand what the bonus is, and uses it in 60% of games. Seems to be working as designed."s

fun description_bonus5_star() -> String:
    return "Bonus 5, STAR. requires a tower in descending order from 6 to 1. 0 means that the bonus was not taken at the end of the game. 1 that it was taken by player 1. The time graph never moves consistently away from 0. The training has compleatly failed. The network never managed to learn what to do with it in the training time allocated to it. The graphs should be ignored and no insight can be gained from them."s

fun description_bonus6_tycoon() -> String:
    return "Bonus 6, TYCOON. requires to build two towers in one turn. 0 means that the bonus was not taken at the end of the game. 1 that it was taken by player 1. The graph has been trending upward for the whole training. At end of the training only 20% of game have used this bonus, but if the more time was allowed to train, more games would have done so."s


fun description_bonus7_lucky() -> String:
    return "Bonus 7, LUCKY. requires to have a tower with 3 sevens. 0 means that the bonus was not taken at the end of the game. 1 that it was taken by player 1. The training graph has been stable. At end of the training only 15% of game have used this bonus. It seems the network does not value the commitment of using 3 high value cards in the same tower."s

fun description_bonus8_perfect() -> String:
    return "Bonus 8, PERFECT. requires to have 0 cards in hand. Stable training with high play rate."s



fun log_p2_kites(Game g) -> Int:
    return g.players[1].kites(g.bonuses)

fun description_p2_kites() -> String:
    return "quantity of kites accumulated by player 2 at the end of the game"s

fun log_p1_flags(Game g) -> Int:
    return g.players[0].flags()

fun description_p1_flags() -> String:
    return "quantity of flags accumulated by player 1 at the end of the game"s

fun description_p2_flags() -> String:
    return "quantity of flags accumulated by player 2 at the end of the game"s

fun log_p2_flags(Game g) -> Int:
    return g.players[1].flags()

fun fuzz(Vector<Byte> input):
    let state = play()
    let x : AnyGameAction
    let enumeration = enumerate(x)
    let index = 0
    while index + 8 < input.size() and !state.is_done():
        let num_action : Int
        from_byte_vector(num_action, input, index)
        if num_action < 0:
          num_action = num_action * -1 
        if num_action < 0:
          num_action = 0 

        let executable : Vector<AnyGameAction>
        let i = 0
        #print("VALIDS")
        while i < enumeration.size():
          if can apply(enumeration.get(i), state):
            #print(enumeration.get(i))
            executable.append(enumeration.get(i))
          i = i + 1
        #print("ENDVALIDS")
        #if executable.size() == 0:
        #print("zero valid actions")
        #print(state)
        #return

        #print(executable.get(num_action % executable.size()))
        apply(executable.get(num_action % executable.size()), state)
