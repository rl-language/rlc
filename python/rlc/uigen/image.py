from .layout import Layout, FIXED
from .view_backend import ViewBackend
from typing import Optional, Tuple


class Image(Layout):

    def __init__(
        self,
        size: Tuple[int, int],
        image_path: Optional[str] = None,
        tint: Optional[str] = None,
        label: str = "",
        sub_label: str = "",
        color: Optional[str] = None,
    ):
        w, h = size
        super().__init__(sizing=(FIXED(w), FIXED(h)), color=color, border=0)
        self.image_path = image_path
        self.tint = tint
        self.label = label
        self.sub_label = sub_label
        self._size = (w, h)

    def compute_size(self, available_width=None, available_height=None, logger=None, backend: Optional[ViewBackend] = None):
        self.width, self.height = self._size
        if logger:
            logger.snapshot(self, "image_compute")

    _UNSET = object()

    def update_image(self, image_path=_UNSET, tint=None, label=None, sub_label=None):
        if image_path is not Image._UNSET:
            self.image_path = image_path
        if tint is not None:
            self.tint = tint
        if label is not None:
            self.label = label
        if sub_label is not None:
            self.sub_label = sub_label
