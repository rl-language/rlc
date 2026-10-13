def register(b):
    b.ui_only("select_cell")
    b.map("input_value", "place",
          bind={"x": "row", "y": "col"},
          free={"value": "num"})
