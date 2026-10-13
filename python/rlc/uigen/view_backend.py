from typing import Tuple, List, Optional
from abc import ABC, abstractmethod


class ViewBackend(ABC):
    @abstractmethod
    def get_text_size(self, text: str, font_name: str, font_size: int) -> Tuple[int, int]:
        pass

    @abstractmethod
    def render_text(self, text: str, font_name: str, font_size: int, color: str) -> List:
        pass

    @abstractmethod
    def render_text_lines(self, lines: List[str], font_name: str, font_size: int, color: str,
                          anim_start, anim_duration, alpha) -> List:
        pass

    @abstractmethod
    def draw_rectangle(self, position: Tuple[int, int], size: Tuple[int, int],
                       color: str, border_size: int = 2):
        pass

    @abstractmethod
    def draw_border(self, position: Tuple[int, int], size: Tuple[int, int],
                    border_color: str = "darkgray", border_size: int = 2):
        pass

    @abstractmethod
    def blit_surface(self, surface, position: Tuple[int, int]):
        pass

    @abstractmethod
    def load_image(self, path: str, size: Tuple[int, int]) -> Optional[object]:
        pass

    @abstractmethod
    def draw_image(self, path: str, position: Tuple[int, int], size: Tuple[int, int]) -> bool:
        pass
