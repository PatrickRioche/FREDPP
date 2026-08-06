# JO — Saut hors du buffer courant ( Jump Out )

> La commande JO permet d'abandonner l'exécution du buffer en cours ou l'abandon d'une itération dans une commande G ou U en fonction de l'indicateur booléen.

## Syntaxe

```fred
JO[T|F]
```

## Paramètres

Aucun paramètre explicite n'est documenté.

## Description

La commande JO permet d'abandonner l'exécution du buffer en cours ou l'abandon d'une itération dans une commande G ou U en fonction de l'indicateur booléen.

## Exemples

```fred
b(exec) a
>
b(buf) *t/^Ajout/ jf jm/Ya Ajout/ jo
jm/Ya pas Ajout/
>
\f
\B(exec)
Ya Ajout
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

- [J](j.md)
- [JB](jb.md)
- [G](g.md)
- [U](u.md)
