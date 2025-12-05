class EventDispatcher:
    def __init__(self, state, renderer, layout, program=None, selection_manager=None, handlers=None):
        self.state = state
        self.renderer = renderer
        self.layout = layout
        self.program = program
        self.handlers = handlers or {}
        self.selection = selection_manager

    def handle_click(self, node, elapsed_time=0.0):
        if not node or not node.on_click:
            return False
        binding_chain = getattr(node, "binding", None)
        handler = self.handlers.get(node.on_click)
        if callable(handler):
            changed = handler(node, self.selection, binding_chain, self.state, self.program, elapsed_time)
            if changed:
                self.renderer.update(self.layout, self.state.state, elapsed_time)
                self.layout.is_dirty = True
            return bool(changed)
        return False
    
    def handle_key(self, key, handler, elapsed_time=0.0):
        """Keyboard input when a cell is selected."""
        # focused = self.selection.focused_node
        # if not focused:
        #     return False
        changed = handler(key, None, self.state, self.program)
        if changed:
            self.renderer.update(self.layout, self.state.state, elapsed_time)
            self.layout.is_dirty = True
            return bool(changed)
        return False    
    
    def play_random_turn(self, elapsed_time=0.0):
        actions = self.state.legal_actions or []
        if not actions:
            return False
        import random
        action = random.choice(actions)
        print(action)
        self.state.step(action)
        self.renderer.update(self.layout, self.state.state, elapsed_time)
        self.layout.is_dirty = True
        return True

