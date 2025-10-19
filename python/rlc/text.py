from .layout import Layout
from .renderer_backend import RendererBackend
from typing import Optional

class Text(Layout):
    def __init__(self, text, font_name, font_size, color="black"):
        super().__init__(color=color)  # Pass color as string to Layout 
        self.text = text
        self.color: str = color  # Store as string for JSON serialization
        # self.pygame_color: pygame.Color = pygame.Color(color)  # Separate attribute for Pygame rendering
        self.font_name = font_name
        self.font_size = font_size
        self.text_surfaces = []
    def compute_size(self, available_width=None, available_height=None, logger=None, backend: Optional[RendererBackend] = None):
        if backend is None:
            # Default to 0 if no backend (for non-rendering modes)
            self.width = 0
            self.height = 0
            return
        # font = pygame.font.SysFont(self.font_name, self.font_size)
        # print("----->>> in Text: ", available_width)
        if available_width:
            lines = self.wrap_text(backend, available_width)
        else:
            lines = [self.text] if self.text.strip() else [" "]
            
        # self.text_surfaces = [font.render(line, True, self.pygame_color) for line in lines]
        # if not self.text_surfaces:  # Safeguard for empty surfaces
        #     dummy_surface = font.render(" ", True, self.pygame_color)
        #     self.text_surfaces = [dummy_surface]
        # self.width = max(s.get_width() for s in self.text_surfaces)
        # self.height = sum(s.get_height() for s in self.text_surfaces)
        widths = [backend.get_text_size(line, self.font_name, self.font_size)[0] for line in lines]
        # print("----->>> in Text widths: ", widths)
        heights = [backend.get_text_size(line, self.font_name, self.font_size)[1] for line in lines]
        # print("----->>> in Text heights: ", heights)
        # widths, heights = zip(*[backend.get_text_size(line, self.font_name, self.font_size) for line in lines])
        self.width = max(widths or [0])
        self.height = sum(heights or [0])
        # print("----->>> in Text width: ", self.width)
        # print("----->>> in Text height: ", self.height)
        
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