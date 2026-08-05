# M — Déplacement de lignes vers un buffer (Move)

> La commande M copie les lignes sélectionnées dans le buffer désigné, et les supprime du buffer courant.

## Syntaxe

```fred
(.,.)M<nom de buffer>
```

## Paramètres

| Élément | Description |
|---|---|
| `(.,.)` | Adresse ou plage de lignes optionnelle, selon la forme indiquée. |
| `<nom de buffer>` | Nom du buffer concerné. |

## Description

La commande M copie les lignes sélectionnées dans le buffer désigné, et les supprime du buffer courant. Le contenu antérieur du buffer désigné  est perdu.

L'adresse de la ligne courante "." du buffer désigné pointe sur la dernière ligne. Le "." du buffer courant pointe sur la ligne qui suit la dernière ligne déplacée.

## Exemples

```fred
b(buf)
1,2m(bufm)*Ajout  ligne de texte numero 3Ajout  ligne de texte numero 4b(bufm)*Ajout  ligne de texte numero 1Ajout  ligne de texte numero 2
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

- [K](k.md)
- [ZK](zk.md)
- [ZM](zm.md)
