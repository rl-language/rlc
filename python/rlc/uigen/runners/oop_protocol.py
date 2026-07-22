import queue
import socket
import threading
from abc import ABC, abstractmethod
from typing import Optional


class ProtocolError(Exception):
    pass


def _enc_scalar(v):
    if isinstance(v, bool):
        return "true" if v else "false"
    return str(v)


def _dec_scalar(s: str):
    if s == "true":
        return True
    if s == "false":
        return False
    try:
        return int(s)
    except ValueError:
        return s


def format_msg(msg: dict) -> str:
    kind = msg["type"]
    if kind == "valid_actions":
        return "valid_actions " + "|".join(msg["set"])
    if kind == "state":
        return "state " + msg["blob"].hex()
    if kind == "action":
        args = " ".join(f"{k}={_enc_scalar(v)}" for k, v in msg.get("args", {}).items())
        return f"action {msg['id']} {msg['name']} {args}".rstrip()
    if kind == "ack":
        ok = "1" if msg["ok"] else "0"
        err = "" if msg.get("error") is None else " " + str(msg["error"])
        return f"ack {msg['id']} {ok}{err}"
    return kind


def _safe_int(s, default=0):
    try:
        return int(s)
    except (ValueError, TypeError):
        return default


def parse_msg(line: str) -> dict:
    line = line.rstrip("\n")
    head, _, rest = line.partition(" ")
    if head == "valid_actions":
        return {"type": "valid_actions", "set": [s for s in rest.split("|") if s]}
    if head == "state":
        try:
            blob = bytes.fromhex(rest)
        except ValueError:
            return {"type": "state", "blob": b""}
        return {"type": "state", "blob": blob}
    if head == "action":
        aid, _, tail = rest.partition(" ")
        name, _, argstr = tail.partition(" ")
        args = {}
        for tok in argstr.split(" "):
            if not tok:
                continue
            k, _, v = tok.partition("=")
            args[k] = _dec_scalar(v)
        return {"type": "action", "id": _safe_int(aid), "name": name, "args": args}
    if head == "ack":
        aid, _, tail = rest.partition(" ")
        ok, _, err = tail.partition(" ")
        return {"type": "ack", "id": _safe_int(aid), "ok": ok == "1", "error": err or None}
    return {"type": head}


class Transport(ABC):
    @abstractmethod
    def send(self, msg: dict) -> None: ...

    @abstractmethod
    def recv(self, timeout: Optional[float] = None) -> Optional[dict]: ...

    @abstractmethod
    def recv_nowait(self) -> Optional[dict]: ...

    def close(self) -> None:
        pass


class TcpTransport(Transport):
    def __init__(self, sock: socket.socket):
        self._sock = sock
        self._inbox: "queue.Queue[Optional[dict]]" = queue.Queue()
        self._closed = False
        self._send_lock = threading.Lock()
        self._reader = threading.Thread(
            target=self._read_loop, name="tcp-transport-reader", daemon=True
        )
        self._reader.start()

    def _read_loop(self):
        buf = ""
        try:
            while not self._closed:
                chunk = self._sock.recv(65536)
                if not chunk:
                    break
                buf += chunk.decode("utf-8")
                while "\n" in buf:
                    line, buf = buf.split("\n", 1)
                    if line:
                        self._inbox.put(parse_msg(line))
        except (ConnectionError, OSError):
            pass
        finally:
            self._inbox.put(None)

    def send(self, msg: dict) -> None:
        if self._closed:
            raise ConnectionError("transport closed")
        data = (format_msg(msg) + "\n").encode("utf-8")
        with self._send_lock:
            self._sock.sendall(data)

    def recv(self, timeout: Optional[float] = None) -> Optional[dict]:
        try:
            return self._inbox.get(timeout=timeout)
        except queue.Empty:
            return None

    def recv_nowait(self) -> Optional[dict]:
        try:
            return self._inbox.get_nowait()
        except queue.Empty:
            return None

    def close(self) -> None:
        if self._closed:
            return
        self._closed = True
        for fn in (lambda: self._sock.shutdown(socket.SHUT_RDWR), self._sock.close):
            try:
                fn()
            except OSError:
                pass


def listen_one(host: str, port: int, timeout: float = 30.0) -> TcpTransport:
    srv = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    srv.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    srv.bind((host, port))
    srv.listen(1)
    srv.settimeout(timeout)
    try:
        conn, _ = srv.accept()
    finally:
        srv.close()
    conn.settimeout(None)
    return TcpTransport(conn)


def connect(host: str, port: int, timeout: float = 5.0) -> TcpTransport:
    sock = socket.create_connection((host, port), timeout=timeout)
    sock.settimeout(None)
    return TcpTransport(sock)


def walk_layout(layout, path):
    node = layout
    for seg in path:
        nxt = getattr(node, "children_mapping", None)
        nxt = nxt.get(seg) if nxt else None
        if nxt is None:
            return None
        node = nxt
    return node


def _seq_size(obj):
    while True:
        if hasattr(obj, "_size"):
            s = obj._size
            return s if isinstance(s, int) else getattr(s, "value", s)
        if hasattr(obj, "_data"):
            obj = obj._data
            continue
        return len(obj)


def _unwrap_scalar(obj):
    fields = getattr(obj, "_fields_", None)
    if fields is None:
        return obj
    public = [f for f in fields if not f[0].startswith("_")]
    if len(public) == 1 and public[0][0] == "value":
        return obj.value
    return obj


def resolve_value(state_obj, path):
    obj = state_obj
    for seg in path:
        if seg == "#":
            return _seq_size(obj)
        if isinstance(seg, int):
            target = obj
            while hasattr(target, "_data"):
                target = target._data
            obj = _unwrap_scalar(target[seg])
        else:
            obj = getattr(obj, seg) if hasattr(obj, seg) else obj
    return _unwrap_scalar(obj)
