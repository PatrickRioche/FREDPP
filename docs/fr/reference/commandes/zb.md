# ZB — Tri en ordre décroissant

> La commande ZB trie les lignes selectionnées dans le buffer, en ordre décroissant.

## Syntaxe

```fred
(.,.)ZB
```

## Paramètres

| Élément | Description |
|---|---|
| `(.,.)` | Adresse ou plage de lignes optionnelle, selon la forme indiquée. |

## Description

La commande ZB trie les lignes selectionnées dans le buffer, en ordre décroissant.

Le registre de condition est positionné à "VRAI" (True) si les lignes triées sont toutes identiques, sinon il est positionné à "FAUX" (False).

## Exemples

```fred
b(buf)
*
Ajout  ligne de texte numero 1
Ajout  ligne de texte numero 2
Ajout  ligne de texte numero 3
Ajout  ligne de texte numero 4
*
zb
*
Ajout  ligne de texte numero 4
Ajout  ligne de texte numero 3
Ajout  ligne de texte numero 2
Ajout  ligne de texte numero 1
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
