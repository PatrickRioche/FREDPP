# ZM — Déplacement lignes dans un buffer à la suite ( Zap Move )

> La commande ZM supprime les lignes sélectionnées du buffer courant et les place dans le buffer désigné, à la suite de la ligne courante de ce buffer.

## Syntaxe

```fred
(.,.)ZM<nom de buffer>
```

## Paramètres

| Élément | Description |
|---|---|
| `(.,.)` | Adresse ou plage de lignes optionnelle, selon la forme indiquée. |
| `<nom de buffer>` | Nom du buffer concerné. |

## Description

La commande ZM supprime les lignes sélectionnées du buffer courant et les place dans le buffer désigné, à la suite de la ligne courante de ce buffer.

Le <nom de buffer> peut désigner le buffer courant, mais alors les lignes spécifiées ne peuvent pas contenir la ligne courante ".". La ligne courante "." du buffer émetteur est positionnée sur la ligne qui suit la dernière ligne déplacée; la ligne courante "." du buffer récepteur est mise sur la dernière ligne rajoutée.

## Exemples

```fred
b(buf)
1zm(bufm)
3zk(bufm)
*
Ajout  ligne de texte numero 2
Ajout  ligne de texte numero 3
Ajout  ligne de texte numero 4
b(bufm)
*
Ajout  ligne de texte numero 1
Ajout  ligne de texte numero 4
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
