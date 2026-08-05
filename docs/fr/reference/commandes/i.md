# I — Insertion de texte avant la ligne courante ( Insert )

> La commande I permet d'entrer du texte dans un buffer.

## Syntaxe

```fred
(.)I<bl><texte>
```

## Paramètres

| Élément | Description |
|---|---|
| `(.)` | Adresse ou plage de lignes optionnelle, selon la forme indiquée. |
| `<bl>` | Un espace. |
| `<texte>` | Texte ou lignes de texte traités par la commande. |

## Description

La commande I permet d'entrer du texte dans un buffer. On peut insérer ce texte avant un numéro de ligne précis ou si aucun numéro de ligne n'est précisé l'insertion s'effectuera avant la ligne courante.

## Exemples

```fred
b(buf)
1i
>
Insert ligne de texte numero 0
>\F
o-i(
$i Insert ligne de texte numero $-1
*
Insert ligne de texte numero 0
Ajout  ligne de texte numero 1
Ajout  ligne de texte numero 2
Ajout  ligne de texte numero 3
Insert ligne de texte numero $-1
Ajout  ligne de texte numero 4
```

## Options

Si l'option o-i\F est validée, le texte pourra être entré directement derrière la commande I et il ne sera pas nécessaire de terminer l'entrée par un \F.

## Remarques

Aucune remarque spécifique n'est documentée.

## Compatibilité

| Implémentation | Statut |
|---|---|
| FRED historique | Compatible |
| FRED++ | À valider selon l'implémentation |

## Voir aussi

- [A](a.md)
- [C](c.md)
- [D](d.md)
