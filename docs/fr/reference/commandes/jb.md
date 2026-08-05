# JB — Saut d'interruption ( Jump Break )

> La commande JB permet l'abandon de la commande U en fonction de l'indicateur booléen.

## Syntaxe

```fred
JB[T|F]
```

## Paramètres

Aucun paramètre explicite n'est documenté.

## Description

La commande JB permet l'abandon de la commande U en fonction de l'indicateur booléen.

## Exemples

```fred
b(buf)
1zu10t .+1t/4$/ jf jm/Arret sur la ligne numero / = jbArret sur la ligne numero 4
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
- [JO](jo.md)
- [U](u.md)
