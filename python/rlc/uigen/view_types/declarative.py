from dataclasses import dataclass, field as dataclass_field
from typing import Callable, List, Optional

from rlc.uigen.layout import (
    Layout, Direction, FIT, GROW, Padding, SizePolicy, SizePolicies,
)
from rlc.uigen.view_types.view_type import (
    ViewType, register_renderer, get_field, has_field, seq_at,
    ViewTypeDumper, ViewTypeLoader,
)
from rlc.uigen.view_types.struct_renderer import ContainerViewType


def _spec_representer(dumper, obj):
    return dumper.represent_mapping(f"!Spec:{type(obj).__name__}", vars(obj))


def _make_spec_constructor(cls):
    def _construct(loader, node):
        obj = object.__new__(cls)
        obj.__dict__.update(loader.construct_mapping(node, deep=True))
        return obj
    return _construct


def _padding_representer(dumper, obj):
    return dumper.represent_mapping("!Padding", vars(obj))


def _padding_constructor(loader, node):
    return Padding(**loader.construct_mapping(node, deep=True))


def _direction_representer(dumper, obj):
    return dumper.represent_scalar("!Direction", obj.value)


def _direction_constructor(loader, node):
    return Direction(loader.construct_scalar(node))


def _sizepolicy_representer(dumper, obj):
    return dumper.represent_mapping(
        "!SizePolicy", {"policy": obj.size_policy.value, "value": obj.value})


def _sizepolicy_constructor(loader, node):
    m = loader.construct_mapping(node, deep=True)
    return SizePolicy(SizePolicies(m["policy"]), m.get("value"))


ViewTypeDumper.add_representer(Padding, _padding_representer)
ViewTypeDumper.add_representer(Direction, _direction_representer)
ViewTypeDumper.add_representer(SizePolicy, _sizepolicy_representer)
ViewTypeLoader.add_constructor("!Padding", _padding_constructor)
ViewTypeLoader.add_constructor("!Direction", _direction_constructor)
ViewTypeLoader.add_constructor("!SizePolicy", _sizepolicy_constructor)


def _stamp_highlight(node, predicate):
    if getattr(node, "on_click", None):
        node._highlight_when = predicate
    for child in getattr(node, "children", []):
        _stamp_highlight(child, predicate)


class _ElementHolder:
    def __init__(self):
        self.children_mapping = {}
        self.children = []
        self.full_subscribers = []

    @property
    def update_fn(self):
        def _update(collection):
            for i, element in self.children_mapping.items():
                if element.update_fn:
                    element.update_fn(seq_at(collection, i))
            for node in self.full_subscribers:
                if node.update_fn:
                    node.update_fn(collection)
        return _update


class Spec:
    def __init_subclass__(cls, **kwargs):
        super().__init_subclass__(**kwargs)
        ViewTypeLoader.add_constructor(
            f"!Spec:{cls.__name__}", _make_spec_constructor(cls))

    def child_specs(self) -> List["Spec"]:
        return []


ViewTypeDumper.add_multi_representer(Spec, _spec_representer)


class Panel(Spec):
    def __init__(self, *children, direction=Direction.COLUMN, color="#F6F0ED",
                 title_color="#750D37", child_gap=8,
                 padding=Padding(12, 12, 14, 14), border=0, sizing=None):
        self.children = children
        self.direction = direction
        self.color = color
        self.title_color = title_color
        self.child_gap = child_gap
        self.padding = padding
        self.border = border
        self.sizing = sizing

    def child_specs(self):
        return [c for c in self.children if isinstance(c, Spec)]


@dataclass
class Text(Spec):
    text: str
    font_name: str = "Arial"
    font_size: int = 18
    color: Optional[str] = None


@dataclass
class Field(Spec):
    name: str
    label: Optional[str] = None
    direction: Optional[Direction] = None
    label_color: str = "#222222"
    label_font_size: int = 16
    bg: Optional[str] = None
    index: Optional[int] = None
    cell: Optional["Widget"] = None
    button: bool = False
    visible_when: Optional[Callable] = None
    highlight_when: Optional[Callable] = None


@dataclass
class Widget(Spec):
    field: str = ""

    def build(self, value) -> Layout:
        raise NotImplementedError

    def refresh(self, layout, value) -> None:
        pass


@dataclass
class ActionButton(Widget):
    label: str = ""
    action: str = ""
    args: dict = dataclass_field(default_factory=dict)
    bg: str = "#3B7A57"

    def build(self, value) -> Layout:
        from rlc.uigen.text import Text as _Text
        node = Layout(sizing=(FIT(), FIT()), direction=Direction.ROW,
                      color=self.bg, border=2, padding=Padding(10, 20, 10, 20),
                      align="center")
        node._no_blend = True
        node.on_click = {"handler": self.action, "args": dict(self.args)}
        node.interactive = True
        node.add_child(_Text(self.label or self.action, "Arial", 18, "#FFFFFF"))
        return node


@register_renderer
@dataclass
class DeclarativeRenderer(ViewType):
    spec: Spec = None
    field_renderers: dict = dataclass_field(default_factory=dict)

    @staticmethod
    def create_fields(factory, rlc_type, rlc_path, config, interaction_ctx):
        return ContainerViewType.create_fields(
            factory, rlc_type, rlc_path, config, interaction_ctx)

    def _field_renderer(self, name):
        data = self.field_renderers.get(name)
        return data[1] if data else None

    def build_layout(self, obj, parent_path, direction=Direction.COLUMN,
                     color=None, sizing=(FIT(), FIT()), logger=None,
                     padding=Padding(18, 18, 18, 18), index_bindings=None):
        if index_bindings is None:
            index_bindings = {}

        if isinstance(self.spec, Field):
            root = self._build_field(self.spec, obj, parent_path, index_bindings)
            root.children_mapping[self.spec.name] = root
            root.render_path = parent_path
            root.update_fn = lambda v: self.update(root, v)
            return root

        if isinstance(self.spec, Widget):
            root = self._build_widget(self.spec, obj)
            root.render_path = parent_path
            root.update_fn = lambda v: self.spec.refresh(root, v)
            self._apply_interaction_mappings(root, index_bindings)
            return root

        root = self._build_panel(self.spec, obj, parent_path, index_bindings,
                                 root_map=None)
        root.render_path = parent_path
        root.update_fn = lambda v: self.update(root, v)
        return root

    def _build_spec(self, spec, obj, parent_path, index_bindings, root_map):
        if isinstance(spec, Panel):
            return self._build_panel(spec, obj, parent_path, index_bindings, root_map)
        if isinstance(spec, Text):
            return self.make_text(spec.text, spec.font_name, spec.font_size,
                                  spec.color or "#222222")
        if isinstance(spec, Widget):
            return self._build_widget(spec, obj)
        if isinstance(spec, Field):
            return self._build_field(spec, obj, parent_path, index_bindings)
        raise TypeError(f"Unknown spec node: {spec!r}")

    def _build_panel(self, panel, obj, parent_path, index_bindings, root_map):
        layout = Layout(sizing=panel.sizing or (GROW(), FIT()),
                        direction=panel.direction,
                        color=panel.color, border=panel.border,
                        child_gap=panel.child_gap, padding=panel.padding)
        layout.render_path = parent_path
        if root_map is None:
            root_map = layout.children_mapping
        for child in panel.child_specs():
            if isinstance(child, Field):
                value = self._build_field(child, obj, parent_path, index_bindings)
                if child.button:
                    built = self._button(child, value)
                elif child.label:
                    built = self._chip(child, value)
                else:
                    built = value
                self._register_field(root_map, child, value)
                if child.visible_when is not None:
                    built._visible_when = child.visible_when
                    built.collapsed = not child.visible_when(obj)
            elif isinstance(child, Widget):
                built = self._build_widget(child, obj)
                if child.field:
                    self._register_widget_field(root_map, child.field, built)
            else:
                built = self._build_spec(child, obj, parent_path, index_bindings, root_map)
                if isinstance(child, Text) and child.color is None:
                    built.color = panel.title_color
            layout.add_child(built)
        return layout

    def _chip(self, field, value):
        chip = Layout(sizing=(FIT(), FIT()), direction=Direction.ROW,
                      color=None, border=0, child_gap=6, padding=Padding(2, 2, 2, 2))
        chip.add_child(self.make_text(field.label + ":", "Arial",
                                      field.label_font_size, field.label_color))
        chip.add_child(value)
        return chip

    def _button(self, field, value):
        click = self._find_on_click(value)
        value.sizing = (FIT(), FIT())
        value.direction = Direction.ROW
        value.color = field.bg or "#3B7A57"
        value.border = 2
        value.padding = Padding(10, 20, 10, 20)
        value.align = "center"
        value._no_blend = True
        value.children = []
        value.children_mapping = {}
        if click is not None:
            value.on_click = click
            value.interactive = True
        value.add_child(self.make_text(field.label or field.name, "Arial",
                                       field.label_font_size + 2, "#FFFFFF"))
        return value

    def _find_on_click(self, node):
        if getattr(node, "on_click", None):
            return node.on_click
        for child in getattr(node, "children", []):
            found = self._find_on_click(child)
            if found is not None:
                return found
        return None

    def _register_field(self, root_map, field, value):
        if field.index is None:
            root_map[field.name] = value
            return
        holder = self._ensure_holder(root_map, field.name)
        holder.children_mapping[field.index] = value

    def _register_widget_field(self, root_map, name, node):
        if root_map.get(name) is None:
            root_map[name] = node
            return
        holder = self._ensure_holder(root_map, name)
        holder.full_subscribers.append(node)

    def _ensure_holder(self, root_map, name):
        existing = root_map.get(name)
        if isinstance(existing, _ElementHolder):
            return existing
        holder = _ElementHolder()
        if existing is not None:
            holder.full_subscribers.append(existing)
        root_map[name] = holder
        return holder

    def _build_field(self, spec, obj, parent_path, index_bindings):
        renderer = self._field_renderer(spec.name)
        if renderer is None:
            raise KeyError(
                f"Field('{spec.name}') has no renderable field on "
                f"{self.rlc_type_name}; known fields: "
                f"{sorted(self.field_renderers)}")
        if spec.cell is not None:
            self._install_cell(renderer, spec.cell)
        collection = get_field(obj, spec.name)
        if spec.index is not None:
            renderer = getattr(renderer, "element_renderer", None)
            if renderer is None:
                raise TypeError(
                    f"Field('{spec.name}', index={spec.index}) is not a "
                    f"collection field")
            collection = seq_at(collection, spec.index)
            child_path = parent_path + [spec.name, spec.index]
            deepest = renderer._get_deepest_interaction_mappings()
            if deepest:
                index_vars = deepest[0].index_vars
                num_bound = len(index_bindings)
                if num_bound < len(index_vars):
                    index_bindings = {**index_bindings, index_vars[num_bound]: spec.index}
        else:
            child_path = parent_path + [spec.name]
        kwargs = {"parent_path": child_path, "index_bindings": index_bindings}
        if spec.direction is not None:
            kwargs["direction"] = spec.direction
        layout = renderer(collection, **kwargs)
        if spec.bg is not None:
            self._blend(layout, spec.bg)
        if spec.highlight_when is not None:
            _stamp_highlight(layout, spec.highlight_when)
        return layout

    _CELL_LINKS = ("element_renderer", "vector_view_type", "inner")

    def _cell_link(self, node):
        for attr in self._CELL_LINKS:
            child = getattr(node, attr, None)
            if child is not None:
                return attr, child
        return None, None

    def _install_cell(self, renderer, cell):
        parent, attr = None, None
        node = renderer
        while True:
            link_attr, child = self._cell_link(node)
            if child is None:
                break
            parent, attr, node = node, link_attr, child
        if parent is None:
            raise TypeError("cell= requires a collection field")
        if isinstance(node, DeclarativeRenderer) and node.spec is cell:
            return
        wrapped = DeclarativeRenderer(node.rlc_type_name, cell)
        wrapped.interaction_mappings = node.interaction_mappings
        setattr(parent, attr, wrapped)

    @staticmethod
    def _blend(node, color):
        from rlc.uigen.image import Image
        from rlc.uigen.text import Text
        if isinstance(node, (Image, Text)) or getattr(node, "_no_blend", False):
            return
        node.color = color
        node.border = 0
        for child in node.children:
            DeclarativeRenderer._blend(child, color)

    def _widget_value(self, widget, obj):
        return get_field(obj, widget.field) if widget.field else obj

    def _build_widget(self, widget, obj):
        node = widget.build(self._widget_value(widget, obj))
        node.update_fn = lambda v: widget.refresh(
            node, get_field(v, widget.field) if widget.field and has_field(v, widget.field) else v)
        return node

    def update(self, layout, obj):
        self._update_spec(self.spec, layout, obj)

    def _update_spec(self, spec, layout, obj):
        if isinstance(spec, Field):
            renderer = self._field_renderer(spec.name)
            if renderer is not None:
                value = get_field(obj, spec.name) if has_field(obj, spec.name) else obj
                renderer.update(layout, value)
        elif isinstance(spec, Widget):
            spec.refresh(layout, self._widget_value(spec, obj))
        elif isinstance(spec, Panel):
            self._update_panel(spec, layout, obj)

    def _update_panel(self, panel, layout, obj):
        for idx, child in enumerate(panel.child_specs()):
            built = layout.children[idx] if idx < len(layout.children) else None
            if built is None:
                continue
            if isinstance(child, Field):
                if child.button:
                    continue
                renderer = self._field_renderer(child.name)
                target = built.children[-1] if child.label else built
                value = get_field(obj, child.name)
                if child.index is not None:
                    renderer = getattr(renderer, "element_renderer", None)
                    value = seq_at(value, child.index)
                if renderer is not None:
                    renderer.update(target, value)
            elif isinstance(child, Widget):
                child.refresh(built, self._widget_value(child, obj))
            elif isinstance(child, Panel):
                self._update_panel(child, built, obj)

    def __getattr__(self, name):
        if name not in ("field_renderers", "spec") and "field_renderers" in self.__dict__:
            fr = self.__dict__["field_renderers"]
            if name in fr and fr[name] is not None:
                return fr[name][1]
            for field_data in fr.values():
                if field_data and field_data[0] == name:
                    return field_data[1]
        raise AttributeError(f"'{type(self).__name__}' has no field renderer '{name}'")

    def _iter_children(self):
        return [r for _, r in self.field_renderers.values() if r is not None]
