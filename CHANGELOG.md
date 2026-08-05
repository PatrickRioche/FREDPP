# Changelog

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
