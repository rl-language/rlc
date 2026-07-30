from typing import Dict
import ctypes as _ctypes
from ctypes import c_long, c_bool
from rlc.uigen.view_types.primitive_view_type import PrimitiveViewType
from rlc.uigen.view_types.bint_view_type import BoundedIntViewType
from rlc.uigen.view_types.array_view_type import ArrayViewType
from rlc.uigen.view_types.vector_view_type import VectorViewType
from rlc.uigen.view_types.struct_renderer import ContainerViewType
from rlc.uigen.view_types.bounded_vector_view_type import BoundedVectorViewType
from rlc.uigen.view_types.unwrap_view_type import UnwrapViewType
from rlc.uigen.view_types.declarative import Spec, DeclarativeRenderer


class _UniversalNoOp:
    def _noop(self, *_a, **_kw): pass
    def __getattr__(self, _name):
        return self._noop


_UNIVERSAL_NOOP = _UniversalNoOp()


class ViewTypeTreeFactory:

    @classmethod
    def collect_pyobject_slots(cls, rlc_type, rlc_path=None):
        if rlc_path is None:
            rlc_path = []
        slots = []
        for fname, ftype in getattr(rlc_type, "_fields_", []):
            if fname.startswith("_"):
                continue
            child_path = rlc_path + [fname]
            if ftype is _ctypes.py_object:
                slots.append(child_path)
            elif hasattr(ftype, "_fields_"):
                slots.extend(cls.collect_pyobject_slots(ftype, child_path))
        return slots

    @staticmethod
    def install_noop_callbacks(state_obj, slots):
        for path in slots:
            parent = state_obj
            for seg in path[:-1]:
                parent = getattr(parent, seg, None)
                if parent is None:
                    break
            if parent is None:
                continue
            setattr(parent, path[-1], _UNIVERSAL_NOOP)

    @classmethod
    def collect_callback_wires(cls, rlc_type, rlc_path=None):
        if rlc_path is None:
            rlc_path = []
        wires = []
        fields = getattr(rlc_type, "_fields_", [])
        for fname, ftype in fields:
            if fname.startswith("_") or ftype is _ctypes.py_object:
                continue
            child_path = rlc_path + [fname]
            if fname == "callback":
                parent_field = rlc_path[-1] if rlc_path else None
                if parent_field:
                    for cb_fname, cb_ftype in getattr(ftype, "_fields_", []):
                        if cb_ftype is _ctypes.py_object:
                            wires.append((child_path + [cb_fname], parent_field))
            else:
                wires.extend(cls.collect_callback_wires(ftype, child_path))
        return wires

    @classmethod
    def from_rlc_type(cls, rlc_type, config : Dict[type, type]={}, interaction_ctx=None, rlc_path=None):

        if rlc_path is None:
            rlc_path = []

        if rlc_type is _ctypes.py_object:
            return None

        name = getattr(rlc_type, "__name__", str(rlc_type))

        def _apply_interactions(renderer, current_path):
            if interaction_ctx:
                mappings = interaction_ctx.resolve_interactions(id(renderer), current_path)
                renderer.interaction_mappings = mappings
            return renderer

        custom_renderer_class = config.get(name)

        if isinstance(custom_renderer_class, Spec) and hasattr(rlc_type, "_fields_"):
            fields = DeclarativeRenderer.create_fields(
                cls.from_rlc_type, rlc_type, rlc_path, config, interaction_ctx)
            renderer = DeclarativeRenderer(rlc_type.__name__, custom_renderer_class, fields)
            return _apply_interactions(renderer, rlc_path)

        def _container_renderer(renderer_cls):
            fields = renderer_cls.create_fields(cls.from_rlc_type, rlc_type, rlc_path, config, interaction_ctx)
            if not fields:
                return None
            renderer = renderer_cls(rlc_type.__name__, fields)
            return _apply_interactions(renderer, rlc_path)
        
        # 1. Hidden wrapper
        if name.startswith("HiddenInformation") and hasattr(rlc_type, "_fields_") and not custom_renderer_class:
            inner = None
            for fname, ftype in getattr(rlc_type, "_fields_", []):
                if fname == "value":
                    inner = ftype
                    break
            if inner is not None:
                inner_renderer = cls.from_rlc_type(inner, config, interaction_ctx, rlc_path)
                if inner_renderer is None:
                    return None
                renderer = UnwrapViewType(rlc_type.__name__, "value", inner_renderer)
                return _apply_interactions(renderer, rlc_path)

        if ("Hidden" in name or name.endswith("Style")) and hasattr(rlc_type, "_fields_"):
            return None
            
        # 2. BoundedVector
        if "Bounded" in name and cls._is_vector(rlc_type):
            renderer_cls = custom_renderer_class or BoundedVectorViewType
            field = None
            for fname, ftype in getattr(rlc_type, "_fields_", []):
                child_path = rlc_path
                candidate = cls.from_rlc_type(ftype, config, interaction_ctx, child_path)
                if candidate is not None:
                    field = candidate
                    break
            renderer = renderer_cls(rlc_type.__name__, field)
            return _apply_interactions(renderer, rlc_path)

        # 3a. RLC String — render as plain text, not a byte vector
        if name == "String":
            renderer = custom_renderer_class(rlc_type.__name__) if custom_renderer_class else PrimitiveViewType(rlc_type.__name__)
            return _apply_interactions(renderer, rlc_path)

        # 3. Vector-like containers
        if cls._is_vector(rlc_type):
            if custom_renderer_class:
                renderer_cls = custom_renderer_class
            else:
                renderer_cls = VectorViewType
            element = cls._extract_vector_element(rlc_type)
            element_renderer = cls.from_rlc_type(element, config, interaction_ctx, rlc_path + ["$i"])
            renderer = renderer_cls(rlc_type.__name__, element_renderer)
            return _apply_interactions(renderer, rlc_path)

        # 4. Array
        if hasattr(rlc_type, "_length_") and hasattr(rlc_type, "_type_"):
            if custom_renderer_class:
                renderer_cls = custom_renderer_class
            else:
                renderer_cls = ArrayViewType
            element_renderer = cls.from_rlc_type(rlc_type._type_, config, interaction_ctx, rlc_path + ["$i"])
            renderer = renderer_cls(
                rlc_type.__name__,
                rlc_type._length_,
                element_renderer
            )
            return _apply_interactions(renderer, rlc_path)

        # 5. Bounded int
        if name.startswith("BInt"):
            if custom_renderer_class:
                renderer_cls = custom_renderer_class
            else:
                renderer_cls = BoundedIntViewType
            renderer = renderer_cls(rlc_type.__name__)
            return _apply_interactions(renderer, rlc_path)

        # 6. Primitive
        if rlc_type in (c_long, c_bool):
            if custom_renderer_class:
                renderer_cls = custom_renderer_class
            else:
                renderer_cls = PrimitiveViewType
            renderer = renderer_cls(rlc_type.__name__)
            return _apply_interactions(renderer, rlc_path)

        # 7. Struct 
        if hasattr(rlc_type, "_fields_"):
            renderer_cls = custom_renderer_class or ContainerViewType
            if hasattr(renderer_cls, "create_fields"):
                return _container_renderer(renderer_cls)
            renderer = renderer_cls(rlc_type.__name__)
            return _apply_interactions(renderer, rlc_path)

        # 8. Fallback: treat as primitive
        renderer = PrimitiveViewType(rlc_type.__name__)
        return _apply_interactions(renderer, rlc_path)

    @staticmethod
    def _is_vector(rlc_type) -> bool:
        name = getattr(rlc_type, "__name__", "")
        fields = getattr(rlc_type, "_fields_", [])
        field_names = {fname for fname, _ in fields}

        if "_data" in field_names and "_size" in field_names:
            return True

        return "Vector" in name and hasattr(rlc_type, "_fields_")

    @staticmethod
    def _extract_vector_element(rlc_type):
        visited = set()
        current = rlc_type

        while True:
            if current in visited:
                raise ValueError(f"Cannot resolve vector element type for {rlc_type}")
            visited.add(current)

            name = getattr(current, "__name__", "")

            if name.startswith("HiddenT"):
                fields = getattr(current, "_fields_", [])
                if len(fields) != 1:
                    raise ValueError(f"Hidden type {current} has {len(fields)} fields, expected 1")
                _, underlying = fields[0]
                current = underlying
                continue

            for field_name, field_type in getattr(current, "_fields_", []):
                if field_name == "_data":
                    elem = getattr(field_type, "_type_", None)
                    if elem is not None:
                        return elem
                    current = field_type
                    break
            else:
                raise ValueError(f"Cannot determine element type for vector-like type: {current}")

    @staticmethod
    def apply_callback_wires(wires, state_obj, renderer):
        for state_path, renderer_field in wires:
            parent = state_obj
            for seg in state_path[:-1]:
                parent = getattr(parent, seg, None)
                if parent is None:
                    break
            if parent is None:
                continue
            renderer_node = getattr(renderer, renderer_field, None)
            if renderer_node is not None:
                setattr(parent, state_path[-1], renderer_node)
