# RUN: python %pyscript/solve.py %s --stdlib %stdlib --rlc rlc
import machine_learning

using ExchangeCardStock = BInt<0, 9>
using HandCardId = BInt<0, 10>
using StockID = BInt<0, 8>
using PLayerID = BInt<0, 8>

cls ExchangeCard:
    ExchangeCardStock remaining_stock

    fun value() -> Int:
        return 8 - self.remaining_stock.value

enum TraderCard:
    buy3:
        Int cost = 10
    stock_option:
        Int cost = 5
    sell2:
        Int cost = -5
    market_manipulation:
        Int cost = 3
    insider_info:
        Int cost = 4
    cash_in:
        Int cost = 4
    partner:
        Int cost = -2
    external_investment:
        Int cost = 1
    stock_dump:
        Int cost = 4
    ipo:
        Int cost = 2

    fun equal(TraderCard other) -> Bool:
        return self.value == other.value

    fun not_equal(TraderCard other) -> Bool:
        return self.value != other.value

cls Player:
    HiddenInformation<Bool[10]> available_cards
    ExchangeCardStock[8] owned_stocks
    ExchangeCardStock[8] cached_in 
    BoundedVector<TraderCard, 10> played_cards

    fun init_cards():
        let i = 0
        while i != 10:
            self.available_cards.value[i] = true
            i = i + 1

    fun has_stock_left() -> Bool:
        let i = 0
        while i != 8:
            if self.owned_stocks[i] != 0:
                return true
            i = i + 1
        return false 

    fun net_worth(BoundedVector<ExchangeCard, 8> stocks) -> Int:
        let to_return = 0
        let i = 0
        while i != stocks.size():
            to_return = to_return + stocks.get(i).value() * self.owned_stocks[i].value
            to_return = to_return + stocks.get(i).value() * self.cached_in[i].value
            i = i + 1

        let card  = 0
        while card != 10:
            if self.available_cards.value[card]:
                let c : TraderCard
                c.value = card
                to_return = to_return + c.cost()
            card = card + 1

        return to_return

fun other_player(Int player) -> Int:
    if player == 0:
        return 1
    return 0

fun is_there_remaining_stock(BoundedVector<ExchangeCard, 8> stocks) -> Bool:
    let i = 0
    while i != stocks.size():
        if stocks.get(i).remaining_stock != 0:
            return true
        i = i + 1
    return false

fun check_can_play_partner(TraderCard id, BoundedVector<ExchangeCard, 8> stocks) -> Bool:
    if id != TraderCard::partner:
       return true 

    let i = 0
    let total_stock = 0
    while i != stocks.size():
        total_stock = total_stock + stocks.get(i).remaining_stock.value
        i = i + 1
    return total_stock != 0

fun check_can_play_sell2(TraderCard id, Player player) -> Bool:
    if id != TraderCard::sell2:
       return true 

    let i = 0
    let total_stock = 0
    while i != 8:
        total_stock = total_stock + player.owned_stocks[i].value
        i = i + 1
    return total_stock >= 2

act remove_stock_from_table(ctx BoundedVector<ExchangeCard, 8> stocks, frm Int quantity) -> RemoveStockSequence:
    frm i = 0
    while i != quantity and is_there_remaining_stock(stocks):
        act select_stock(StockID id) {id < stocks.size(), stocks.get(id.value).remaining_stock != 0}
        stocks.get(id.value).remaining_stock = stocks.get(id.value).remaining_stock - 1
        i = i + 1

act take_stock_from_table(ctx BoundedVector<ExchangeCard, 8> stocks, ctx Player player, frm Int quantity) -> TakeStockSequence:
    frm i = 0
    while i != quantity and is_there_remaining_stock(stocks):
        act select_stock(StockID id) {id < stocks.size(), stocks.get(id.value).remaining_stock != 0}
        stocks.get(id.value).remaining_stock = stocks.get(id.value).remaining_stock - 1
        ref selected_stock = player.owned_stocks[id.value].value
        selected_stock = selected_stock + 1
        i = i + 1

act return_stock_from_table(ctx BoundedVector<ExchangeCard, 8> stocks, ctx Player player, frm Int quantity) -> ReturnStockSequence:
    frm i = 0
    while i != 2 and player.has_stock_left():
        act select_stock(StockID id) {id < 8, player.owned_stocks[id.value] != 0} 
        stocks.get(id.value).remaining_stock = stocks.get(id.value).remaining_stock + 1
        ref selected_stock = player.owned_stocks[id.value].value
        selected_stock = selected_stock - 1
        i = i + 1

act remove_stock_from_player(ctx Player player, frm Int quantity) -> TakeStockFromPlayerSequence:
    frm i = 0
    while i != quantity and player.has_stock_left():
        act select_stock(StockID id) {id < 8, player.owned_stocks[id.value] != 0}
        player.owned_stocks[id.value] = player.owned_stocks[id.value] - 1
        player.cached_in[id.value] = player.cached_in[id.value] + 1
        i = i + 1

@classes
act play() -> Game:
    frm players : BoundedVector<Player, 8>
    frm stocks : BoundedVector<ExchangeCard, 8>
    frm i = 0
    while i != 2:
        let player : Player
        player.owned_stocks[i] = 1
        player.available_cards.owner = i
        player.init_cards()
        players.append(player)
        let card : ExchangeCard
        card.remaining_stock = 7
        stocks.append(card)
        i = i + 1

    if players.size() == 2:
        let card : ExchangeCard 
        card.remaining_stock = 8
        stocks.append(card)
    i = 0
    frm game_started = false
    while i != 3:
        act discard_random(frm HandCardId card) {players.get(0).available_cards.value[card.value]}
        let p = 0 
        while p != players.size():
            players.get(p).available_cards.value[card.value] = false
            p = p + 1
        i = i + 1

    game_started = true
    frm current_round = 0
    frm current_player : PLayerID
    frm decision_maker : PLayerID
    while current_round != 5:
   
        frm committed_cards : Hidden<BoundedVector<TraderCard, 8>>
        i = 0
        while i != players.size():
            decision_maker = (current_player + i) % 2
            act play_card(TraderCard card) { 
                    players.get(decision_maker.value).available_cards.value[card.value],
                    check_can_play_sell2(card, players.get(decision_maker.value)),
                    check_can_play_partner(card, stocks) 
            }
            committed_cards.value.append(card)
            players.get(decision_maker.value).available_cards.value[card.value] = false
            i = i + 1 

        frm played_cards = committed_cards.value
        i = 0
        frm player_canceled : Bool[8]
        frm ipo_played = false
        frm next_current_player = current_player + 1
        while i != players.size():
            decision_maker = (current_player + i) % players.size()
            if player_canceled[decision_maker.value]:
                players.get(decision_maker.value).played_cards.append(played_cards.get(i))
                i = i + 1 
                continue
            actions:
                act buy3() {played_cards.get(i) == TraderCard::buy3}
                    subaction*(stocks, players.get(decision_maker.value)) take_stock = take_stock_from_table(stocks, players.get(decision_maker.value), 3)
                act sell2() {played_cards.get(i) == TraderCard::sell2}
                    subaction*(stocks, players.get(decision_maker.value)) return_stock = return_stock_from_table(stocks, players.get(decision_maker.value), 2)
                act stock_option() {played_cards.get(i) == TraderCard::stock_option}
                    act take_from_table(Bool do_it)
                    if do_it:
                        subaction*(stocks, players.get(decision_maker.value)) take_stock = take_stock_from_table(stocks, players.get(decision_maker.value), 2)
                    else:
                        act select_other_player(frm PLayerID id) {id != decision_maker, id < players.size()}
                        subaction*(stocks, players.get(id.value)) return_stock = return_stock_from_table(stocks, players.get(id.value), 2)
                act partner() {played_cards.get(i) == TraderCard::partner}
                    if is_there_remaining_stock(stocks):
                        act select_other_player(frm PLayerID id) {id != decision_maker, id < players.size()}
                        subaction*(stocks, players.get(id.value)) take_stock = take_stock_from_table(stocks, players.get(id.value), 1)
                act market_manipulation() {played_cards.get(i) == TraderCard::market_manipulation}
                    act select_other_player(frm PLayerID id) {id != decision_maker, id < players.size()}
                    player_canceled[id.value] = true
                act external_investment() {played_cards.get(i) == TraderCard::external_investment}
                    subaction*(stocks) remove_tock = remove_stock_from_table(stocks, 3)
                act ipo() {played_cards.get(i) == TraderCard::ipo}
                    if !ipo_played:
                        act select_stock(StockID id) {id < stocks.size()} 
                        ref remaining_stock = stocks.get(id.value).remaining_stock
                        let p = decision_maker.value
                        while remaining_stock != 0:
                            ref stock = players.get(p).owned_stocks[id.value].value
                            stock = stock + 1 
                            p = (p + 1) % players.size()
                            remaining_stock = remaining_stock - 1
                        ipo_played = true
                act cash_in() {played_cards.get(i) == TraderCard::cash_in}
                    subaction*(players.get(decision_maker.value)) remove_stock = remove_stock_from_player(players.get(decision_maker.value), 3)
                act stock_dump() {played_cards.get(i) == TraderCard::stock_dump}
                    act select_stock(StockID id) {id < stocks.size()} 
                    let p = 0
                    ref stock = stocks.get(id.value).remaining_stock.value
                    while p != players.size():
                        ref player_stock = players.get(p).owned_stocks[id.value].value
                        stock = stock + player_stock
                        player_stock = 0
                        p = p + 1
                act insider_info() {played_cards.get(i) == TraderCard::insider_info}
                    act play_card(TraderCard card) { 
                        players.get(decision_maker.value).available_cards.value[card.value],
                        check_can_play_sell2(card, players.get(decision_maker.value)),
                        check_can_play_partner(card, stocks) 
                    }
                    players.get(decision_maker.value).played_cards.append(TraderCard::insider_info)
                    played_cards.get(i).value = card.value 
                    players.get(decision_maker.value).available_cards.value[card.value] = false
                    next_current_player = decision_maker
                    continue
                    
            players.get(decision_maker.value).played_cards.append(played_cards.get(i))
            i = i + 1 

        current_player = next_current_player % 2
        current_round = current_round + 1
           
fun test_discard_3_cards() -> Bool:
    let state = play()
    let card : HandCardId
    state.discard_random(card)
    card.value = 1
    state.discard_random(card)
    card.value = 2
    state.discard_random(card)
    if state.players.get(0).available_cards.value[0]:
        return false
    if state.players.get(1).available_cards.value[0]:
        return false

    if state.players.get(0).available_cards.value[1]:
        return false
    if state.players.get(1).available_cards.value[1]:
        return false

    if state.players.get(0).available_cards.value[2]:
        return false
    if state.players.get(1).available_cards.value[2]:
        return false

    if !state.players.get(0).available_cards.value[3]:
        return false
    if !state.players.get(1).available_cards.value[3]:
        return false

    return true


fun get_current_player(Game g) -> Int:
    if g.is_done():
        return -4
    if g.game_started == false:
        return -1
    return g.decision_maker.value

fun score(Game g, Int player_id) -> Float:
    if g.players.get(player_id).net_worth(g.stocks) > g.players.get(other_player(player_id)).net_worth(g.stocks):
        return 1.0
    if g.players.get(player_id).net_worth(g.stocks) < g.players.get(other_player(player_id)).net_worth(g.stocks):
        return -1.0
    return 0.0

fun get_num_players() -> Int:
    return 2 

fun max_game_lenght() -> Int:
    return 1000


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

        print(executable.get(num_action % executable.size()))
        apply(executable.get(num_action % executable.size()), state)

fun log_p1_score(Game g) -> Int:
    return g.players[0].net_worth(g.stocks)

fun log_p2_score(Game g) -> Int:
    return g.players[0].net_worth(g.stocks)

fun pretty_print(Game g):
    print(g.stocks.get(0))
    print(g.stocks.get(1))
    print(g.stocks.get(2))
    print(g.players.get(0))
    print(g.players.get(0).net_worth(g.stocks))
    print(g.players.get(1))
    print(g.players.get(1).net_worth(g.stocks))
