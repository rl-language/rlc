from functools import singledispatchmethod
from .array_renderer import ArrayRenderer
from .bint_renderer import BoundedIntRenderer
from .vector_renderer import VectorRenderer
from .struct_renderer import ContainerRenderer
from .primitive_renderer import PrimitiveRenderer
from .vector_renderer import VectorRenderer
from .renderable import Renderable
from dataclasses import dataclass
from typing import TextIO

class Indenter:
    def __init__(self, ctx: 'SerializationContext'):
        self.ctx = ctx

    def __enter__(self):
        self.ctx.current_indent = self.ctx.current_indent + 1

    def __exit__(self, *args):
        self.ctx.current_indent = self.ctx.current_indent - 1

@dataclass
class SerializationContext:
    outstream: TextIO
    current_indent: int = 0
    start_of_line: bool = False

    def write(self, string):
        if self.start_of_line:
            self._do_indent()
            self.start_of_line = False
        self.outstream.write(string)

    def writenl(self, string):
        self.outstream.write(string)
        self.endline()

    def endline(self):
        self.outstream.write("\n")
        self.start_of_line = True

    def _do_indent(self):
        for i in range(self.current_indent):
            self.outstream.write(" ")

    def indent(self):
        return Indenter(self)

    def write_type_unique_id(self, renderer_type: Renderable):
        if hasattr(renderer_type, "name"):
            if renderer_type.name != "":
                self.write(renderer_type.name + "Renderer")
            else:
                self.write("anon_" + str(id(renderer_type)))
        else:
            self.write(type(renderer_type).__name__)


    @singledispatchmethod
    def serialize_declaration(self, renderer_type: Renderable):
        self.write(str(renderer_type))
        raise NotImplementedError()

    @serialize_declaration.register
    def _(self, renderer_type: ArrayRenderer):
        pass

    @serialize_declaration.register
    def _(self, renderer_type: BoundedIntRenderer):
        pass

    @serialize_declaration.register
    def _(self, renderer_type: PrimitiveRenderer):
        pass

    @serialize_declaration.register
    def _(self, renderer_type: VectorRenderer):
        pass

    @serialize_declaration.register
    def _(self, renderer_type: ContainerRenderer):
        self.write("class ")
        self.write_type_unique_id(renderer_type)
        self.writenl(" {")
        with self.indent() as indenter:
            for field_name, renderer in renderer_type.field_renderers.items():
                self.serialize_use(renderer)
                self.write(" ")
                self.write(field_name)
                self.write(";")
                self.endline()
        self.writenl("};");

    @singledispatchmethod
    def serialize_use(self, renderer_type: Renderable):
        self.write(str(renderer_type))
        raise NotImplementedError()

    @serialize_use.register
    def _(self, renderer_type: ArrayRenderer):
        self.write("ArrayRenderer<")
        self.serialize_use(renderer_type.element_renderer)
        self.write(", ")
        self.write(str(renderer_type.length))
        self.write(">")

    @serialize_use.register
    def _(self, renderer_type: BoundedIntRenderer):
        self.write("BoundedIntRenderer")

    @serialize_use.register
    def _(self, renderer_type: PrimitiveRenderer):
        self.write("PrimitiveRenderer")

    @serialize_use.register
    def _(self, renderer_type: VectorRenderer):
        self.write("VectorRenderer<")
        self.serialize_use(renderer_type.element_renderer)
        self.write(">")

    @serialize_use.register
    def _(self, renderer_type: ContainerRenderer):
        self.write_type_unique_id(renderer_type)
