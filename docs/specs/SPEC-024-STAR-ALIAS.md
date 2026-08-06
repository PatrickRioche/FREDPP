# SPEC-024 — Alias historique `*`

## Objectif

Implémenter l'astérisque comme raccourci historique de la plage complète `1,$` au début d'une commande FRED.

## Règles fonctionnelles

1. `*` utilisé seul est équivalent à `1,$P`.
2. `*<commande>` est équivalent à `1,$<commande>`.
3. La commande suivant l'astérisque reste insensible à la casse.
4. L'alias ne doit être reconnu qu'en position initiale d'une commande.
5. L'astérisque présent dans un modèle ou dans du texte conserve sa signification propre.
6. Les restrictions d'adressage de la commande cible restent applicables.

## Architecture

Le lexer produit déjà `*` comme symbole imprimable. Le `CommandParser` transforme ce symbole initial en un `RangeAddressNode` composé de :

- `AbsoluteAddressNode(1)` ;
- `LastAddressNode($)`.

Lorsque la ligne se termine immédiatement après `*`, le parser construit implicitement une commande `P`. Le runtime ne reçoit donc aucune commande spéciale supplémentaire : il exécute les nœuds existants avec une plage normale.

## Exemples

```fred
*             " équivaut à 1,$P
*D            " équivaut à 1,$D
*G/erreur/P   " équivaut à 1,$G/erreur/P
```

## Tests requis

- parsing de `*` comme `PrintCommand` adressée par `1,$` ;
- parsing de `*D` ;
- parsing de `*G/<modèle>/P` ;
- affichage complet du buffer avec `*` et `*P` ;
- suppression complète du buffer avec `*D` ;
- aide embarquée `?*` ;
- non-régression de la suite existante.

## Documentation

Aucun fichier sous `docs/fr/reference/commandes/` n'est modifié. L'aide spécifique de l'alias est stockée dans `docs/fr/fredpp/alias-etoile.md`.
