# rlc/renderer/factory.py
from typing import Dict
from ctypes import c_long, c_bool
from rlc.renderer.renderable import Renderable
from rlc.renderer.primitive_renderer import PrimitiveRenderer
from rlc.renderer.bint_renderer import BoundedIntRenderer
from rlc.renderer.array_renderer import ArrayRenderer
from rlc.renderer.vector_renderer import VectorRenderer
from rlc.renderer.struct_renderer import ContainerRenderer
from rlc.renderer.bounded_vector_renderer import BoundedVectorRenderer


class RendererFactory:
    """
    Stateless renderer factory used for:
    - building renderers from RLC ctypes 
    - rebuilding renderers from JSON 
    """

    _cache = {}
    @classmethod
    def from_rlc_type(cls, rlc_type, config : Dict[type, type]):
        """
        config: { rlc_type : RendererClass } for user overrides.
        """
        if config is None:
            config = {}

        # Use cache for performance
        if rlc_type in cls._cache:
            return cls._cache[rlc_type]

        name = getattr(rlc_type, "__name__", str(rlc_type))

        # 1. User-specified renderer override (for custom classes)
        custom_conf = config.get(rlc_type, {})
        custom_renderer_class = custom_conf.get("renderer")
        if custom_renderer_class is not None:
            custom_conf = {}

        def _container_renderer(renderer_cls):
            fields = {}
            for fname, ftype in getattr(rlc_type, "_fields_", []):
                child_renderer = cls.from_rlc_type(ftype, config)
                if child_renderer is None:
                    continue
                fields[fname] = child_renderer
            renderer = renderer_cls(rlc_type.__name__, fields, custom_conf)
            cls._cache[rlc_type] = renderer
            return renderer

        if "Hidden" in name and hasattr(rlc_type, "_fields_"):
            renderer_cls = custom_renderer_class or ContainerRenderer
            return _container_renderer(renderer_cls)
            
        if "BoundedVector" in name:
            renderer_cls = custom_renderer_class or BoundedVectorRenderer
            field = None
            for _, ftype in getattr(rlc_type, "_fields_", []):
                candidate = cls.from_rlc_type(ftype, config)
                if candidate is not None:
                    field = candidate
                    break
            renderer = renderer_cls(rlc_type.__name__, field, custom_conf)
            cls._cache[rlc_type] = renderer
            return renderer

        # 2. Vector
        if "Vector" in name and hasattr(rlc_type, "_fields_"):
            if custom_renderer_class:
                renderer_cls = custom_renderer_class
            else:
                renderer_cls = VectorRenderer
            element = cls._extract_vector_element(rlc_type)
            element_renderer = cls.from_rlc_type(element, config)
            renderer = renderer_cls(rlc_type.__name__, element_renderer, custom_conf)
            cls._cache[rlc_type] = renderer
            return renderer

        # 3. Array
        if hasattr(rlc_type, "_length_") and hasattr(rlc_type, "_type_"):
            if custom_renderer_class:
                renderer_cls = custom_renderer_class
            else:
                renderer_cls = ArrayRenderer
            element_renderer = cls.from_rlc_type(rlc_type._type_, config)
            renderer = renderer_cls(
                rlc_type.__name__,
                rlc_type._length_,
                element_renderer,
                custom_conf
            )
            cls._cache[rlc_type] = renderer
            return renderer

        # 4. Bounded int
        if name.startswith("BInt"):
            if custom_renderer_class:
                renderer_cls = custom_renderer_class
            else:
                renderer_cls = BoundedIntRenderer
            renderer = renderer_cls(rlc_type.__name__, custom_conf)
            cls._cache[rlc_type] = renderer
            return renderer

        # 5. Primitive
        if rlc_type in (c_long, c_bool):
            if custom_renderer_class:
                renderer_cls = custom_renderer_class
            else:
                renderer_cls = PrimitiveRenderer
            renderer = renderer_cls(rlc_type.__name__, custom_conf)
            cls._cache[rlc_type] = renderer
            return renderer

        # 6. Struct (object with fields)
        if hasattr(rlc_type, "_fields_"):
            renderer_cls = custom_renderer_class or ContainerRenderer
            return _container_renderer(renderer_cls)

        # 7. Fallback: treat as primitive
        renderer = PrimitiveRenderer(rlc_type.__name__, custom_conf)
        cls._cache[rlc_type] = renderer
        return renderer

    # Extract element type from Vector<T>
    @staticmethod
    def _extract_vector_element(rlc_type):
        visited = set()
        current = rlc_type

        while True:
            if current in visited:
                raise ValueError(f"Cannot resolve vector element type for {rlc_type}")
            visited.add(current)

            name = getattr(current, "__name__", "")

            # --- Case 1: Hidden<T> wrapper ---------------------------------------
            if name.startswith("HiddenT"):
                fields = getattr(current, "_fields_", [])
                if len(fields) != 1:
                    raise ValueError(f"Hidden type {current} has {len(fields)} fields, expected 1")
                _, underlying = fields[0]
                current = underlying
                continue

            # --- Case 2: Find _data pointer inside vector-like struct ------------
            for field_name, field_type in getattr(current, "_fields_", []):
                if field_name == "_data":
                    # Direct pointer to element = element type found
                    elem = getattr(field_type, "_type_", None)
                    if elem is not None:
                        return elem

                    # Otherwise the field is another wrapper: descend into it
                    current = field_type
                    break
            else:
                raise ValueError(f"Cannot determine element type for vector-like type: {current}")

    @staticmethod
    def from_json_file(path: str):
        import json
        with open(path, "r") as f:
            data = json.load(f)
        return Renderable.from_dict(data)

    @staticmethod
    def from_json_string(text: str):
        import json
        return Renderable.from_dict(json.loads(text))
