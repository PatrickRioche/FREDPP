# Commandes FREDPP : correspondance des 103 entrées avec les sources C++

> Carte de lecture du code sur la branche `dev` (14 novembre 2026), issue de la référence historique et de la série v0.1.1. Elle n'est pas une nouvelle spécification du langage.

Le catalogue ci-dessous couvre **les 103 entrées** répertoriées dans [l'index français](../fr/reference/commandes/index.md). Pour connaître la disponibilité et la compatibilité, consulter [l'état officiel des commandes](../project/COMMAND_STATUS.md) et [les limites du portage](SOURCE-LIMITS.md).

## 1. Cheminement d'une commande

```text
Commande saisie / procédure
  -> FlowEngine (expansions et substitutions d'entrée)
  -> Lexer / TokenStream
  -> CommandParser (analyse de la syntaxe)
  -> CommandNode / AST
  -> CommandExecutor (effets sur le buffer / l'état / la sortie)
```

**Fichiers à connaître :**

- **REG :** [src/command/CommandRegistry.cpp](../../src/command/CommandRegistry.cpp) : registre de 18 mnémoniques de base ; il ne recense pas individuellement les 103 formes, et certains descripteurs sont des placeholders.
- **PAR :** [src/parser/CommandParser.cpp](../../src/parser/CommandParser.cpp) : syntaxe, paramètres et variantes des commandes.
- **AST :** [include/fred/ast/CommandNode.hpp](../../include/fred/ast/CommandNode.hpp) : types représentant les commandes analysées.
- **EXE :** [src/runtime/CommandExecutor.cpp](../../src/runtime/CommandExecutor.cpp) : répartiteur `CommandExecutor::execute()` et fonctions `execute_*()`.
- **PROC :** [src/runtime/ProcedureRunner.cpp](../../src/runtime/ProcedureRunner.cpp) : commandes de contrôle des procédures et blocs de saisie.
- **CLI :** [src/cli/main.cpp](../../src/cli/main.cpp) : saisie interactive et gestion des commandes à plusieurs lignes.
- **CORE :** [src/core/Buffer.cpp](../../src/core/Buffer.cpp) et [BufferManager.cpp](../../src/core/BufferManager.cpp) : stockage et modification des textes.

Dans chaque ligne, la syntaxe est analysée par **PAR** ; la fonction `execute_*()` citée est définie dans **EXE** sauf indication contraire. La colonne de test est une **piste de recherche** vers un test pertinent, sans garantie que toutes les variantes historiques y soient testées.

## 2. Correspondance commande par commande

### Commandes principales (22)

| Commande / documentation | Nœud ou traitement C++ | Test à consulter |
|---|---|---|
| [`A`](../fr/reference/commandes/a.md) | `AppendCommandNode / CommandExecutor::execute_append() (CLI/PROC)` | [`test_append.cpp`](../../tests/test_append.cpp) |
| [`B`](../fr/reference/commandes/b.md) | `BufferCommandNode / execute_buffer()` | [`test_buffer_command.cpp`](../../tests/test_buffer_command.cpp) |
| [`C`](../fr/reference/commandes/c.md) | `ChangeCommandNode / CommandExecutor::execute_change() (CLI/PROC)` | [`test_change.cpp`](../../tests/test_change.cpp) |
| [`D`](../fr/reference/commandes/d.md) | `DeleteCommandNode / execute_delete()` | [`test_command_executor.cpp`](../../tests/test_command_executor.cpp) |
| [`E`](../fr/reference/commandes/e.md) | `NamedPatternCommandNode / ExecutionContext::define_pattern()` | [`test_lot_91_103.cpp`](../../tests/test_lot_91_103.cpp) |
| [`F`](../fr/reference/commandes/f.md) | `FactsCommandNode ou FileAssociationCommandNode / execute_facts(), execute_file_association()` | [`test_facts.cpp`](../../tests/test_facts.cpp) |
| [`G`](../fr/reference/commandes/g.md) | `GlobalCommandNode / execute_global()` | [`test_global.cpp`](../../tests/test_global.cpp) |
| [`I`](../fr/reference/commandes/i.md) | `InsertCommandNode / CommandExecutor::execute_insert() (CLI/PROC)` | [`test_insert.cpp`](../../tests/test_insert.cpp) |
| [`J`](../fr/reference/commandes/j.md) | `ProcedureRunner::execute_procedure_line() (sauts) ou MessageCommandNode (messages)` | [`test_procedure_jump.cpp`](../../tests/test_procedure_jump.cpp) |
| [`K`](../fr/reference/commandes/k.md) | `CopyCommandNode / execute_copy()` | [`test_copy.cpp`](../../tests/test_copy.cpp) |
| [`L`](../fr/reference/commandes/l.md) | `ListCommandNode / execute_list() (nom de fichier requis)` | [`test_file_io.cpp`](../../tests/test_file_io.cpp) |
| [`M`](../fr/reference/commandes/m.md) | `MoveCommandNode / execute_move()` | [`test_move.cpp`](../../tests/test_move.cpp) |
| [`P`](../fr/reference/commandes/p.md) | `PrintCommandNode / execute_print()` | [`test_command_executor.cpp`](../../tests/test_command_executor.cpp) |
| [`Q`](../fr/reference/commandes/q.md) | `QuitCommandNode / execute_quit()` | [`test_quit.cpp`](../../tests/test_quit.cpp) |
| [`R`](../fr/reference/commandes/r.md) | `ReadCommandNode / execute_read()` | [`test_file_io.cpp`](../../tests/test_file_io.cpp) |
| [`S`](../fr/reference/commandes/s.md) | `SubstituteCommandNode / execute_substitute()` | [`test_substitute.cpp`](../../tests/test_substitute.cpp) |
| [`T`](../fr/reference/commandes/t.md) | `TestCommandNode / execute_test()` | [`test_test_command.cpp`](../../tests/test_test_command.cpp) |
| [`U`](../fr/reference/commandes/u.md) | `RepeatCommandNode / execute_repeat()` | [`test_lot_91_103.cpp`](../../tests/test_lot_91_103.cpp) |
| [`V`](../fr/reference/commandes/v.md) | `UndoCommandNode / ExecutionContext::undo()` | [`test_lot_91_103.cpp`](../../tests/test_lot_91_103.cpp) |
| [`W`](../fr/reference/commandes/w.md) | `WriteCommandNode / execute_write()` | [`test_file_io.cpp`](../../tests/test_file_io.cpp) |
| [`Y`](../fr/reference/commandes/y.md) | `WhyCommandNode / execute_why()` | [`test_lot_71_80.cpp`](../../tests/test_lot_71_80.cpp) |
| [`Z`](../fr/reference/commandes/z.md) | `ZapCommandNode / execute_zap()` | [`test_zap.cpp`](../../tests/test_zap.cpp) |

### Commandes spéciales (7)

| Commande / documentation | Nœud ou traitement C++ | Test à consulter |
|---|---|---|
| [`!`](../fr/reference/commandes/bang.md) | `SystemCommandNode / execute_system()` | [`test_system_zg.cpp`](../../tests/test_system_zg.cpp) |
| [`"`](../fr/reference/commandes/quote.md) | `CommentCommandNode / execute_comment() ; filtrage dans PROC` | [`test_lot_71_80.cpp`](../../tests/test_lot_71_80.cpp) |
| [`#`](../fr/reference/commandes/counter.md) | `CounterCommandNode / execute_counter()` | [`test_lot_71_80.cpp`](../../tests/test_lot_71_80.cpp) |
| [`=`](../fr/reference/commandes/equals.md) | `LineNumberCommandNode / execute_line_number()` | [`test_line_number.cpp`](../../tests/test_line_number.cpp) |
| [`:`](../fr/reference/commandes/colon.md) | `WindowCommandNode / execute_window()` | [`test_lot_71_80.cpp`](../../tests/test_lot_71_80.cpp) |
| [`@`](../fr/reference/commandes/at.md) | `LabelCommandNode ; étiquettes dans ProcedureRunner` | [`test_procedure_jump.cpp`](../../tests/test_procedure_jump.cpp) |
| [`\`](../fr/reference/commandes/backslashbackslash.md) | `WaitCommandNode / CommandExecutor::execute()` | [`test_lot_71_80.cpp`](../../tests/test_lot_71_80.cpp) |

### Famille F — Informations (7)

| Commande / documentation | Nœud ou traitement C++ | Test à consulter |
|---|---|---|
| [`FB`](../fr/reference/commandes/fb.md) | `FactsCommandNode / execute_facts() (FactsKind::Buffers)` | [`test_facts.cpp`](../../tests/test_facts.cpp) |
| [`FE`](../fr/reference/commandes/fe.md) | `FactsCommandNode / execute_facts() (FactsKind::Patterns)` | [`test_lot_91_103.cpp`](../../tests/test_lot_91_103.cpp) |
| [`FF`](../fr/reference/commandes/ff.md) | `FactsCommandNode / execute_facts() (FactsKind::Files)` | [`test_facts.cpp`](../../tests/test_facts.cpp) |
| [`FN`](../fr/reference/commandes/fn.md) | `FactsCommandNode / execute_facts() (FactsKind::Registers)` | [`test_facts.cpp`](../../tests/test_facts.cpp) |
| [`FO`](../fr/reference/commandes/fo.md) | `FactsCommandNode / execute_facts() (FactsKind::Options)` | [`test_options.cpp`](../../tests/test_options.cpp) |
| [`F?`](../fr/reference/commandes/fq.md) | `FactsCommandNode / execute_facts() (FactsKind::Modified)` | [`test_facts.cpp`](../../tests/test_facts.cpp) |
| [`FV`](../fr/reference/commandes/fv.md) | `FactsCommandNode / execute_facts() (FactsKind::Version)` | [`test_facts.cpp`](../../tests/test_facts.cpp) |

### Famille J — Sauts et branchements (5)

| Commande / documentation | Nœud ou traitement C++ | Test à consulter |
|---|---|---|
| [`JB`](../fr/reference/commandes/jb.md) | `RepeatControlCommandNode / execute_repeat()` | [`test_lot_91_103.cpp`](../../tests/test_lot_91_103.cpp) |
| [`JE`](../fr/reference/commandes/je.md) | `MessageCommandNode / execute_message()` | [`test_messages.cpp`](../../tests/test_messages.cpp) |
| [`JM`](../fr/reference/commandes/jm.md) | `MessageCommandNode / execute_message()` | [`test_messages.cpp`](../../tests/test_messages.cpp) |
| [`JO`](../fr/reference/commandes/jo.md) | `ProcedureRunner (sortie) ; RepeatControlCommandNode dans U` | [`test_procedure_jump.cpp`](../../tests/test_procedure_jump.cpp) |
| [`JP`](../fr/reference/commandes/jp.md) | `MessageCommandNode / execute_message()` | [`test_messages.cpp`](../../tests/test_messages.cpp) |

### Famille N — Registres numériques (21)

| Commande / documentation | Nœud ou traitement C++ | Test à consulter |
|---|---|---|
| [`N:`](../fr/reference/commandes/ncolon.md) | `NumericAssignmentCommandNode / execute_numeric_assignment()` | [`test_numeric_operations.cpp`](../../tests/test_numeric_operations.cpp) |
| [`N+`](../fr/reference/commandes/nplus.md) | `NumericOperationCommandNode / execute_numeric_operation()` | [`test_numeric_operations.cpp`](../../tests/test_numeric_operations.cpp) |
| [`N-`](../fr/reference/commandes/nminus.md) | `NumericOperationCommandNode / execute_numeric_operation()` | [`test_numeric_operations.cpp`](../../tests/test_numeric_operations.cpp) |
| [`N*`](../fr/reference/commandes/nmultiply.md) | `NumericOperationCommandNode / execute_numeric_operation()` | [`test_numeric_operations.cpp`](../../tests/test_numeric_operations.cpp) |
| [`N/`](../fr/reference/commandes/ndivide.md) | `NumericOperationCommandNode / execute_numeric_operation()` | [`test_numeric_operations.cpp`](../../tests/test_numeric_operations.cpp) |
| [`N%`](../fr/reference/commandes/nmodulo.md) | `NumericOperationCommandNode / execute_numeric_operation()` | [`test_numeric_operations.cpp`](../../tests/test_numeric_operations.cpp) |
| [`N~`](../fr/reference/commandes/ncomplement.md) | `NumericOperationCommandNode / execute_numeric_operation()` | [`test_numeric_extended.cpp`](../../tests/test_numeric_extended.cpp) |
| [`N&`](../fr/reference/commandes/nand.md) | `NumericOperationCommandNode / execute_numeric_operation()` | [`test_numeric_extended.cpp`](../../tests/test_numeric_extended.cpp) |
| [`N\|`](../fr/reference/commandes/npipe.md) | `NumericOperationCommandNode / execute_numeric_operation() (valeur absolue OU bit à bit selon syntaxe)` | [`test_numeric_extended.cpp`](../../tests/test_numeric_extended.cpp) |
| [`N^`](../fr/reference/commandes/nxor.md) | `NumericOperationCommandNode / execute_numeric_operation()` | [`test_numeric_extended.cpp`](../../tests/test_numeric_extended.cpp) |
| [`N{`](../fr/reference/commandes/nshiftleft.md) | `NumericOperationCommandNode / execute_numeric_operation()` | [`test_numeric_extended.cpp`](../../tests/test_numeric_extended.cpp) |
| [`N}`](../fr/reference/commandes/nshiftright.md) | `NumericOperationCommandNode / execute_numeric_operation()` | [`test_numeric_extended.cpp`](../../tests/test_numeric_extended.cpp) |
| [`N=`](../fr/reference/commandes/nequals.md) | `NumericOperationCommandNode / execute_numeric_operation()` | [`test_numeric_operations.cpp`](../../tests/test_numeric_operations.cpp) |
| [`N<`](../fr/reference/commandes/nless.md) | `NumericOperationCommandNode / execute_numeric_operation()` | [`test_numeric_operations.cpp`](../../tests/test_numeric_operations.cpp) |
| [`N>`](../fr/reference/commandes/ngreater.md) | `NumericOperationCommandNode / execute_numeric_operation()` | [`test_numeric_operations.cpp`](../../tests/test_numeric_operations.cpp) |
| [`NA`](../fr/reference/commandes/na.md) | `NumericOperationCommandNode / execute_numeric_operation()` | [`test_numeric_extended.cpp`](../../tests/test_numeric_extended.cpp) |
| [`NB`](../fr/reference/commandes/nb.md) | `NumericOperationCommandNode / execute_numeric_operation()` | [`test_numeric_extended.cpp`](../../tests/test_numeric_extended.cpp) |
| [`ND`](../fr/reference/commandes/nd.md) | `NumericOperationCommandNode / execute_numeric_operation()` | [`test_numeric_extended.cpp`](../../tests/test_numeric_extended.cpp) |
| [`NF`](../fr/reference/commandes/nf.md) | `NumericOperationCommandNode / execute_numeric_operation()` | [`test_numeric_extended.cpp`](../../tests/test_numeric_extended.cpp) |
| [`NL`](../fr/reference/commandes/nl.md) | `NumericOperationCommandNode / execute_numeric_operation()` | [`test_numeric_extended.cpp`](../../tests/test_numeric_extended.cpp) |
| [`NP`](../fr/reference/commandes/np.md) | `NumericOperationCommandNode / execute_numeric_operation()` | [`test_numeric_extended.cpp`](../../tests/test_numeric_extended.cpp) |

### Famille O — Options (24)

| Commande / documentation | Nœud ou traitement C++ | Test à consulter |
|---|---|---|
| [`OB`](../fr/reference/commandes/ob.md) | `OptionCommandNode / execute_option() (OptionKind::BufferPages)` | [`test_lot_91_103.cpp`](../../tests/test_lot_91_103.cpp) |
| [`OC`](../fr/reference/commandes/oc.md) | `OptionCommandNode / execute_option() (OptionKind::InputChange)` | [`test_lot_81_90.cpp`](../../tests/test_lot_81_90.cpp) |
| [`OI(`](../fr/reference/commandes/oi_parenthesis.md) | `OptionCommandNode / execute_option() (OptionKind::InputParenthesis)` | [`test_options.cpp`](../../tests/test_options.cpp) |
| [`OIL`](../fr/reference/commandes/oil.md) | `OptionCommandNode / execute_option() (OptionKind::InputLowerEscape)` | [`test_lot_81_90.cpp`](../../tests/test_lot_81_90.cpp) |
| [`OIU`](../fr/reference/commandes/oiu.md) | `OptionCommandNode / execute_option() (OptionKind::InputUpperEscape)` | [`test_lot_81_90.cpp`](../../tests/test_lot_81_90.cpp) |
| [`OIE`](../fr/reference/commandes/oie.md) | `OptionCommandNode / execute_option() (OptionKind::InputEscape)` | [`test_lot_81_90.cpp`](../../tests/test_lot_81_90.cpp) |
| [`OIF`](../fr/reference/commandes/oif.md) | `OptionCommandNode / execute_option() (OptionKind::InputFeed)` | [`test_lot_81_90.cpp`](../../tests/test_lot_81_90.cpp) |
| [`OIA`](../fr/reference/commandes/oia.md) | `OptionCommandNode / execute_option() (OptionKind::InputAppendPrompt)` | [`test_lot_81_90.cpp`](../../tests/test_lot_81_90.cpp) |
| [`OIC`](../fr/reference/commandes/oic.md) | `OptionCommandNode / execute_option() (OptionKind::InputCommandPrompt)` | [`test_lot_81_90.cpp`](../../tests/test_lot_81_90.cpp) |
| [`OIR`](../fr/reference/commandes/oir.md) | `OptionCommandNode / execute_option() (OptionKind::InputReturnMessage)` | [`test_lot_81_90.cpp`](../../tests/test_lot_81_90.cpp) |
| [`OI\F`](../fr/reference/commandes/oi_backslash_f.md) | `OptionCommandNode / execute_option() (OptionKind::InputTerminatorRequired)` | [`test_options.cpp`](../../tests/test_options.cpp) |
| [`OOO`](../fr/reference/commandes/ooo.md) | `OptionCommandNode / execute_option() (OptionKind::OutputOctal)` | [`test_options.cpp`](../../tests/test_options.cpp) |
| [`OOL`](../fr/reference/commandes/ool.md) | `OptionCommandNode / execute_option() (OptionKind::OutputLower)` | [`test_lot_81_90.cpp`](../../tests/test_lot_81_90.cpp) |
| [`OOU`](../fr/reference/commandes/oou.md) | `OptionCommandNode / execute_option() (OptionKind::OutputUpper)` | [`test_lot_81_90.cpp`](../../tests/test_lot_81_90.cpp) |
| [`OO`](../fr/reference/commandes/oo.md) | `OptionCommandNode / execute_option() (OptionKind::OutputCharacters)` | [`test_lot_91_103.cpp`](../../tests/test_lot_91_103.cpp) |
| [`OSD`](../fr/reference/commandes/osd.md) | `OptionCommandNode / execute_option() (OptionKind::CaseInsensitiveSearch)` | [`test_lot_81_90.cpp`](../../tests/test_lot_81_90.cpp) |
| [`OS&`](../fr/reference/commandes/os_ampersand.md) | `OptionCommandNode / execute_option() (OptionKind::SubstituteAmpersand)` | [`test_lot_81_90.cpp`](../../tests/test_lot_81_90.cpp) |
| [`OS`](../fr/reference/commandes/os.md) | `OptionCommandNode / execute_option() (OptionKind::SpecialCharacters)` | [`test_lot_91_103.cpp`](../../tests/test_lot_91_103.cpp) |
| [`OS/`](../fr/reference/commandes/os_separator.md) | `OptionCommandNode / execute_option() (OptionKind::SubstituteSeparator)` | [`test_lot_81_90.cpp`](../../tests/test_lot_81_90.cpp) |
| [`OP`](../fr/reference/commandes/op.md) | `OptionCommandNode / execute_option() (OptionKind::PageSize)` | [`test_options.cpp`](../../tests/test_options.cpp) |
| [`OQ`](../fr/reference/commandes/oq.md) | `OptionCommandNode / execute_option() (OptionKind::QuitOnError)` | [`test_options.cpp`](../../tests/test_options.cpp) |
| [`OM`](../fr/reference/commandes/om.md) | `OptionCommandNode / execute_option() (OptionKind::Monitor)` | [`test_options.cpp`](../../tests/test_options.cpp) |
| [`OR`](../fr/reference/commandes/or.md) | `OptionCommandNode / execute_option() (OptionKind::PublicRead)` | [`test_lot_91_103.cpp`](../../tests/test_lot_91_103.cpp) |
| [`OT`](../fr/reference/commandes/ot.md) | `OptionCommandNode / execute_option() (OptionKind::TabStops)` | [`test_lot_81_90.cpp`](../../tests/test_lot_81_90.cpp) |

### Famille Z — Commandes diverses (17)

| Commande / documentation | Nœud ou traitement C++ | Test à consulter |
|---|---|---|
| [`ZA`](../fr/reference/commandes/za.md) | `ZapExtendedCommandNode / execute_zap_extended() (ZapExtendedKind::AppendBuffer)` | [`test_zap_extended.cpp`](../../tests/test_zap_extended.cpp) |
| [`ZB`](../fr/reference/commandes/zb.md) | `ZapExtendedCommandNode / execute_zap_extended() (ZapExtendedKind::SortDescending)` | [`test_zap_extended.cpp`](../../tests/test_zap_extended.cpp) |
| [`ZC`](../fr/reference/commandes/zc.md) | `ZapExtendedCommandNode / execute_zap_extended() (ZapExtendedKind::ClearModified)` | [`test_zap_extended.cpp`](../../tests/test_zap_extended.cpp) |
| [`ZD`](../fr/reference/commandes/zd.md) | `ZapExtendedCommandNode / execute_zap_extended() (ZapExtendedKind::DeleteBuffer)` | [`test_zap_extended.cpp`](../../tests/test_zap_extended.cpp) |
| [`ZG`](../fr/reference/commandes/zg.md) | `ZapGatherCommandNode / execute_zap_gather()` | [`test_system_zg.cpp`](../../tests/test_system_zg.cpp) |
| [`ZI`](../fr/reference/commandes/zi.md) | `ZapExtendedCommandNode / execute_zap_extended() (ZapExtendedKind::CompressTabs)` | [`test_zap_extended.cpp`](../../tests/test_zap_extended.cpp) |
| [`ZK`](../fr/reference/commandes/zk.md) | `ZapCopyCommandNode / execute_zap_copy()` | [`test_zap_copy.cpp`](../../tests/test_zap_copy.cpp) |
| [`ZL`](../fr/reference/commandes/zl.md) | `ZapExtendedCommandNode / execute_zap_extended() (ZapExtendedKind::Lowercase)` | [`test_zap_extended.cpp`](../../tests/test_zap_extended.cpp) |
| [`ZM`](../fr/reference/commandes/zm.md) | `ZapExtendedCommandNode / execute_zap_extended() (ZapExtendedKind::MoveLines)` | [`test_zap_extended.cpp`](../../tests/test_zap_extended.cpp) |
| [`ZO`](../fr/reference/commandes/zo.md) | `ZapExtendedCommandNode / execute_zap_extended() (ZapExtendedKind::ExpandTabs)` | [`test_zap_extended.cpp`](../../tests/test_zap_extended.cpp) |
| [`ZP`](../fr/reference/commandes/zp.md) | `ZapExtendedCommandNode / execute_zap_extended() (ZapExtendedKind::PrintBuffer)` | [`test_zap_extended.cpp`](../../tests/test_zap_extended.cpp) |
| [`ZR`](../fr/reference/commandes/zr.md) | `ReadCommandNode / execute_read() (forme ZR)` | [`test_file_io.cpp`](../../tests/test_file_io.cpp) |
| [`ZS`](../fr/reference/commandes/zs.md) | `ZapExtendedCommandNode / execute_zap_extended() (ZapExtendedKind::SortAscending)` | [`test_zap_extended.cpp`](../../tests/test_zap_extended.cpp) |
| [`ZT`](../fr/reference/commandes/zt.md) | `ZapExtendedCommandNode / execute_zap_extended() (ZapExtendedKind::Translate)` | [`test_zap_extended.cpp`](../../tests/test_zap_extended.cpp) |
| [`ZU`](../fr/reference/commandes/zu.md) | `ZapExtendedCommandNode / execute_zap_extended() (ZapExtendedKind::Uppercase)` | [`test_zap_extended.cpp`](../../tests/test_zap_extended.cpp) |
| [`ZV`](../fr/reference/commandes/zv.md) | `RedoCommandNode / ExecutionContext::redo()` | [`test_lot_91_103.cpp`](../../tests/test_lot_91_103.cpp) |
| [`ZW`](../fr/reference/commandes/zw.md) | `WriteCommandNode / execute_write() (écriture ajoutée)` | [`test_file_io.cpp`](../../tests/test_file_io.cpp) |

## 3. Différences et limites à connaître

- **A / I / C :** la saisie d'un bloc de texte est orchestrée par **CLI** ou **PROC** ; `CommandExecutor::execute()` seul ne suffit pas. Les méthodes `execute_append()`, `execute_insert()` et `execute_change()` reçoivent les lignes collectées.
- **J :** `J(label)`, `JT` et `JF` passent par **PROC** ; `JM`, `JP`, `JE` produisent des messages ; `JB`/`JO` contrôlent les boucles `U` et, pour `JO`, les procédures.
- **N :** la syntaxe numérique en procédure peut être prise en charge directement dans **PROC** ; les commandes numériques ordinaires passent par les nœuds et fonctions indiqués dans le tableau.
- **O :** `OO` (caractères spéciaux à l'affichage) ne doit pas être confondu avec `OOO` (affichage octal). Les 24 options mènent à `execute_option()`.
- **Z :** `Z` simple, `ZK`, `ZG`, `ZR`, `ZV` et `ZW` ont des voies distinctes de la plupart des autres sous-commandes `Z`.
- **G :** les commandes imbriquées exécutables sont actuellement limitées à `P`, `D`, `Z` et `S`. Les commandes imbriquées adressées dans `ZG` ne sont pas prises en charge.
- **L / W / Q :** `L` sans nom de fichier non implémenté ; format historique BCD de `WB` non pris en charge ; `WU` est une extension UTF-8 de FREDPP ; `Q!` non implémenté.
- **! :** exécution via le shell de l'hôte, non par le TSS historique ; ne jamais y transmettre une entrée non fiable.

**Important :** le chiffre `103/103` dans le suivi de développement indique la **couverture des entrées de la référence**, pas la conformité exhaustive de toutes les variantes historiques GCOS.

## 4. Recherches rapides depuis la racine du dépôt

```powershell
rg -n 'AppendCommandNode|execute_append' src include tests
rg -n 'ZapCopyCommandNode|execute_zap_copy|ZK' src include tests
rg -n 'OptionKind::|execute_option' src include
rg -n 'case AstNodeKind::' src/runtime/CommandExecutor.cpp
```

`rg` désigne ripgrep (si installé), sinon utiliser la recherche globale de Visual Studio ou VS Code.

## 5. Maintenance

Pour modifier une commande : consulter sa fiche française, vérifier **PAR**, suivre son nœud **AST**, modifier **EXE** ou **PROC** et les tests associés ; lancer ensuite la suite CTest complète et les recettes interactives utiles. Ne pas confondre la présence d'un nœud AST avec une implémentation historique parfaite.

Voir aussi [SOURCE-ARCHITECTURE.md](SOURCE-ARCHITECTURE.md), [SOURCE-FLOW.md](SOURCE-FLOW.md) et [RUNTIME.md](RUNTIME.md).
