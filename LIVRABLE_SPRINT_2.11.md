# Livrable Sprint 2.11 — Alias historique `*`

## Version cible

La version exacte se consulte dans l'exécutable avec :

```text
?version
```

## Fonctionnalités intégrées

- `*` reconnu comme alias de la plage complète `1,$` ;
- `*` utilisé seul équivaut à `1,$P` ;
- prise en charge de `*P`, `*D` et des autres commandes adressables déjà disponibles ;
- compatibilité de l'alias avec `G`, par exemple `*G/<modèle>/P` ;
- commandes insensibles à la casse après l'alias ;
- aide embarquée `?*` ;
- nouvelle spécification `SPEC-024` ;
- nouveau test d'intégration `test_star_alias`.

## Architecture

L'alias n'ajoute pas de nouvelle commande runtime. Le parser le transforme en une plage AST normale :

```text
*
  ↓
RangeAddressNode(1, $)
```

Lorsque `*` est seul, une commande `P` implicite est construite.

## Documentation

Aucun fichier situé sous `docs/fr/reference/commandes/` n'est modifié.

La documentation de l'alias est séparée dans :

```text
docs/fr/fredpp/alias-etoile.md
```

Elle est accessible dans FREDPP avec :

```text
?*
```

## Homologation manuelle

```fred
A
alpha
beta
gamma
\F
*
*D
```

Résultat attendu :

1. `*` affiche les trois lignes ;
2. `*D` vide le buffer ;
3. `*` sur le buffer vide signale que le buffer courant est vide.

## Validation attendue

- Windows / MSVC : 23/23 tests ;
- Debian / GCC : 23/23 tests ;
- Debian / Clang : 23/23 tests.
