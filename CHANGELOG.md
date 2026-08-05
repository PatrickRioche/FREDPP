# Changelog

## v0.0.5

### Added

- Métadonnées de compilation contenant le commit Git et l'état des sources.
- Aide `?:` consacrée aux commandes spéciales de FREDPP.
- Documentation française `docs/fr/fredpp/commandes-speciales.md`.
- Spécification `SPEC-023` sur l'identité de compilation.

### Changed

- La version est désormais définie uniquement par `project(FREDPP VERSION ...)` dans CMake.
- `?version` et `--version` affichent la version, le commit, l'état des sources et la section de roadmap associée.
- `:help` affiche la même rubrique que `?:`.
- `ROADMAP.md` est entièrement actualisé en français avec le contenu de chaque version.
- Les tests de version et d'aide couvrent les nouvelles informations.
- Les index `?` et `?:` utilisent désormais une présentation alignée et lisible dans le terminal.

## v0.0.4

### Added

- Commande historique `G` avec sélection normale ou inversée (`G~`).
- Commande historique `Z` pour changer la ligne courante sans affichage.
- Moteur d'évaluation des modèles FRED.
- Exécution initiale de `P`, `D` et `Z` à l'intérieur de `G`.
- Trois nouveaux tests : global, zap et correspondance de modèles.

### Changed

- L'aide embarquée lit désormais uniquement les pages sélectionnées sous
  `docs/fr/reference/commandes/`, sans les modifier.
- `?index` est généré à partir de la liste des commandes intégrées.
- Le README indique `?version` au lieu d'un numéro de version écrit en dur.
- Le `switch` de diagnostic des adresses est désormais exhaustif pour Clang.

## v0.0.3

### Added

- Aide Markdown française embarquée et disponible hors connexion.
- `HelpManager` et test unitaire associé.
- Alias `?help` et `?h` pour l'index de l'aide.

## v0.0.2

- Commandes historiques initiales et gestion des buffers nommés.

- amélioration de la lisibilité terminal des aides de commandes sans modification des Markdown de référence ;
