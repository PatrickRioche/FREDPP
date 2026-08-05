# ZS — Tri en ordre croissant ( Zap Sort )

> La commande ZS trie les lignes sélectionnées dans le buffer par ordre croissant.

## Syntaxe

```fred
(.,.)ZS
```

## Paramètres

| Élément | Description |
|---|---|
| `(.,.)` | Adresse ou plage de lignes optionnelle, selon la forme indiquée. |

## Description

La commande ZS trie les lignes sélectionnées dans le buffer par ordre croissant.

Le registre de condition est chargé à "VRAI" (True) si les lignes triées sont toutes identiques, et mis à "FAUX" dans tous les autres cas .

## Exemples

```fred
b(buf)
*zb*Ajout  ligne de texte numero 4Ajout  ligne de texte numero 3Ajout  ligne de texte numero 2Ajout  ligne de texte numero 1*zs*Ajout  ligne de texte numero 1Ajout  ligne de texte numero 2Ajout  ligne de texte numero 3Ajout  ligne de texte numero 4
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
