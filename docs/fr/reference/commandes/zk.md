# ZK — Copie lignes dans un buffer désigné à la suite ( Zap Kopie )

> La commande ZK copie les lignes spécifiées du buffer courant dans le buffer désigné, après la ligne courante "." du buffer désigné.

## Syntaxe

```fred
(.,.)ZK<nom de buffer>
```

## Paramètres

| Élément | Description |
|---|---|
| `(.,.)` | Adresse ou plage de lignes optionnelle, selon la forme indiquée. |
| `<nom de buffer>` | Nom du buffer concerné. |

## Description

La commande ZK copie les lignes spécifiées du buffer courant dans le buffer désigné, après la ligne courante "." du buffer désigné.

Le <nom de buffer> peut désigner le buffer courant, mais les lignes spécifiées ne peuvent alors inclure la ligne courante. Les adresses des lignes courantes "." des buffers émetteur et récepteur pointent sur la dernière ligne copiée, respectivement.

## Exemples

```fred
b(buf)
1zk(bufk)3zk(bufk)b(bufk)*Ajout  ligne de texte numero 1Ajout  ligne de texte numero 3
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
