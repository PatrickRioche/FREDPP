# ZW — Ecriture à la suite ( Zap Write)

> La commande ZW écrit les lignes spécifiées dans le fichier désigné, à la suite de ce qu'il y avait déjà dans ce fichier.

## Syntaxe

```fred
(.,.)ZW<options de W>
```

## Paramètres

| Élément | Description |
|---|---|
| `(.,.)` | Adresse ou plage de lignes optionnelle, selon la forme indiquée. |
| `<options de W>` | Élément défini par la syntaxe historique de la commande. |

## Description

La commande ZW écrit  les lignes spécifiées dans le fichier désigné, à la suite de ce qu'il y avait déjà dans ce fichier. Pour le reste, accepte les mêmes options que la commande W, et agit de la même façon.

## Exemples

```fred
b(buf)
w /fred/parex
4,123 b(buf) /fred/parex
1,2zw /fred/parex
2,61
b(bufw)r /fred/parex
6,185 b(bufw) /fred/parex
*
Ajout  ligne de texte numero 1
Ajout  ligne de texte numero 2
Ajout  ligne de texte numero 3
Ajout  ligne de texte numero 4
Ajout  ligne de texte numero 1
Ajout  ligne de texte numero 2
```

## Options

Aucune option spécifique n'est documentée.

## Remarques

Aucune remarque spécifique n'est documentée.

## Compatibilité

| Implémentation | Statut |
|---|---|
| FRED historique | Compatible |
| FRED++ | À valider selon l'implémentation |

## Voir aussi

Aucun lien associé n'est encore défini.
