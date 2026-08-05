# A — Insertion  après la ligne courante ( Append)

> La commande A permet d'entrer du texte dans un buffer.

## Syntaxe

```fred
(.)A<bl><texte>
```

## Paramètres

| Élément | Description |
|---|---|
| `(.)` | Adresse ou plage de lignes optionnelle, selon la forme indiquée. |
| `<bl>` | Un espace. |
| `<texte>` | Texte ou lignes de texte traités par la commande. |

## Description

La commande A permet d'entrer du texte dans un buffer. On peut insérer ce texte après un numéro de ligne précis ou si aucun numéro de ligne n'est précisé l'insertion s'effectuera après la ligne courante.

## Exemples

```fred
b(buf)
a
>
Ajout  ligne de texte numero 1
>
\F
$a 
>
Ajout  ligne de texte numero 2
Ajout  ligne de texte numero 3
>\F
o-i\F
$a Ajout  ligne de texte numero 4
*
Ajout  ligne de texte numero 1
Ajout  ligne de texte numero 2
Ajout  ligne de texte numero 3
Ajout  ligne de texte numero 4
```

## Options

Si l'option O-I\F est validée, le texte pourra être entré directement derrière la commande A et il ne sera pas nécessaire de terminer l'entrée par un \F.

## Remarques

Le buffer "buf" avec son contenu nous servira de base pour les exemples des autres commandes.

## Compatibilité

| Implémentation | Statut |
|---|---|
| FRED historique | Compatible |
| FRED++ | À valider selon l'implémentation |

## Voir aussi

- [I](i.md)
- [C](c.md)
- [D](d.md)
