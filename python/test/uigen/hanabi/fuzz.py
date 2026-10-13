def register(b):
    b.map("card_click",
          lambda a: "select_own" if a.get("player") == 0 else "select_opponent",
          bind={"index": "index"}, guarded=True,
          targets=("select_own", "select_opponent"))
    b.dispatch_guarded("clue_color")
    b.dispatch_guarded("clue_number")
