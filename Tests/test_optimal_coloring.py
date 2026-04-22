#!/usr/bin/env python3
# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# SPDX-License-Identifier: Apache-2.0

"""Tests for the optimal_coloring backtracking solver used by CMSIS-Stream
buffer sharing.

Run:  python3 Tests/test_optimal_coloring.py
"""

import ast
import os
import sys

# Extract optimal_coloring directly from description.py source via AST,
# bypassing heavy module-level imports (networkx, numpy, sympy, jinja2)
# that are irrelevant to the pure-Python coloring algorithm.
_desc_path = os.path.join(
    os.path.dirname(__file__), '..', 'PythonPackage',
    'cmsis_stream', 'cg', 'scheduler', 'description.py',
)
with open(_desc_path) as _f:
    _tree = ast.parse(_f.read(), filename=_desc_path)
for _node in _tree.body:
    if isinstance(_node, ast.FunctionDef) and _node.name == 'optimal_coloring':
        _code = compile(ast.Module(body=[_node], type_ignores=[]), _desc_path, 'exec')
        _ns: dict = {}
        exec(_code, _ns)  # noqa: P204
        optimal_coloring = _ns['optimal_coloring']
        break
else:
    raise ImportError("optimal_coloring not found in description.py")


# ---------- helpers ----------

def build_interval_graph(intervals):
    """Build adjacency from (name, start, stop) intervals."""
    names = [name for name, _, _ in intervals]
    adj = {n: set() for n in names}
    for i in range(len(intervals)):
        for j in range(i + 1, len(intervals)):
            n1, s1, e1 = intervals[i]
            n2, s2, e2 = intervals[j]
            if s1 <= e2 and s2 <= e1:
                adj[n1].add(n2)
                adj[n2].add(n1)
    return names, adj


def check(condition, msg):
    if not condition:
        print(f"  FAIL: {msg}")
        raise AssertionError(msg)


# ---------- tests ----------

def test_correctness():
    """Verify optimal coloring on small graphs with known chromatic numbers."""
    # K3 (triangle): chromatic = 3
    nodes = [0, 1, 2]
    adj = {0: {1, 2}, 1: {0, 2}, 2: {0, 1}}
    result = optimal_coloring(nodes, adj)
    check(result is not None, "K3: returned None")
    check(max(result.values()) + 1 == 3, f"K3: expected 3, got {max(result.values()) + 1}")

    # C5 (odd 5-cycle): chromatic = 3
    nodes = list(range(5))
    adj = {i: {(i - 1) % 5, (i + 1) % 5} for i in nodes}
    result = optimal_coloring(nodes, adj)
    check(result is not None, "C5: returned None")
    check(max(result.values()) + 1 == 3, f"C5: expected 3, got {max(result.values()) + 1}")
    for n in nodes:
        for nb in adj[n]:
            check(result[n] != result[nb], f"C5: {n} and {nb} share color")


def test_budget_fallback():
    """With a tiny budget the solver gives up and returns None."""
    n = 10
    nodes = list(range(n))
    adj = {i: set(range(n)) - {i} for i in nodes}
    result = optimal_coloring(nodes, adj, max_visits=5)
    check(result is None, f"expected None on budget exceeded, got {result}")


def test_ordering_comparison():
    """Demonstrate that node ordering affects whether the solver succeeds
    within its visit budget.  High-degree-first finds the optimal 4-coloring
    within budget, while low-degree-first exhausts the budget and returns None."""
    intervals = [
        # Chain A
        ("a",  4,  5), ("b",  5,  6), ("c",  6,  7), ("d",  7,  8),
        ("e",  8,  9), ("f",  9, 10), ("g", 10, 25),
        # Chain B
        ("h",  4, 11), ("i", 11, 12), ("j", 12, 13), ("k", 13, 14),
        ("l", 14, 15), ("m", 15, 16), ("n", 16, 17), ("o", 17, 25),
        # Chain C
        ("p",  4, 18), ("q", 18, 19), ("r", 19, 20), ("s", 20, 21),
        ("t", 21, 22), ("u", 22, 23), ("v", 23, 24), ("w", 24, 25),
    ]
    names, adj = build_interval_graph(intervals)
    degrees = {n: len(adj[n]) for n in names}

    high_first = sorted(names, key=lambda n: degrees[n], reverse=True)
    low_first = sorted(names, key=lambda n: degrees[n])

    result_high = optimal_coloring(high_first, adj)
    check(result_high is not None, "high-degree-first returned None")
    high_colors = max(result_high.values()) + 1
    for n in names:
        for nb in adj[n]:
            check(result_high[n] != result_high[nb],
                  f"high-first: {n} and {nb} share color")

    result_low = optimal_coloring(low_first, adj)
    check(result_low is None, "low-degree-first should exhaust budget")

    print(f"  high-degree-first = {high_colors} colors (optimal)")
    print(f"  low-degree-first  = None (budget exhausted)")


# ---------- runner ----------

TESTS = [test_correctness, test_budget_fallback, test_ordering_comparison]


def main():
    passed = 0
    failed = 0
    for t in TESTS:
        name = t.__name__
        try:
            t()
            print(f"PASS  {name}")
            passed += 1
        except AssertionError as e:
            print(f"FAIL  {name}: {e}")
            failed += 1
    print(f"\n{passed} passed, {failed} failed")
    sys.exit(1 if failed else 0)


if __name__ == "__main__":
    main()
