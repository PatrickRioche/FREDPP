# OT — Positionne des tabulations ( Option Tab )

> L'option permet de positionner des tabulations.

## Syntaxe

```fred
O[+|-]T<liste tabulation>
```

## Paramètres

| Élément | Description |
|---|---|
| `<liste tabulation>` | Élément défini par la syntaxe historique de la commande. |

## Description

L'option permet de positionner des tabulations. L'option O-T supprime l'affichage des tabulations.

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
```

## Options

Valeur par défaut les tabulations sont "O+T5,9".

## Remarques

Aucune remarque spécifique n'est documentée.

## Compatibilité

| Implémentation | Statut |
|---|---|
| FRED historique | Compatible |
| FRED++ | À valider selon l'implémentation |

## Voir aussi

Aucun lien associé n'est encore défini.
