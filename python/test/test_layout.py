import pytest
import pygame
from rlc import Layout, Text, FIXED, FIT, GROW, Padding, Direction

@pytest.fixture(autouse=True, scope="session")
def init_pygame():
    pygame.init()
    pygame.font.init()
    yield
    pygame.quit()

# Fixed sizing
def test_fixed_size_layout():
    layout = Layout("red", sizing=(FIXED(100), FIXED(50)))
    layout.compute_size()
    assert layout.width==100
    assert layout.height==50

def test_fit_size_single_child():
    parent = Layout("white", sizing=(FIT(), FIT()), padding=Padding(5, 5, 5, 5))
    child = Layout("blue", sizing=(FIXED(50), FIXED(20)))
    parent.add_child(child)
    parent.compute_size()

    # FIT should equal child size + padding
    assert parent.width == 50 + 5 + 5
    assert parent.height == 20 + 5 + 5

# Fit sizing
def test_fit_multiple_children_row():
    parent = Layout("white", sizing=(FIT(), FIT()), padding=Padding(0, 0, 0, 0), direction=Direction.ROW, child_gap=10)
    c1 = Layout("blue", sizing=(FIXED(50), FIXED(20)))
    c2 = Layout("green", sizing=(FIXED(30), FIXED(40)))
    parent.add_child(c1)
    parent.add_child(c2)
    parent.compute_size()

    # width = sum of widths + gap
    assert parent.width == 50 + 30 + 10
    # height = max of heights
    assert parent.height == 40

def test_fit_multiple_children_column():
    parent = Layout("white", sizing=(FIT(), FIT()), padding=Padding(10, 10, 10, 10), direction=Direction.COLUMN, child_gap=5)
    c1 = Layout("blue", sizing=(FIXED(20), FIXED(10)))
    c2 = Layout("green", sizing=(FIXED(60), FIXED(15)))
    parent.add_child(c1)
    parent.add_child(c2)
    parent.compute_size()

    # height = sum of heights + gap
    assert parent.height == 10 + 15 + 5 + 10 + 10
    # width = max of widths
    assert parent.width == 60 + 10 + 10

# Grow anf Shrink
def test_grow_child_in_row():
    parent = Layout("white", sizing=(FIXED(200), FIXED(50)), direction=Direction.ROW, padding=Padding(0, 0, 0, 0), child_gap=0)
    c1 = Layout("blue", sizing=(FIXED(50), FIXED(50)))
    c2 = Layout("green", sizing=(GROW(), FIXED(50)))
    c3 = Layout("pink", sizing=(GROW(), FIXED(50)))
    parent.add_child(c1)
    parent.add_child(c2)
    parent.add_child(c3)
    parent.compute_size()

    # total = 200, child1 = 50, so child2 and child3 should grow evenly to 150/2=75 each
    assert c2.width == 75
    assert c3.width == 75

def test_grow_child_in_column():
    parent = Layout("white", sizing=(FIXED(100), FIXED(300)), direction=Direction.COLUMN, padding=Padding(0, 0, 0, 0), child_gap=0)
    c1 = Layout("blue", sizing=(FIXED(100), FIXED(100)))
    c2 = Layout("green", sizing=(FIXED(100), GROW()))
    parent.add_child(c1)
    parent.add_child(c2)
    parent.compute_size()

    # total = 300, child1 = 100, so child2 should grow to 200
    assert c2.height == 200

def test_extensive_layout():
    root = Layout("blue", sizing=(FIT(), FIT()) , padding=Padding(32, 32, 32, 32), direction=Direction.ROW, child_gap=32)
    child1 = Layout("yellow", sizing=(FIXED(300), FIXED(300)))
    child11 = Text("One Two Three Four", "Arial", 32)
    child1.add_child(child11)

    child2 = Layout("pink", sizing=(FIXED(200), FIXED(200)))

    child3 = Layout("lightblue", sizing=(FIT(), FIXED(300)), child_gap=32, direction=Direction.ROW)
    child4 = Layout("orange", sizing=(FIXED(250), FIT()))
    
    child44 = Text("Five Six Seven Eight Nine Ten Five Six Seven Eight Nine Ten Five Six Seven Eight Nine Ten", "Arial", 22)

    child4.add_child(child44)

    child3.add_child(child4)
    child3.add_child(child4)
    child3.add_child(child4)

    root.add_child(child1)
    root.add_child(child2)
    root.add_child(child3)
    root.compute_size()


    # root.width =
    #   child1.width (300)
    # + gap (32)
    # + child2.width (200)
    # + gap (32)
    # + child3.width (3 * 250 + 2 * 32 = 814)
    # + left/right padding (32 + 32)
    # = 1442
    assert root.width == 1442
    # root.height = max( child1.height (300), child2.height (200), child3.height (300) )
    # + top/bottom padding (32 + 32)
    # = 364
    assert root.height == 364

    # child1 is FIXED(300, 300)
    assert child1.width == 300
    assert child1.height == 300

    # child2 is FIXED(200, 200)
    assert child2.width == 200
    assert child2.height == 200

    # child3 has three children (each FIXED width 250) plus two gaps (32 each)
    # width = 250 + 32 + 250 + 32 + 250 = 814
    # height = FIXED(300)
    assert child3.width == 814
    assert child3.height == 300