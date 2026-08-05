# V — Défaire la dernière ligne de commandes ( Void )

> La commande V inverse les résultats de la dernière ligne de commandes contenant une commande active; les commandes actives sont : a, b, c, d, e, i, k, m, n, o, r, s, za, zb, zd, zi, zk, zl, zm, zs, zt, zu, zv; et f et...

## Syntaxe

```fred
V
```

## Paramètres

Aucun paramètre explicite n'est documenté.

## Description

La commande V inverse les résultats de la dernière ligne de commandes contenant une commande active; les commandes actives sont : a, b, c, d, e, i, k, m, n, o, r, s, za, zb, zd, zi, zk, zl, zm, zs, zt, zu, zv; et f et w, quand elles spécifient un nouveau nom de fichier. On peut "défaire" jusqu'aux 10 dernières commandes. Pour défaire  la commande V, il faut utiliser la commande ZV.

## Exemples

```fred
b(buf)
*Ajout  ligne de texte numero 1Ajout  ligne de texte numero 2Ajout  ligne de texte numero 3Ajout  ligne de texte numero 41d*Ajout  ligne de texte numero 2Ajout  ligne de texte numero 3Ajout  ligne de texte numero 4v*Ajout  ligne de texte numero 1Ajout  ligne de texte numero 2Ajout  ligne de texte numero 3Ajout  ligne de texte numero 4
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
