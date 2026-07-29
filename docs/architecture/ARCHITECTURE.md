# Architecture

## Processing pipeline

FlowEngine → CharacterStream → Lexer → TokenStream → AddressParser → PatternParser → CommandParser → AST → AddressEvaluator → Runtime

## Core boundaries

- Parser code never accesses Buffer state.
- Runtime code never parses source text.
- AddressEvaluator is the bridge between parsed addresses and Buffer state.
- Each module must have an independently executable test target.
