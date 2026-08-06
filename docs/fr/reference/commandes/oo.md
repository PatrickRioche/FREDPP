# OO — Modification de caractère en sortie ( Option Output )

> L'option OO permet de modifier le format de certains caractères en impression sur des terminaux ne reconnaissant pas ces caractères.

## Syntaxe

```fred
O[+|-]O<caractères>
```

## Paramètres

| Élément | Description |
|---|---|
| `<caractères>` | Ensemble de caractères. |

## Description

L'option OO permet de modifier le format de certains caractères en impression sur des terminaux ne reconnaissant pas ces caractères.

Voir la liste caractères d'échappements dans EXPL FRED ESCAPES.

## Exemples

```fred
fo
...
o+oo\B\C\F\L\N\R\E\O\W\S
...
b(bufo)
a
>\C\B(bufe)
>
\f
o-o\B
*
\030(bufe)
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
