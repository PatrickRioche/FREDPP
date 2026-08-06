# ZL — Conversion en minuscules ( Zap Lower )

> La commande ZL convertit toutes les lettres en minuscules; le nombre de lignes modifiées est mis dans le registre compteur ( # ).

## Syntaxe

```fred
(.,.)ZL[/<modèle>/]
```

## Paramètres

| Élément | Description |
|---|---|
| `(.,.)` | Adresse ou plage de lignes optionnelle, selon la forme indiquée. |
| `<modèle>` | Modèle de recherche FRED. |

## Description

La commande ZL convertit toutes les lettres en minuscules; le nombre de lignes modifiées est mis dans le registre compteur ( # ).

La commande ZL/<modèle>/ met toutes les chaînes de caractères conformes au modèle en minuscules; le nombre de chaînes converties est mis dans le registre compteur ( # ).

L'adresse de la ligne courante "." pointe sur la dernière ligne où des caractères ont été modifiés par une conversion.

## Exemples

```fred
b(buf)
1zl
2zl/^./
1,2p
ajout  ligne de texte numero 1
ajout  ligne de texte numero 2
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
