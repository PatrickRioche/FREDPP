# ZT — Traduction ( Traduct )

> La commande ZT convertit respectivement les caractères de <chaîne1> en caractères de <chaîne2>.

## Syntaxe

```fred
(.,.)ZT/<chaîne1>/<chaîne2>/
```

## Paramètres

| Élément | Description |
|---|---|
| `(.,.)` | Adresse ou plage de lignes optionnelle, selon la forme indiquée. |
| `<chaîne1>` | Première chaîne de caractères. |
| `<chaîne2>` | Deuxième chaîne de caractères. |

## Description

La commande ZT convertit respectivement les caractères de <chaîne1> en caractères de <chaîne2>.

Il faut que <chaîne2> soit de la même longueur que <chaîne1>, il faut également qu'aucun caractère ne puisse apparaître plus d'une fois dans <chaîne1>.

Le nombre de caractères convertis est mis dans le registre compteur. L'adresse de la ligne courante "." pointe sur la  dernière ligne où des conversions ont eu lieu.

## Exemples

```fred
b(buf)
*zt/ABCDEFGH/BCDEFGHI/
*
Bjout  ligne de texte numero 1
Bjout  ligne de texte numero 2
Bjout  ligne de texte numero 3
Bjout  ligne de texte numero 4
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
