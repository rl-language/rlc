from command_line import load_program_from_args, make_rlc_argparse
from rlc import Program
from rlc.renderer.renderable import Renderable
from rlc.renderer_type_conversion import create_renderer
import sys
from dataclasses import asdict
from rlc.renderer import SerializationContext

if __name__ == "__main__":
    parser = make_rlc_argparse("rlc_to_layout", description="Dumps the layout and exits")
    parser.add_argument("-o", default="-", nargs="?")
    parser.add_argument("-cpp", action="store_true")
    args = parser.parse_args()
    output = sys.stdout if args.o == "-" else open(args.o, "w+")
    with load_program_from_args(args, optimize=True) as program:
        config = {}
        renderer = create_renderer(program.module.Game, config)
        if args.cpp:
            ctx = SerializationContext(output)
            for renderer in renderer.post_order_types():
                ctx.serialize_declaration(renderer)
        else:
            output.write(renderer.to_yaml())
