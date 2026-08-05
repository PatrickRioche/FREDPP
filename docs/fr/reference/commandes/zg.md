# ZG — Exécution des commandes et ramassage dans un buffer, de la production destinée à la visualisation

> La commande ZG exécute les commandes et met le résultat destiné à la visualisation , dans un buffer.

## Syntaxe

```fred
ZG<nom de buffer><commandes><nl>
```

## Paramètres

| Élément | Description |
|---|---|
| `<nom de buffer>` | Nom du buffer concerné. |
| `<commandes>` | Commandes FRED à exécuter. |
| `<nl>` | Fin de ligne. |

## Description

La commande ZG exécute les commandes et met le résultat destiné à la visualisation , dans un buffer. Le texte produit par l'exécution des commandes et qui normalement est affiché à l'écran est inséré dans le buffer désigné, après la ligne courante ( "." ).

## Exemples

```fred
zg(bufg)fv
b(bufg)*compiled Fri May  3 15:11:45 1991 using workfile fc*wk
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
