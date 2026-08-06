# OIE — Définition caractère d'échappement ( Option Input Escape )

> L'option OIE permet la définition du caractère d'échappement pour les entrées sur un terminal ne disposant pas du caractère "\".

## Syntaxe

```fred
O[+|-]IE<caractère>
```

## Paramètres

| Élément | Description |
|---|---|
| `<caractère>` | Caractère utilisé par la commande ou l'option. |

## Description

L'option OIE permet la définition du caractère d'échappement pour les entrées sur un terminal ne disposant pas du caractère "\".

## Exemples

```fred
o+ie!
b(bufo)
a
>
Ajout ligne dans le buffer
>
!F
p
Ajout ligne dans le buffer
```

## Options

Valeur implicite O+IE\

## Remarques

Aucune remarque spécifique n'est documentée.

## Compatibilité

| Implémentation | Statut |
|---|---|
| FRED historique | Compatible |
| FRED++ | À valider selon l'implémentation |

## Voir aussi

Aucun lien associé n'est encore défini.
