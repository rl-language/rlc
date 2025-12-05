from rlc import Layout, Text, Padding, Direction, FIT, FIXED, GROW

from test.display_layout import display

def build():
    root = Layout(sizing=(FIT(), FIT()) , padding=Padding(32, 32, 32, 32), direction=Direction.ROW, child_gap=32, color="white")
    child1 = Layout(sizing=(FIXED(300), FIXED(300)), color="yellow")
    child11 = Text("One Two Three Four", "Arial", 32)
    child1.add_child(child11)

    child2 = Layout(sizing=(FIXED(200), FIXED(200)), color="blue")

    child3 = Layout(sizing=(FIT(), FIXED(300)), child_gap=32, direction=Direction.ROW, color="lightblue")
    child4 = Layout(sizing=(FIXED(250), FIT()), color="orange")

    child44 = Text("Five Six Seven Eight Nine Ten Five Six Seven Eight Nine Ten Five Six Seven Eight Nine Ten", "Arial", 22)

    child4.add_child(child44)

    child3.add_child(child4)
    child3.add_child(child4)
    child3.add_child(child4)

    root.add_child(child1)
    root.add_child(child2)
    root.add_child(child3)
    return root

def build1():
    parent = Layout(sizing=(FIXED(200), FIXED(50)), direction=Direction.ROW, padding=Padding(0, 0, 0, 0), child_gap=0)
    c1 = Layout(sizing=(FIXED(50), FIXED(50)), color="blue")
    c2 = Layout(sizing=(GROW(), FIXED(50)), color="pink")
    c3 = Layout(sizing=(GROW(), FIXED(50)), color="purple")
    parent.add_child(c1)
    parent.add_child(c2)
    parent.add_child(c3)
    return parent

if __name__ == "__main__":
    display(build_function=build1)


