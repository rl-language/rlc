import hashlib


def signature(state) -> str:
    return hashlib.sha1(str(state.state).encode()).hexdigest()
