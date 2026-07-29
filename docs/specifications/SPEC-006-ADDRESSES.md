# SPEC-006 — Address AST and parser

Status: implemented in FRED++ 0.5.0.

## 1. AST hierarchy

```text
AstNode
└── AddressNode
    ├── AbsoluteAddressNode
    ├── CurrentAddressNode
    ├── LastAddressNode
    ├── RelativeAddressNode
    └── RangeAddressNode
```

Every node exposes an `AstNodeKind` and its starting `SourceLocation`.

## 2. Node semantics represented by the AST

- `AbsoluteAddressNode(n)` represents the numeric spelling `n`.
- `CurrentAddressNode` represents `.`.
- `LastAddressNode` represents `$`.
- `RelativeAddressNode(Forward, n)` represents `+n`.
- `RelativeAddressNode(Backward, n)` represents `-n`.
- `RangeAddressNode(a, b)` represents `a,b`.

The parser records syntax only. It does not check whether a line exists and does
not evaluate addresses against a buffer. In particular, numeric zero is retained
in the AST; any semantic restriction belongs to a later semantic/runtime layer.

## 3. Ownership

Nodes are owned through `std::unique_ptr`. A range owns both endpoint nodes.
The AST is therefore explicit, non-shared, and lifetime-safe.

## 4. Numeric conversion

Address numbers are converted to `std::size_t`. Overflow is diagnosed as a
`ParseError`; conversion never wraps.

## 5. Accepted examples

```text
1
.
$
+3
-2
1,$
.,+4
```

## 6. Rejected examples in this milestone

```text

,1
1,
+
+.
1 2
1,$,2
```
