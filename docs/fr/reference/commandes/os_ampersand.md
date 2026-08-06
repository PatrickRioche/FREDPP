# OS& — & sens spécial dans la commande S ( Option Substitut & )

> L'option OS& permet de valider ou d'invalider le sens spécial du caractère & dans la commande S.

## Syntaxe

```fred
O[+|-]S&
```

## Paramètres

Aucun paramètre explicite n'est documenté.

## Description

L'option OS& permet de valider ou d'invalider le sens spécial du caractère & dans la commande S.

## Exemples

```fred
o-s&
b(bufo)
a
>
MAJUSCULE
>
\f
s/.*/& &/p
& &
o+s&
s/.*/& &/p
MAJUSCULE MAJUSCULE
```

## Options

Valeur implicite O+S&.

## Remarques

Aucune remarque spécifique n'est documentée.

## Compatibilité

| Implémentation | Statut |
|---|---|
| FRED historique | Compatible |
| FRED++ | À valider selon l'implémentation |

## Voir aussi

Aucun lien associé n'est encore défini.
