from command_line import load_program_from_args, make_rlc_argparse
from rlc import Program
from rlc.renderer_type_conversion import create_renderer
import sys
from dataclasses import asdict
import yaml

if __name__ == "__main__":
    parser = make_rlc_argparse("rlc_to_layout", description="Dumps the layout and exits")
    parser.add_argument("-o", default="-", nargs="?")
    args = parser.parse_args()
    output = sys.stdout if args.o == "-" else open(args.o, "w+")
    with load_program_from_args(args, optimize=True) as program:
        config = {}
        renderer = create_renderer(program.module.Game, config)
        output.write(yaml.dump(asdict(renderer)))
