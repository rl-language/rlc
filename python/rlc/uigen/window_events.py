from dataclasses import dataclass


@dataclass
class QuitEvent:
    pass

@dataclass
class ResizeEvent:
    w: int
    h: int

@dataclass
class ScrollEvent:
    dx: int
    dy: int

@dataclass
class ClickEvent:
    x: int
    y: int

@dataclass
class KeyEvent:
    key: int
