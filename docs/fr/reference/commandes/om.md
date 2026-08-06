# OM — Passage mode trace ( Option Monitor )

> L'option OM permet de valider ou d'invalider le mode trace des commandes exécutées.

## Syntaxe

```fred
O[+|-]M
```

## Paramètres

Aucun paramètre explicite n'est documenté.

## Description

L'option OM permet de valider ou d'invalider le mode trace des commandes exécutées.

## Exemples

```fred
fred
a
>
o+m
a
\c\r\c\f
wa *list
q!l *list
>
\f
wa *proc
1,5,28 t b(0) *proc 
!fred *proc
>
Bonjour
Bonjour
\F
wa *list 
q!l *listBonjour!
```

## Options

Valeur par défaut O-M.

## Remarques

Aucune remarque spécifique n'est documentée.

## Compatibilité

| Implémentation | Statut |
|---|---|
| FRED historique | Compatible |
| FRED++ | À valider selon l'implémentation |

## Voir aussi

Aucun lien associé n'est encore défini.
