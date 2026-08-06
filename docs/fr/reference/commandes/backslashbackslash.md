# \\ — Attente

> La commande \n provoque une boucle d'attente de n secondes.

## Syntaxe

```fred
\\<n>
```

fredfred## Paramètres

| Élément | Description    |
| --------- | -------------- |
| `<n>`   | Nombre entier. |

## Description

La commande \n provoque une boucle d'attente de n secondes. La limite maximum est de 24 heures.

La commande \\0 provoque un vidage immédiat à l'écran des messages stockés dans les buffers du TSS.

## Exemples

```fred
jm/J'attends 1 heure/ \\0
\\3600
jm/Deja un heure/ \\0
```

## Options

Aucune option spécifique n'est documentée.

## Remarques

Aucune remarque spécifique n'est documentée.

## Compatibilité

| Implémentation | Statut                             |
| --------------- | ---------------------------------- |
| FRED historique | Compatible                         |
| FRED++          | À valider selon l'implémentation |

## Voir aussi

Aucun lien associé n'est encore défini.
