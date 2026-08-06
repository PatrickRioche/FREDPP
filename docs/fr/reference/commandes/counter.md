# # — Impression du registre compteur

> La commande # imprime la valeur contenue dans le registre compteur.

## Syntaxe

```fred
#
```

## Paramètres

Aucun paramètre explicite n'est documenté.

## Description

La commande # imprime la valeur contenue dans le registre compteur. Ce registre est mis à jour par les commandes S, G, R, W ou L.

## Exemples

```fred
b(buf)
1p
Ajout  ligne de texte numero 1
1s/./&/#
30
n(car)lp
31
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
