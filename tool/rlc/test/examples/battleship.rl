# battleship implementation using the same rules as openspiel implementation (10x10 board, and 5 ships for each player)
# RUN: rlc %s -o %t -i %stdlib

import action
import enum_utils
import machine_learning

enum SlotMarking:
    unexplored
    hitted
    water

    fun equal(SlotMarking other) -> Bool:
        return self.value == other.value

enum Orientation:
    up
    left
    right
    down

    fun equal(Orientation other) -> Bool:
        return self.value == other.value

    fun to_y() -> Int:
        if self == Orientation::up:
            return 1
        if self == Orientation::down:
            return -1
        return 0

    fun to_x() -> Int:
        if self == Orientation::right:
            return 1
        if self == Orientation::left:
            return -1
        return 0

cls Ship:
    BInt<0, 10> x
    BInt<0, 10> y
    BInt<2, 6> lenght
    Orientation orientation 
    BInt<0, 6> num_hits

    fun contains(Int x, Int y) -> Bool:
        let i = 0
        while i != self.lenght.value:
            if x == self.x.value + self.orientation.to_x() and y == self.y.value + self.orientation.to_y():
                return true
            i = i + 1
        return false

cls Player:
    SlotMarking[10][10] slots
    BoundedVector<Ship, 5> player_ships
    
    # marks cell and returns if it contained a ship or not
    fun mark(Int x, Int y) -> Bool:
        let i = 0
        while i != self.player_ships.size():
            if self.player_ships[i].contains(x, y):
                self.slots[x][y] = SlotMarking::hitted
                self.player_ships[i].num_hits = self.player_ships[i].num_hits + 1
                if self.player_ships[i].num_hits.value == self.player_ships[i].lenght.value:
                    self.player_ships.erase(i)
                return true
            i = i + 1
        self.slots[x][y] = SlotMarking::water
        return false

    fun ship_in_bound(Ship s, Int x, Int y, Orientation o) -> Bool:
        let i = 0
        while i != s.lenght.value:
            let pos_x = (o.to_x() * i) + x
            let pos_y = (o.to_y()) * i + y
            if pos_x < 0:
                return false
            if pos_y < 0:
                return false
            if pos_x > 9:
                return false
            if pos_y >  9:
                return false
            let k = 0
            while k != self.player_ships.size():
                if self.player_ships[k].contains(pos_x, pos_y):
                    return false
                k = k + 1 
            i = i + 1
        return true

    fun can_be_targeted_at(Int x, Int y) -> Bool:
        return self.slots[x][y] == SlotMarking::unexplored
    

fun make_player_ships() -> BoundedVector<Ship, 5>:
    let ship : Ship
    let ships_to_place : BoundedVector<Ship, 5>
    ship.lenght = 2
    ships_to_place.append(ship)
    ship.lenght = 3
    ships_to_place.append(ship)
    ships_to_place.append(ship)
    ship.lenght = 4
    ships_to_place.append(ship)
    ships_to_place.append(ship)
    ship.lenght = 5
    ships_to_place.append(ship)
    ships_to_place.append(ship)

    return ships_to_place

act play() -> Game:
    frm current_player : Bool
    frm players : HiddenInformation<Player>[2]
    players[0].owner = 0
    players[1].owner = 1

    # setup, each player places its five ships in a non overlapping, non out of bounds way
    frm i = 0
    while i != 2:
        current_player = bool(i) 
        frm ships = make_player_ships() 
        while ships.size() != 0:
            act place_ship(BInt<0, 10> x, BInt<0, 10> y, Orientation orientation) {
                players[i].value.ship_in_bound(ships[0], x.value, y.value, orientation)
            }
            ref ship = ships[ships.size() - 1]
            ship.x = x
            ship.y = y
            ship.orientation = orientation
            players[i].value.player_ships.append(ship)
            ships.pop()
        i = i + 1

    # game start
    i = 0
    while i != 100: # open spiele gives 50 shots to each player
        current_player = bool(i % 2) 
        act shoot(BInt<0, 10> x, BInt<0, 10> y) { 
            players[(i+1) % 2].value.can_be_targeted_at(x.value, y.value)
        }

        players[(i+1) % 2].value.mark(x.value, y.value)
        if players[(i+1) % 2].value.player_ships.empty():
            return
        i = i + 1

fun get_current_player(Game g) -> Int:
    if g.is_done():
        return -4
    return int(g.current_player)

fun score(Game g, Int player_id) -> Float:
    if !g.is_done(): 
        return 0.0 
    if g.players[(1 + player_id) % 2].value.player_ships.empty():
        return 1.0
    if g.players[player_id].value.player_ships.empty():
        return -1.0
    return 0.0

fun get_num_players() -> Int:
    return 2


fun fuzz(Vector<Byte> input):
    if input.size() == 0:
        return
    let state = play()
    let action : AnyGameAction 
    parse_and_execute(state, action, input) 

fun main() -> Int:
    let o : Orientation 
    print(max(o))
    from_byte_vector(o, as_byte_vector(o))
    print(o)
    from_byte_vector(o, as_byte_vector(Orientation::left))
    print(o)
    from_byte_vector(o, as_byte_vector(Orientation::right))
    print(o)
    from_byte_vector(o, as_byte_vector(Orientation::up))
    print(o)
    from_byte_vector(o, as_byte_vector(Orientation::down))
    print(o)
    return 0
