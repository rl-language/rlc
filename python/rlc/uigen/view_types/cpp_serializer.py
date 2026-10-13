from functools import singledispatchmethod
from .array_view_type import ArrayViewType
from .bint_view_type import BoundedIntViewType
from .vector_view_type import VectorViewType
from .struct_renderer import ContainerViewType
from .primitive_view_type import PrimitiveViewType
from .view_type import ViewType
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

    def write_type_unique_id(self, renderer_type: ViewType):
        if hasattr(renderer_type, "name"):
            if renderer_type.name != "":
                self.write(renderer_type.name + "ViewType")
            else:
                self.write("anon_" + str(id(renderer_type)))
        else:
            self.write(type(renderer_type).__name__)


    @singledispatchmethod
    def serialize_declaration(self, renderer_type: ViewType):
        self.write(str(renderer_type))
        raise NotImplementedError()

    @serialize_declaration.register
    def _(self, renderer_type: ArrayViewType):
        pass

    @serialize_declaration.register
    def _(self, renderer_type: BoundedIntViewType):
        pass

    @serialize_declaration.register
    def _(self, renderer_type: PrimitiveViewType):
        pass

    @serialize_declaration.register
    def _(self, renderer_type: VectorViewType):
        pass

    @serialize_declaration.register
    def _(self, renderer_type: ContainerViewType):
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
    def serialize_use(self, renderer_type: ViewType):
        self.write(str(renderer_type))
        raise NotImplementedError()

    @serialize_use.register
    def _(self, renderer_type: ArrayViewType):
        self.write("ArrayViewType<")
        self.serialize_use(renderer_type.element_renderer)
        self.write(", ")
        self.write(str(renderer_type.length))
        self.write(">")

    @serialize_use.register
    def _(self, renderer_type: BoundedIntViewType):
        self.write("BoundedIntViewType")

    @serialize_use.register
    def _(self, renderer_type: PrimitiveViewType):
        self.write("PrimitiveViewType")

    @serialize_use.register
    def _(self, renderer_type: VectorViewType):
        self.write("VectorViewType<")
        self.serialize_use(renderer_type.element_renderer)
        self.write(">")

    @serialize_use.register
    def _(self, renderer_type: ContainerViewType):
        self.write_type_unique_id(renderer_type)
