from typing import Type
from typing import Dict

from ctypes import c_long, Array, c_bool
from rlc.renderer.primitive_renderer import PrimitiveRenderer
from rlc.renderer.array_renderer import ArrayRenderer
from rlc.renderer.vector_renderer import VectorRenderer
from rlc.renderer.struct_renderer import ContainerRenderer
from rlc.renderer.bint_renderer import BoundedIntRenderer


def create_renderer(rlc_type, config : Dict[type, type]):
    _renderer_cache = {}
    def _create_renderer(rlc_type, config : Dict[type, type]):
        if rlc_type in _renderer_cache:
            return _renderer_cache[rlc_type]

        name = getattr(rlc_type, "__name__", str(rlc_type))

        if rlc_type in config:
            pytyp = config.get(rlc_type)
            if hasattr(rlc_type, "_fields_"):
                field_renderer = {
                    name: _create_renderer(field_type, config) for name, field_type in rlc_type._fields_
                }
                renderer = pytyp(rlc_type, field_renderer)
                return renderer

        # Vector
        elif "Vector" in name:
            # find the _data field to determine element type
            data_field = next((f for f in getattr(rlc_type, "_fields_", []) if f[0] == "_data"), None)
            if data_field:
                element_type = getattr(data_field[1], "_type_", None)
                if element_type is None:
                    element_type = data_field[1]
                element_renderer = _create_renderer(element_type, config)
                renderer = VectorRenderer(element_renderer)
            else:
                renderer = PrimitiveRenderer()


        # Array
        elif hasattr(rlc_type, "_length_") and hasattr(rlc_type, "_type_"):
            element_rendere = _create_renderer(rlc_type._type_, config)
            renderer = ArrayRenderer(rlc_type._length_, element_rendere)

        # Primitive
        elif rlc_type == c_bool or rlc_type == c_long:
            renderer = PrimitiveRenderer()

        # Bounded integer
        elif name.startswith("BInt"):
            renderer = BoundedIntRenderer()

        # Struct
        elif hasattr(rlc_type, "_fields_"):
            field_renderer = {
                name: _create_renderer(field_type, config) for name, field_type in rlc_type._fields_
            }
            renderer = ContainerRenderer(name, field_renderer)
        else:
            renderer = PrimitiveRenderer(name)
        _renderer_cache[rlc_type] = renderer
        return renderer
    return _create_renderer(rlc_type, config)
