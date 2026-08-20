# Core buffers and limits

## 1. Scope

The current `fred::core` layer provides:

```text
Buffer
BufferManager
CharacterInterpretation
limits::max_buffer_name_length
```

`CharacterInterpretation` is documented with the Lexer/Flow boundary. This
document focuses on Buffer state and ownership.

## 2. `Buffer`

`Buffer` is the in-memory editor object.

Owned state:

```text
name_
lines_
current_line_
modified_
associated_file_
encoding_
line_ending_
final_newline_
```

It does not parse FRED commands and does not know about AST nodes.

### 2.1 Line-number model

Stored lines are one-based at the public API boundary:

```text
line(1) -> first line
line(N) -> Nth line
```

`0` has a separate cursor meaning:

```text
current_line() == 0
```

means no current stored line.

`set_current_line(0)` is accepted.

### 2.2 New buffer defaults

A directly constructed Buffer starts with:

```text
lines            empty
current_line     0
modified         false
associated file  none
encoding         Unknown
line ending      Lf
final newline    true
```

The Buffer constructor requires a non-empty name.

It does **not** enforce the 64-character manager limit itself.

## 3. Buffer mutations

### `append(text)`

```text
append line
current = new last line
modified = true
```

### `insert_before(number, text)`

Accepted:

```text
1..line_count()
line_count()+1 -> append-at-end
```

The inserted line becomes current.

### `insert_after(number, lines)`

Accepted position:

```text
0..line_count()
```

This matters because:

```text
insert_after(0, ...)
```

inserts before the first existing line.

For non-empty input, the last inserted line becomes current and the Buffer is
marked modified.

For an empty vector, no content is changed and the Buffer is **not** dirtied;
only `current_line` becomes `number`.

### `replace(number, text)`

The target line becomes current.

The Buffer is marked modified only if the replacement text differs from the
existing text.

### `erase(first, last)`

The range is inclusive.

Both endpoints must already exist and:

```text
first <= last
```

After deletion:

- empty Buffer -> current line 0;
- if a line now occupies the original `first` position -> that line is current;
- otherwise -> new final line is current.

Deletion marks the Buffer modified.

## 4. Existing-line validation

`line()`, `replace()` and erase endpoints use the strict existing-line rule:

```text
1 <= number <= line_count()
```

Violations raise:

```cpp
std::out_of_range("line number out of range")
```

By contrast, insertion/cursor APIs have explicitly wider position rules as
described above.

## 5. File metadata

### `load_file(...)`

This is a full content baseline replacement.

It:

- replaces all lines;
- sets current line to the final loaded line, or 0 if empty;
- associates the filename;
- stores encoding/line-ending/final-newline metadata;
- marks the Buffer clean.

### `associate_file(...)`

This changes only association metadata.

It does **not**:

- replace lines;
- change current line;
- change modified state.

### Explicit dirty-state methods

```cpp
mark_clean()
mark_modified()
```

only change the flag.

## 6. Borrowed references

These APIs expose Buffer-owned objects by const reference:

```cpp
name()
line()
lines()
associated_file()
```

Callers must not retain line/vector references across mutations that can
reallocate or erase line storage.

## 7. `BufferManager`

Internal model:

```text
unordered_map<string, unique_ptr<Buffer>> buffers_
vector<string> usage_order_
Buffer* current_
```

The manager owns Buffers. `current_` is a non-owning pointer into owned
storage.

## 8. Bootstrap Buffer `0`

`BufferManager()` calls:

```cpp
create_or_select("0")
```

so a new manager has:

```text
buffer "0" exists
current buffer == "0"
recent_names()[0] == "0"
```

This is a Core invariant used by higher layers.

## 9. Creation APIs

### `create_or_select(name)`

This operation:

1. validates the name;
2. creates it if missing;
3. selects it;
4. touches the MRU order;
5. may remove the previously selected transient empty Buffer.

### `get_or_create(name)`

This operation:

1. validates the name;
2. creates it if missing;
3. returns it.

It intentionally does **not**:

- select it;
- change `current_`;
- touch MRU order.

This distinction is important for bootstrap/internal code.

## 10. Transient empty-buffer cleanup — FR-0006

When `create_or_select()` switches away from the current Buffer, the old Buffer
is automatically deleted only when all conditions are true:

```text
old name != new name
old name != "0"
old buffer is empty
old buffer is clean
old buffer has no associated file
```

Therefore an empty buffer survives if it is:

- Buffer `0`;
- modified;
- associated with a file.

Tests exercise the transient `B(temp)` behavior.

## 11. Name limits

`limits::max_buffer_name_length` is:

```cpp
64
```

The code records the compatibility distinction:

```text
historical FRED: 15 characters
FREDPP extension: 64 characters
```

This must be described as a **FREDPP extension**, not as a historical FRED
property.

Creation through `BufferManager` rejects names over 64.

The Flow layer also checks this limit while resolving directive buffer names.

## 12. Selection and MRU

`select(name)` requires the Buffer to exist and moves its name to the front of:

```cpp
recent_names()
```

The MRU list is unique: `touch()` removes the prior occurrence before inserting
at the front.

`names()` has different semantics:

```text
all live names, lexicographically sorted
```

`modified_names()` is also sorted lexicographically.

## 13. Erase behavior

`erase(name)`:

- rejects unknown names;
- removes the name from MRU;
- invalidates all references/pointers to that Buffer.

If no buffers remain, `"0"` is recreated and selected.

If the erased Buffer was current but others remain:

```cpp
current_ = buffers_.begin()->second.get();
```

Because storage is `unordered_map`, **which remaining Buffer becomes current is
not a stable semantic ordering**. Code must not rely on a specific fallback
name.

## 14. Ownership hazards

Most Buffer references remain stable even if the unordered map rehashes because
the actual Buffer objects are separately allocated by `unique_ptr`.

However, a reference becomes invalid when its Buffer is erased.

This can happen explicitly through `erase()` or implicitly through transient
cleanup in `create_or_select()`.

## 15. Architectural boundary

Core Buffer classes must not:

- parse command text;
- parse/evaluate AST addresses;
- interpret flow directives;
- perform command dispatch;
- directly print to a terminal.

They provide state and primitive mutation operations for Runtime/Flow layers.
