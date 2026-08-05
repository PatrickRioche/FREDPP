# ZI — Passage en mode tabulation

> La commande ZI remplace les espaces par des tabulations si possible.

## Syntaxe

```fred
(.,.)ZI
```

## Paramètres

| Élément | Description |
|---|---|
| `(.,.)` | Adresse ou plage de lignes optionnelle, selon la forme indiquée. |

## Description

La commande ZI remplace les espaces par des tabulations si possible.

## Exemples

```fred
b(buf)
o+t7,13,16,22,29*zi*Ajout  ligne de texte numero 1Ajout  ligne de texte numero 2Ajout  ligne de texte numero 3Ajout  ligne de texte numero 4o+t10,20,30,40,50,60,70*Ajout              ligne     de        texte     numero    1Ajout              ligne     de        texte     numero    2Ajout              ligne     de        texte     numero    3Ajout              ligne     de        texte     numero    4
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
