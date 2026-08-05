# Feuille de route de FREDPP

Cette feuille de route présente les fonctionnalités intégrées dans chaque version et les prochains jalons du projet.

L'identité exacte d'un exécutable se consulte directement dans FREDPP avec :

```text
?version
```

Cette commande affiche le numéro de version, le commit Git utilisé lors de la configuration et l'état des sources.

## Versions réalisées

### v0.0.1 — Fondation du dépôt

- création du dépôt structuré ;
- mise en place de CMake et du socle C++20 ;
- premières règles de gouvernance et de contribution.

### v0.0.2 — Premières commandes historiques

- intégration des buffers et du moteur Flow ;
- lexer, flux de jetons, analyse des adresses et des modèles ;
- premières commandes exécutables de FRED ;
- mise en place de la suite de tests CTest.

### v0.0.3 — Aide embarquée et portabilité

- ajout de l'aide française Markdown embarquée ;
- consultation avec `?`, `?index`, `?h` et `?help` ;
- validation de 19 tests sous Windows/MSVC, Debian/GCC et Debian/Clang ;
- ajout du script Linux `scripts/rebuild.sh`.

### v0.0.4 — Sprint 2.9 : commandes G et Z

Modifications intégrées :

- commande historique `G` avec sélection normale et inversée `G~` ;
- commande historique `Z` pour positionner silencieusement la ligne courante ;
- moteur de correspondance des modèles FRED ;
- exécution initiale de `P`, `D` et `Z` à l'intérieur de `G` ;
- intégration sélective des aides françaises de `G` et `Z` ;
- passage de 19 à 22 tests ;
- validation de 22/22 tests sous MSVC, GCC et Clang.

### v0.0.5 — Sprint 2.10 : traçabilité et aide FREDPP

Modifications intégrées :

- numéro de version centralisé dans `project(FREDPP VERSION ...)` de CMake ;
- liaison automatique entre l'exécutable et le commit Git au moment de la configuration ;
- indication `propre`, `modifié` ou `inconnu` pour l'état des sources ;
- enrichissement de `?version` et de `--version` avec ces informations ;
- référence automatique à la section correspondante de cette feuille de route ;
- ajout de `?:` pour lister les commandes spéciales de FREDPP ;
- alignement de `:help` sur la même aide ;
- documentation française séparée des commandes spéciales, sans modification de `docs/fr/reference/commandes/` ;
- suppression de tout numéro de version écrit en dur dans le README ;
- tests étendus pour l'identité de compilation et l'aide `?:` ;
- présentation alignée des aides `?` et `?:` pour améliorer leur lisibilité dans le terminal.

## État fonctionnel actuel

Commandes FRED disponibles :

```text
P, L, D, A, B, I, C, M, T, G, Z
```

Les limites détaillées sont suivies dans `docs/project/COMMAND_STATUS.md`.

## Prochains jalons

### Prochaines versions 0.0.x

- réaligner les commandes `M` et `T` sur leur comportement historique documenté ;
- compléter les formes courtes de la commande `B` ;
- intégrer les alias historiques comme `*` et `*D` ;
- poursuivre l'implémentation des commandes historiques documentées ;
- étendre `G` aux suites de commandes et aux comportements historiques complémentaires ;
- embarquer uniquement la documentation française des commandes effectivement implémentées.

### v0.1.0 — Premier éditeur historique utilisable

- ensemble cohérent de commandes FRED validées ;
- exécution d'exemples historiques sans adaptation majeure ;
- documentation utilisateur française intégrée ;
- release testée sous MSVC, GCC et Clang.

### v1.0.0 — Réimplémentation moderne de référence

- couverture des commandes principales du langage FRED ;
- procédures, options et environnement documentés ;
- compatibilité multiplateforme automatisée ;
- documentation utilisateur et développeur complète ;
- corpus de tests historiques et de non-régression.

- rendu terminal lisible des documentations Markdown embarquées, sans modifier les sources de référence ;
