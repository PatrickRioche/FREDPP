# OQ — Abandon FRED si incident  ( Option Quit )

> L'option OQ permet l'abandon de la procédure en cas d'incident FRED.

## Syntaxe

```fred
O[+|-]Q
```

## Paramètres

Aucun paramètre explicite n'est documenté.

## Description

L'option OQ permet l'abandon de la procédure en cas d'incident FRED.

## Exemples

```fred
fred
a>o+qa\c\r\c\fwa *listq!l *list>\fwa *proc1,5,28 t b(0) *proc !fred *proc><$*$brk>!1dw1,4,24 t b(0) *proc!fred *proc><$*$brk>?qq!qq
```

## Options

Valeur par défaut O-Q .

## Remarques

Aucune remarque spécifique n'est documentée.

## Compatibilité

| Implémentation | Statut |
|---|---|
| FRED historique | Compatible |
| FRED++ | À valider selon l'implémentation |

## Voir aussi

Aucun lien associé n'est encore défini.
