# RUN: python %pyscript/test.py --source %s -i %stdlib --rlc rlc
import bounded_arg
import action

ent Graph:
    Bool[10] nodes
    Bool[10][10] edges
    BInt<0, 10> pebble_reserve

    fun dump_dot():
        print("digraph g {")
        let x = 0
        while x != 10:
            let s : String
            s.append("_")
            s.append(to_string(x))
            s.append(to_string("[label=\""))
            s.append(to_string(self.nodes[x]))
            s.append(to_string("\"]"))
            print(s)
            x = x + 1

        x = 0
        while x != 10:
            let y = 0
            while y != 10:
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
        while x != 9:
            if self.nodes[x]:
                count = count + 1
            x = x + 1
        return count

    fun left_most_marked() -> Int:
        let x = 9
        while x != -1:
            if self.nodes[x]:
                return x 
            x = x - 1
        return 0

    fun has_won() -> Bool:
        if !self.nodes[9]:
            return false

        let x = 1
        while x != 9:
            if self.nodes[x]:
                return false
            x = x + 1

        return true

    fun is_unpeppable(Int node_index) -> Bool:
        if !self.nodes[node_index]:
            return false

        let x = 0
        while x != 9:
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
        while x != 9:
            if self.edges[x][node_index]:
                if !self.nodes[x]:
                    return false
            x = x + 1
        return true

fun create_graph() -> Graph:
    let self : Graph
    self.pebble_reserve.value = 10
    self.nodes[0] = true
    
    let x = 0
    while x != 9:
        self.edges[x][x + 1] = true
        x = x + 1
    return self 

act play() -> Game:
    frm graph = create_graph()
    frm are_we_done_generating = false
    while !are_we_done_generating:
        actions:
            act connect(BInt<0, 10> source, BInt<0, 10> target) {
                source.value < target.value
            }
                graph.edges[source.value][target.value] = true
            act done_generating()
                are_we_done_generating = true


    frm num_actions = 0
    while !graph.has_won() and num_actions != 100:
        num_actions = num_actions + 1
        actions:
            act pebble(BInt<0, 10> node) {
                graph.is_pebblable(node.value)            
            }
                graph.pebble_reserve = graph.pebble_reserve - 1
                graph.nodes[node.value] = true
            act unpebble(BInt<0, 10> node) {
                graph.is_unpeppable(node.value)            
            }
                graph.pebble_reserve = graph.pebble_reserve + 1
                graph.nodes[node.value] = false

fun test_dump_dot() -> Bool:
    let game = play()
    let source : BInt<0, 10>
    source.value = 0
    let target : BInt<0, 10>
    target.value = 2
    game.connect(source, target)
    game.graph.dump_dot()
    return true

fun get_current_player(Game g) -> Int:
    if g.is_done():
        return -4
    if !g.are_we_done_generating:
        return -1
    return 0

fun score(Game g, Int player_id) -> Float:
    if g.graph.has_won():
        return 12.0 - (float(g.num_actions) / 10.0)
    if g.is_done():
        return (float(g.graph.left_most_marked()) - float(g.graph.wrongly_marked_count())) / 10.0
    return 0.0

fun get_num_players() -> Int:
    return 1

fun gen_printer_parser():
    let state : Game 
    let any_action : AnyGameAction 
    gen_python_methods(state, any_action)

fun fuzz(Vector<Byte> input):
    if input.size() == 0:
        return
    let state = play()
    let action : AnyGameAction
    parse_and_execute(state, action, input) 

fun pretty_print_board(Game g):
    g.graph.dump_dot()

