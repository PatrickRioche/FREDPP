#!/usr/bin/env python3
"""Regenerate and exhaustively verify the FREDPP Tic-tac-toe solo policy.

No third-party dependencies. Minimax is used only at generation time;
the shipped .fredpp file plays using FREDPP pattern tests (T) and jumps (J).

Usage:
  python scripts/generate_tictactoe_policy.py --check
  python scripts/generate_tictactoe_policy.py --write
"""
from __future__ import annotations

import argparse
from collections import Counter
from functools import lru_cache
from pathlib import Path

LINES = ((0, 1, 2), (3, 4, 5), (6, 7, 8),
         (0, 3, 6), (1, 4, 7), (2, 5, 8),
         (0, 4, 8), (2, 4, 6))
PREFERENCE = (4, 0, 2, 6, 8, 1, 3, 5, 7)
BEGIN = '" BEGIN MINIMAX POLICY\n'
END = '" END MINIMAX POLICY'


def winner(board: str) -> str | None:
    for a, b, c in LINES:
        if board[a] != "." and board[a] == board[b] == board[c]:
            return board[a]
    return "D" if "." not in board else None


def placed(board: str, square: int, player: str) -> str:
    if board[square] != ".":
        raise ValueError("attempt to occupy an already played square")
    return board[:square] + player + board[square + 1:]


@lru_cache(maxsize=None)
def minimax(board: str, to_move: str) -> int:
    """Score relative to the computer: O wins +1, draw 0, X wins -1."""
    end = winner(board)
    if end is not None:
        return {"O": 1, "D": 0, "X": -1}[end]
    scores = (minimax(placed(board, i, to_move),
                      "X" if to_move == "O" else "O")
              for i in range(9) if board[i] == ".")
    return max(scores) if to_move == "O" else min(scores)


def best_computer_move(board: str) -> int:
    """Optimal O move; deterministic tie-breaks favour centre then corners."""
    return max(
        (i for i in PREFERENCE if board[i] == "."),
        key=lambda i: minimax(placed(board, i, "O"), "X"),
    )


def compact_patterns(states: list[str], offset: int = 0) -> str:
    """Make an exact-match trie regex using FREDPP's (), | and [] grammar."""
    if offset == 9:
        return ""
    by_character: dict[str, list[str]] = {}
    for state in states:
        by_character.setdefault(state[offset], []).append(state)

    by_suffix: dict[str, list[str]] = {}
    for char, subset in by_character.items():
        suffix = compact_patterns(subset, offset + 1)
        by_suffix.setdefault(suffix, []).append(char)

    branches = [
        (chars[0] if len(chars) == 1 else "[" + "".join(chars) + "]") +
        suffix
        for suffix, chars in by_suffix.items()
    ]
    return branches[0] if len(branches) == 1 else "(" + "|".join(branches) + ")"


def build_policy() -> tuple[dict[str, int], Counter]:
    """Explore every legal human reply to the deterministic minimax bot."""
    policy: dict[str, int] = {}
    outcomes: Counter = Counter()

    def human_turn(board: str) -> None:
        end = winner(board)
        if end:
            outcomes[end] += 1
            return
        for i in range(9):
            if board[i] != ".":
                continue
            after_x = placed(board, i, "X")
            end = winner(after_x)
            if end:
                outcomes[end] += 1
                continue
            if after_x not in policy:
                policy[after_x] = best_computer_move(after_x)
            after_o = placed(after_x, policy[after_x], "O")
            human_turn(after_o)

    human_turn("." * 9)

    if outcomes["X"] or len(policy) != 285:
        raise AssertionError(
            "minimax is not unbeaten: " +
            f"X={outcomes['X']}, states={len(policy)}"
        )
    return policy, outcomes


def render_body(policy: dict[str, int]) -> str:
    cases: list[list[str]] = [[] for _ in range(9)]
    for board, square in policy.items():
        compact_board = "".join(c if c != "." else str(i + 1)
                                for i, c in enumerate(board))
        cases[square].append(compact_board)

    parts: list[str] = []
    for i, states in enumerate(cases, 1):
        if not states:
            raise AssertionError("unexpected empty target in policy")
        parts.append(f"1T/^{compact_patterns(states)}$/\nJ(tt_ai{i})T\n")
    parts.append(
        "JM!Erreur interne : position non couverte par la strategie Minimax.!\n"
        "JO\n\n"
    )
    for i in range(1, 10):
        parts.append(
            f"@(tt_ai{i})\nB(tt_choice)\n1S/^.*$/{i}/\n"
            "J(tt_ai_play)\n\n"
        )
    parts.append(
        "@(tt_ai_play)\n"
        "JM!FREDPP (O) choisit la case \\S(tt_choice).!\n"
        "J(tt_valid)\n"
    )
    return "".join(parts)


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    option = parser.add_mutually_exclusive_group(required=True)
    option.add_argument("--check", action="store_true",
                        help="verify the committed policy against Minimax")
    option.add_argument("--write", action="store_true",
                        help="regenerate the policy within the procedure")
    args = parser.parse_args()

    procedure = Path(__file__).resolve().parents[1] / "library/tictactoe.fredpp"
    source = procedure.read_text(encoding="utf-8")
    if source.count(BEGIN) != 1 or source.count(END) != 1:
        raise SystemExit("expected exactly one BEGIN and one END policy marker")

    policy, outcomes = build_policy()
    before, tail = source.split(BEGIN)
    existing, after = tail.split(END)
    replacement = render_body(policy)

    if args.check:
        if existing != replacement:
            raise SystemExit("Minimax policy is out of date. Run --write.")
    else:
        procedure.write_text(
            before + BEGIN + replacement + END + after,
            encoding="utf-8",
            newline="\n",
        )
    print(
        f"Minimax OK: {len(policy)} positions; "
        f"human wins={outcomes['X']}, draws={outcomes['D']}, "
        f"computer wins={outcomes['O']}."
    )


if __name__ == "__main__":
    main()
