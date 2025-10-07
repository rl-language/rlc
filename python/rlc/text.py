from .layout import Layout
import pygame
from typing import Optional

class Text(Layout):
    def __init__(self, text, font_name, font_size, color="black"):
        super().__init__(color=color)  # Pass color as string to Layout 
        self.text = text
        self.color: str = color  # Store as string for JSON serialization
        self.pygame_color: pygame.Color = pygame.Color(color)  # Separate attribute for Pygame rendering
        self.font_name = font_name
        self.font_size = font_size
    def compute_size(self, available_width=None, available_height=None, logger=None):
        if not pygame.font.get_init():
            pygame.font.init()
        font = pygame.font.SysFont(self.font_name, self.font_size)
        if available_width:
            lines = self.wrap_text(font, available_width)
        else:
            lines = [self.text]
            
        self.text_surfaces = [font.render(line, True, self.pygame_color) for line in lines]
        if not self.text_surfaces:  # Safeguard for empty surfaces
            dummy_surface = font.render(" ", True, self.pygame_color)
            self.text_surfaces = [dummy_surface]
        self.width = max(s.get_width() for s in self.text_surfaces)
        self.height = sum(s.get_height() for s in self.text_surfaces)
        # print(f"Text '{self.text}': width={self.width}, height={self.height}")
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
    
    def to_dot(self, dot: 'Digraph', logger: Optional['LayoutLogger'] = None) -> str:
        """Generate Graphviz DOT representation for this text node."""
        from graphviz import Digraph
        nid = str(logger._attach_id(self) if logger else id(self))
        label = f'{self.__class__.__name__}#{nid}\n'
        label += f'{self.sizing[0].size_policy.value}×{self.sizing[1].size_policy.value}\n'
        label += f'dir={"-"}\n'
        label += f'size=({self.width}×{self.height})\n'
        label += f'pos=({self.x},{self.y})'
        txt = self.text
        preview = (txt[:20] + "…") if len(txt) > 20 else txt
        label += f'\n"{preview}"'
        color = self.color if hasattr(self, "color") and self.color else "#dddddd"
        dot.node(nid, label, style="filled", fillcolor=color)
        return nid