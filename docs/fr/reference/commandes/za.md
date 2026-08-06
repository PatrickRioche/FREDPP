# ZA — ( Zap Append )

> La commande ZA insère le buffer désigné, après la ligne courante, dans le buffer courant.

## Syntaxe

```fred
(.)ZA<nom de buffer>
```

## Paramètres

| Élément | Description |
|---|---|
| `(.)` | Adresse ou plage de lignes optionnelle, selon la forme indiquée. |
| `<nom de buffer>` | Nom du buffer concerné. |

## Description

La commande ZA insère le buffer désigné, après la ligne courante, dans le buffer courant.

L'adresse de la ligne courante "." pointe sur la dernière ligne ajoutée.

## Exemples

```fred
b(buf)
*
Ajout  ligne de texte numero 1
Ajout  ligne de texte numero 2
Ajout  ligne de texte numero 3
Ajout  ligne de texte numero 4
1k(bufa)
$za(bufa)
*
Ajout  ligne de texte numero 1
Ajout  ligne de texte numero 2
Ajout  ligne de texte numero 3
Ajout  ligne de texte numero 4
Ajout  ligne de texte numero 1
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
