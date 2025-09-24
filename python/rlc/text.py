from .layout import Layout
import pygame

class Text(Layout):
    def __init__(self, text, font_name, font_size, color="black"):
        super().__init__() 
        self.text = text
        self.color = pygame.Color(color)
        self.font_name = font_name
        self.font_size = font_size
    def compute_size(self, available_width=None, available_height=None, logger=None):
        font = pygame.font.SysFont(self.font_name, self.font_size)
        if available_width:
            lines = self.wrap_text(font, available_width)
        else:
            lines = [self.text]
            
        self.text_surfaces = [font.render(line, True, self.color) for line in lines]
        self.width = max(s.get_width() for s in self.text_surfaces)
        self.height = sum(s.get_height() for s in self.text_surfaces)
        if logger: logger.snapshot(self, "text_compute")

    def wrap_text(self, font, max_width):
        words = self.text.split(" ")
        lines = []
        current_line = ""

        for word in words:
            line = current_line + (" " if current_line else "") + word
            if font.size(line)[0] <= max_width:
                current_line = line
            else:
                if current_line:
                    lines.append(current_line)
                current_line = word
        if current_line:
            lines.append(current_line)
        return lines