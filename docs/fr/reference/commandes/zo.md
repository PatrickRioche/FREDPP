# ZO — Passage en mode non tabulation

> La commande ZO remplace les tabulations par des espaces si possible.

## Syntaxe

```fred
(.,.)ZO
```

## Paramètres

| Élément | Description |
|---|---|
| `(.,.)` | Adresse ou plage de lignes optionnelle, selon la forme indiquée. |

## Description

La commande ZO  remplace les tabulations par des espaces si possible.

## Exemples

```fred
b(buf)
*s! !\ !
o+t10,20,30,40,50,60,70
*
Ajout              ligne     de        texte     numero    1
Ajout              ligne     de        texte     numero    2
Ajout              ligne     de        texte     numero    3
Ajout              ligne     de        texte     numero    4
*
zo
*s! !.!*
Ajout....ligne.....de........texte.....numero....1
Ajout....ligne.....de........texte.....numero....2
Ajout....ligne.....de........texte.....numero....3
Ajout....ligne.....de........texte.....numero....4
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
