from typing import Tuple, List
from abc import ABC, abstractmethod

class RendererBackend(ABC):
    @abstractmethod
    def get_text_size(self, text: str, font_name: str, font_size: int) -> Tuple[int, int]:
        pass

    @abstractmethod
    def render_text(self, text: str, font_name: str, font_size: int, color: str) -> List['Surface']:
        pass
    
    @abstractmethod
    def render_text_lines(self, lines: List[str], font_name: str, font_size: int, color: str) -> List['Surface']:
        pass

    @abstractmethod
    def draw_rectangle(self, position: Tuple[int, int], size: Tuple[int, int], color: str):
        pass

    @abstractmethod
    def blit_surface(self, surface, position: Tuple[int, int]):
        pass