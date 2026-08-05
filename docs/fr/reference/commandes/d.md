# D — Suppression de n lignes ( Delete )

> La commande D permet de supprimer une ligne ou un groupe de lignes d'un buffer.

## Syntaxe

```fred
(.,.)D
```

## Paramètres

| Élément | Description |
|---|---|
| `(.,.)` | Adresse ou plage de lignes optionnelle, selon la forme indiquée. |

## Description

La commande D permet de supprimer une ligne ou un groupe de lignes d'un buffer.

## Exemples

```fred
b(buf)
4d*Ajout  ligne de texte numero 1Ajout  ligne de texte numero 2Ajout  ligne de texte numero 3*d*? empty buffer
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

- [A](a.md)
- [C](c.md)
- [V](v.md)
