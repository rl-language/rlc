from .layout import Layout
from .view_backend import ViewBackend
from typing import Optional
import time

class Text(Layout):
    def __init__(self, text, font_name, font_size, color="black"):
        super().__init__(color=color)  
        self.text = text
        self.color: str = color  
        self.font_name = font_name
        self.font_size = font_size
        self.text_surfaces = []
        self.alpha = 255
        self.last_value = text
        self.anim_start = None
        self.anim_duration = 0.3 

    def compute_size(self, available_width=None, available_height=None, logger=None, backend: Optional[ViewBackend] = None):
        if backend is None:
            self.width = 0
            self.height = 0
            return
        if available_width:
            lines = self.wrap_text(backend, available_width)
        else:
            lines = [self.text] if self.text.strip() else [" "]
            
        widths = [backend.get_text_size(line, self.font_name, self.font_size)[0] for line in lines]
        heights = [backend.get_text_size(line, self.font_name, self.font_size)[1] for line in lines]
        self.width = max(widths or [0])
        self.height = sum(heights or [0])
        
        if logger: logger.snapshot(self, "text_compute")

    def wrap_text(self, backend, max_width):
        words = self.text.split(" ")
        lines = []
        current_line = ""

        for word in words:
            line = current_line + (" " if current_line else "") + word
            w, h = backend.get_text_size(line, self.font_name, self.font_size)
            if w <= max_width:
                current_line = line
            else:
                if current_line:
                    lines.append(current_line)
                current_line = word
        if current_line:
            lines.append(current_line)
        return lines
    
    def update_text(self, new_text):
        """Start a smooth transition if value changed."""
        if new_text != self.text:
            self.last_value = self.text
            self.text = new_text
            self.anim_start = time.time()