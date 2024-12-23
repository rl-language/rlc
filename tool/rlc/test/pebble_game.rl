# RUN: python %pyscript/test.py %s --stdlib %stdlib --rlc rlc
# RUN: python %pyscript/learn.py %s --stdlib %stdlib --rlc rlc --total-train-iterations=1 -o %t
# RUN: python %pyscript/play.py %s --stdlib %stdlib --rlc rlc %t

# implementation of https://en.wikipedia.org/wiki/Pebble_game
import bounded_arg
import action
import math.numeric

cls Graph:
    Bool[20] nodes
    Bool[20][20] edges
    BInt<0, 20> pebble_reserve
    BInt<0, 20> max_used_pebble

    fun after_placement():
        self.max_used_pebble.value = max(self.max_used_pebble.value, 20 - self.pebble_reserve.value)

    fun dump_dot():
        print("digraph g {")
        let x = 0
        while x != 20:
            let s : String
            s.append("_")
            s.append(to_string(x))
            s.append(to_string("[label=\""))
            s.append(to_string(self.nodes[x]))
            s.append(to_string("\"]"))
            print(s)
            x = x + 1

        x = 0
        while x != 20:
            let y = 0
            while y != 20:
                if self.edges[x][y]:
                    let s : String
                    s.append("_")
                    s.append(to_string(x))
                    s.append(" -> ")
                    s.append("_")
                    s.append(to_string(y))
                    print(s)
                y = y + 1
            x = x + 1

        print("}")

    fun wrongly_marked_count() -> Int:
        let count = 0
        let x = 1
        while x != 19:
            if self.nodes[x]:
                count = count + 1
            x = x + 1
        return count

    fun left_most_marked() -> Int:
        let x = 19
        while x != -1:
            if self.nodes[x]:
                return x 
            x = x - 1
        return 0

    fun has_won() -> Bool:
        if !self.nodes[19]:
            return false

        let x = 1
        while x != 19:
            if self.nodes[x]:
                return false
            x = x + 1

        return true

    fun is_unpeppable(Int node_index) -> Bool:
        if !self.nodes[node_index]:
            return false

        let x = 0
        while x != 19:
            if self.edges[x][node_index]:
                if !self.nodes[x]:
                    return false
            x = x + 1
        return true

    fun is_pebblable(Int node_index) -> Bool:
        if self.nodes[node_index]:
            return false

        if self.pebble_reserve == 0:
            return false

        let x = 0
        while x != 19:
            if self.edges[x][node_index]:
                if !self.nodes[x]:
                    return false
            x = x + 1
        return true

fun create_graph() -> Graph:
    let self : Graph
    self.pebble_reserve.value = 20
    self.nodes[0] = true
    self.edges[0][1] = true
    self.edges[0][2] = true
    self.edges[0][3] = true
    self.edges[0][4] = true
    self.edges[0][5] = true

    self.edges[1][6] = true
    self.edges[2][6] = true

    self.edges[1][7] = true
    self.edges[2][7] = true

    self.edges[3][8] = true
    self.edges[4][8] = true

    self.edges[4][9] = true
    self.edges[5][9] = true

    self.edges[3][10] = true
    self.edges[5][10] = true

    self.edges[6][11] = true
    self.edges[6][11] = true

    self.edges[4][12] = true
    self.edges[8][12] = true

    self.edges[8][13] = true
    self.edges[9][13] = true

    self.edges[9][14] = true

    self.edges[10][14] = true

    self.edges[11][15] = true
    self.edges[12][15] = true

    self.edges[13][16] = true
    self.edges[15][16] = true

    self.edges[12][18] = true
    self.edges[13][18] = true

    self.edges[14][19] = true
    self.edges[16][19] = true
    self.edges[18][19] = true


    return self 

@classes
act play() -> Game:
    frm graph = create_graph()
    frm are_we_done_generating = 1
    while are_we_done_generating != 0:
        actions:
            act connect(BInt<0, 20> source, BInt<0, 20> target) {
                source.value < target.value
            }
                graph.edges[source.value][target.value] = true
            act done_generating()
                are_we_done_generating = are_we_done_generating - 1


    frm num_actions = 0
    while !graph.has_won() and num_actions != 100:
        num_actions = num_actions + 1
        graph.after_placement()
        actions:
            act pebble(BInt<0, 20> node) {
                graph.is_pebblable(node.value)            
            }
                graph.pebble_reserve = graph.pebble_reserve - 1
                graph.nodes[node.value] = true
            act unpebble(BInt<0, 20> node) {
                graph.is_unpeppable(node.value)            
            }
                graph.pebble_reserve = graph.pebble_reserve + 1
                graph.nodes[node.value] = false

fun test_dump_dot() -> Bool:
    let game = play()
    game.graph.dump_dot()
    return true

fun main() -> Int:
    test_dump_dot()
    return 0

fun max_game_lenght() -> Int:
    return 100

fun get_current_player(Game g) -> Int:
    if g.is_done():
        return -4
    if g.are_we_done_generating != 0:
        return -1
    return 0

fun score(Game g, Int player_id) -> Float:
    if g.graph.has_won():
        return 12.0 - (float(g.graph.max_used_pebble.value) / 20.0)
    if g.is_done() and g.graph.nodes[19]:
        return 2.0 - (float(g.graph.wrongly_marked_count()) / 20.0)
    if g.is_done():
        return (float(g.graph.left_most_marked())) / 20.0
    return 0.0

fun get_num_players() -> Int:
    return 1

fun fuzz(Vector<Byte> input):
    if input.size() == 0:
        return
    let state = play()
    let action : AnyGameAction
    parse_and_execute(state, action, input) 

fun pretty_print_board(Game g):
    g.graph.dump_dot()

