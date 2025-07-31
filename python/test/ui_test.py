from rlc import Container, Text, Padding, Direction, FIT, FIXED, GROW

from rlc import display

def build():
    root = Container("blue", sizing=(FIT(), FIT()) , padding=Padding(32, 32, 32, 32), direction=Direction.ROW, child_gap=32)
    child1 = Container("yellow", sizing=(FIXED(300), FIXED(300)))
    child11 = Text("One Two Three Four", "Arial", 32)
    child1.add_child(child11)

    child2 = Container("pink", sizing=(FIXED(200), FIXED(200)))

    child3 = Container("lightblue", sizing=(FIT(), FIXED(300)), child_gap=32, direction=Direction.ROW)
    child4 = Container("orange", sizing=(FIXED(250), FIT()))
    
    child44 = Text("Five Six Seven Eight Nine Ten Five Six Seven Eight Nine Ten Five Six Seven Eight Nine Ten", "Arial", 22)

    child4.add_child(child44)

    child3.add_child(child4)
    child3.add_child(child4)
    child3.add_child(child4)

    root.add_child(child1)
    root.add_child(child2)
    root.add_child(child3)
    return root


if __name__ == "__main__":
    display(build_function=build)
    
    