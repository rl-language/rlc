from dataclasses import dataclass, field
from typing import List, Optional
from enum import Enum
import yaml
import os

_VALID_EVENTS = {"on_click", "on_hover", "on_key"} 


class SegmentKind(str, Enum):
    ROOT = "root"
    FIELD = "field"
    INDEX_VAR = "index_var"
    EVENT = "event"
    PARAM_VAR = "param_var"

@dataclass(frozen=True)
class PathSegment:
    kind: SegmentKind
    value: str 

@dataclass(frozen=True)
class ParsedPath:
    raw: str
    segments: List[PathSegment]

    @property
    def event(self) -> str:
        for seg in self.segments:
            if seg.kind == SegmentKind.EVENT:
                return seg.value
        raise ValueError(f"ParsedPath has no EVENT segment: {self.raw}")

    @property
    def param_vars(self) -> List[str]:
        return [seg.value for seg in self.segments if seg.kind == SegmentKind.PARAM_VAR]

    @property
    def index_vars(self) -> List[str]:
        return [seg.value for seg in self.segments if seg.kind == SegmentKind.INDEX_VAR]


@dataclass
class InteractionMapping:
    event_type: str 
    handler_name: str
    index_vars: List[str]  
    param_vars: List[str] 
    path: List[str]  


@dataclass
class InteractionContext:
    config_rules: List[tuple[ParsedPath, str]] = field(default_factory=list)

    @classmethod
    def from_config_file(cls, config_path: Optional[str] = None) -> 'InteractionContext':
        config_dict = _load_config_file(config_path)

        rules = []
        for path_str, handler_name in config_dict.items():
            parsed_path = parse_config_path(path_str)
            rules.append((parsed_path, handler_name))

        return cls(config_rules=rules)

    def resolve_interactions(self, _renderer_id: int, rlc_path: List[str]) -> List[InteractionMapping]:
        mappings = []

        for parsed_path, handler_name in self.config_rules:
            if self._matches_pattern(parsed_path, rlc_path):
                event_type = parsed_path.event
                readable_path = self._make_readable_path(rlc_path, parsed_path.index_vars)

                mappings.append(InteractionMapping(
                    event_type=event_type,
                    handler_name=handler_name,
                    index_vars=parsed_path.index_vars,
                    param_vars=parsed_path.param_vars,
                    path=readable_path,
                ))

        return mappings

    def _matches_pattern(self, parsed_path: ParsedPath, rlc_path: List[str]) -> bool:
        segments = parsed_path.segments
        event_index = None
        for i, seg in enumerate(segments):
            if seg.kind == SegmentKind.EVENT:
                event_index = i
                break

        if event_index is None:
            return False

        pattern_segments = segments[:event_index]

        if len(pattern_segments) != len(rlc_path):
            return False

        for seg, path_value in zip(pattern_segments, rlc_path):
            if seg.kind == SegmentKind.ROOT:
                if path_value != seg.value:
                    return False
            elif seg.kind == SegmentKind.FIELD:
                if path_value != seg.value:
                    return False
            elif seg.kind == SegmentKind.INDEX_VAR:
                if not isinstance(path_value, int) and path_value != '$i':
                    return False

        return True

    def _make_readable_path(self, path: List[str], index_vars: List[str]) -> List[str]:
        readable_path = []
        var_index = 0

        for segment in path:
            if segment == '$i':
                if var_index < len(index_vars):
                    readable_path.append(f'${index_vars[var_index]}')
                    var_index += 1
                else:
                    readable_path.append(segment)
            else:
                readable_path.append(segment)

        return readable_path


def _load_config_file(config_path: Optional[str] = None) -> dict:
    if config_path is None:
        search_paths = [
            "interactions.yaml",  
            "test/uigen/interactions.yaml",  
            "python/test/uigen/interactions.yaml",  
            os.path.join(os.path.dirname(__file__), "../../../test/uigen/interactions.yaml"),
        ]
        for path in search_paths:
            if os.path.exists(path):
                config_path = path
                break
        else:
            return {}

    if not os.path.exists(config_path):
        return {}

    with open(config_path, 'r') as f:
        config = yaml.safe_load(f)

    return config if config else {}


def parse_config_path(path: str) -> ParsedPath:
    raw = path
    path = path.strip().strip("/") 

    if not path:
        raise ValueError("Empty config path")

    parts = [p for p in path.split("/") if p]
    if len(parts) < 2:
        raise ValueError(f"Path too short: '{raw}'")

    event_index = None
    for i, part in enumerate(parts):
        if part in _VALID_EVENTS:
            event_index = i
            break

    if event_index is None:
        raise ValueError(
            f"No valid event found in '{raw}'. "
            f"Allowed: {sorted(_VALID_EVENTS)}"
        )

    root = parts[0]
    event = parts[event_index]
    middle = parts[1:event_index]  
    params = parts[event_index + 1:]  

    segments: List[PathSegment] = [PathSegment(SegmentKind.ROOT, root)]

    for seg in middle:
        if seg.startswith("$"):
            var = seg[1:]
            if not var:
                raise ValueError(f"Empty variable segment in '{raw}'")
            segments.append(PathSegment(SegmentKind.INDEX_VAR, var))
            continue

        segments.append(PathSegment(SegmentKind.FIELD, seg))

    segments.append(PathSegment(SegmentKind.EVENT, event))

    for seg in params:
        if not seg.startswith("$"):
            raise ValueError(
                f"Parameter '{seg}' must start with '$' in '{raw}'"
            )
        var = seg[1:]
        if not var:
            raise ValueError(f"Empty parameter segment in '{raw}'")
        segments.append(PathSegment(SegmentKind.PARAM_VAR, var))

    return ParsedPath(raw=raw, segments=segments)



