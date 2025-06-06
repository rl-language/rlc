# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information
from sphinx.highlighting import lexers
from pygments.lexer import RegexLexer, bygroups
from pygments.token import Keyword, Name, Operator, Number, String, Comment, Text, Literal, Punctuation

project = 'Rulebook documentation'
copyright = '2025, Massimo Fioravanti'
author = 'Massimo Fioravanti'
release = '0.3.0'

class RLCLexer(RegexLexer):
    name = 'RLC'
    aliases = ['rlc']
    filenames = ['*.rlc']

    tokens = {
        'root': [
            # Comments
            (r'#.*', Comment.Single),

            (r'[\@()\[\]{}.,;:\|]', Punctuation),

            # Top-level keywords
            (r'\b(subaction|act|fun|trait|cls|enum|import|actions|ref|frm|ctx)\b', Keyword.Namespace),

            # Control flow
            (r'\b(const|in|if|else|while|is|of|return|let|for|req|using|break|continue)\b', Keyword.Reserved),

            # Boolean literals
            (r'\b(true|false)\b', Literal),

            # Operators (surrounded by whitespace or not)
            (r'(\+|\-|\*|/|==|=|<=|>=|<|>|\!|\band\b|\bor\b)', Operator),

            # Numbers (integers and floats)
            (r'\b\d+\.\d+\b', Number.Float),
            (r'\b\d+\b', Number.Integer),

            # String literal
            (r'"[^"\n]*"', String.Double),

            # Char literal
            (r"'[^']'", String.Char),

            # Type names (start with uppercase letter)
            (r'\b[A-Z][a-zA-Z0-9]*\b', Name.Class),

            # Anything else
            (r'\s+', Text),
            (r'\w+', Name),
        ],
    }


lexers['rlc'] = RLCLexer()

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = [
        'myst_parser',
        'sphinx.ext.graphviz',
        'sphinx.ext.githubpages',
        'sphinxcontrib.tikz',
        ]

templates_path = ['_templates']
exclude_patterns = []



# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = "sphinx_rtd_theme"
html_static_path = ['_static']
html_css_files = ['custom.css']
tikz_latex_preamble  = r"\usetikzlibrary{arrows.meta,calc}"
tikz_work_path       = "_build/.tikz"

