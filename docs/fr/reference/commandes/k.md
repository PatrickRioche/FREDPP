# K — Copie de lignes vers un buffer désigné ( Kopie )

> La commande K copie les lignes sélectionnées dans le buffer désigné.

## Syntaxe

```fred
(.,.)K<nom de buffer>
```

## Paramètres

| Élément | Description |
|---|---|
| `(.,.)` | Adresse ou plage de lignes optionnelle, selon la forme indiquée. |
| `<nom de buffer>` | Nom du buffer concerné. |

## Description

La commande K copie les lignes sélectionnées dans le buffer désigné. Le contenu antérieur du buffer désigné  est perdu. Les lignes successives, sans <cr> en fin de ligne, sont concaténées.

L'adresse de la ligne courante "." du buffer récepteur est positionnée sur la dernière ligne de ce buffer. Dans le buffer courant, "." pointe sur la dernière ligne  copiée.

## Exemples

```fred
b(buf)
1,2k(bufk)*Ajout  ligne de texte numero 1Ajout  ligne de texte numero 2Ajout  ligne de texte numero 3Ajout  ligne de texte numero 4b(bufk)*Ajout  ligne de texte numero 1Ajout  ligne de texte numero 2
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

- [ZK](zk.md)
- [M](m.md)
- [ZM](zm.md)
