def rl_string_to_python(rl_string) -> str:
    """
    Convert an RL string representation to a Python string.
    """
    python_string = "".join(
        [chr(rl_string.get(i).contents.value) for i in range(rl_string._data._size)]
    )[:-1]
    return python_string

def rl_vector_of_strings_to_python(vec) -> list[str]:
    """
    Convert an RL vector of strings to a list of Python strings.
    """
    result: list[str] = []
    for i in range(vec.size()):
        s_ptr = vec.get(i)
        s = s_ptr.contents
        result.append(rl_string_to_python(s))
    return result