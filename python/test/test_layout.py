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
    layout = Layout(sizing=(FIXED(100), FIXED(50)), color="red")
    layout.compute_size()
    assert layout.width==100
    assert layout.height==50

def test_fit_size_single_child():
    parent = Layout(sizing=(FIT(), FIT()), padding=Padding(5, 5, 5, 5), color="white")
    child = Layout(sizing=(FIXED(50), FIXED(20)), color="blue")
    parent.add_child(child)
    parent.compute_size()

    # FIT should equal child size + padding
    assert parent.width == 50 + 5 + 5
    assert parent.height == 20 + 5 + 5

# Fit sizing
def test_fit_multiple_children_row():
    parent = Layout(sizing=(FIT(), FIT()), padding=Padding(0, 0, 0, 0), direction=Direction.ROW, child_gap=10)
    c1 = Layout(sizing=(FIXED(50), FIXED(20)), color="blue")
    c2 = Layout(sizing=(FIXED(30), FIXED(40)), color="green")
    parent.add_child(c1)
    parent.add_child(c2)
    parent.compute_size()

    # width = sum of widths + gap
    assert parent.width == 50 + 30 + 10
    # height = max of heights
    assert parent.height == 40

def test_fit_multiple_children_column():
    parent = Layout(sizing=(FIT(), FIT()), padding=Padding(10, 10, 10, 10), direction=Direction.COLUMN, child_gap=5)
    c1 = Layout(sizing=(FIXED(20), FIXED(10)), color="blue")
    c2 = Layout(sizing=(FIXED(60), FIXED(15)), color="pink")
    parent.add_child(c1)
    parent.add_child(c2)
    parent.compute_size()

    # height = sum of heights + gap
    assert parent.height == 10 + 15 + 5 + 10 + 10
    # width = max of widths
    assert parent.width == 60 + 10 + 10

# Grow anf Shrink
def test_grow_child_in_row():
    parent = Layout(sizing=(FIXED(200), FIXED(50)), direction=Direction.ROW, padding=Padding(0, 0, 0, 0), child_gap=0)
    c1 = Layout(sizing=(FIXED(50), FIXED(50)), color="blue")
    c2 = Layout(sizing=(GROW(), FIXED(50)), color="green")
    c3 = Layout(sizing=(GROW(), FIXED(50)), color="pink")
    parent.add_child(c1)
    parent.add_child(c2)
    parent.add_child(c3)
    parent.compute_size()

    # total = 200, child1 = 50, so child2 and child3 should grow evenly to 150/2=75 each
    assert parent.children[1].width == 75
    assert parent.children[2].width == 75

def test_grow_child_in_column():
    parent = Layout(sizing=(FIXED(100), FIXED(300)), direction=Direction.COLUMN, padding=Padding(0, 0, 0, 0), child_gap=0)
    c1 = Layout(sizing=(FIXED(100), FIXED(100)), color="blue")
    c2 = Layout(sizing=(FIXED(100), GROW()), color="red")
    parent.add_child(c1)
    parent.add_child(c2)
    parent.compute_size()

    # total = 300, child1 = 100, so child2 should grow to 200
    assert parent.children[1].height == 200

def test_extensive_layout():
    root = Layout(sizing=(FIT(), FIT()) , padding=Padding(32, 32, 32, 32), direction=Direction.ROW, child_gap=32)
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
    assert root.children[0].width == 300
    assert root.children[0].height == 300

    # child2 is FIXED(200, 200)
    assert root.children[1].width == 200
    assert root.children[1].height == 200

    # child3 has three children (each FIXED width 250) plus two gaps (32 each)
    # width = 250 + 32 + 250 + 32 + 250 = 814
    # height = FIXED(300)
    assert root.children[2].width == 814
    assert root.children[2].height == 300

def test_multiple_same_child_independent_sizes():
    parent = Layout(sizing=(FIXED(300), FIXED(100)), direction=Direction.ROW, padding=Padding(0, 0, 0, 0), child_gap=10, color="white")
    child = Layout(sizing=(GROW(), FIXED(50)), color="blue")
    parent.add_child(child)
    parent.add_child(child)  # Same instance, should be deep-copied
    parent.add_child(child)
    parent.compute_size()
    # Total width = 300, gap = 2 * 10 = 20, so each child gets (300 - 20) / 3 = 93
    assert len(parent.children) == 3
    assert parent.children[0].width == 93
    assert parent.children[1].width == 93
    assert parent.children[2].width == 93
    # Verify children are distinct objects
    assert parent.children[0] is not parent.children[1]
    assert parent.children[1] is not parent.children[2]
    assert parent.children[0] is not parent.children[2]
    # Verify heights are unchanged
    assert parent.children[0].height == 50
    assert parent.children[1].height == 50
    assert parent.children[2].height == 50