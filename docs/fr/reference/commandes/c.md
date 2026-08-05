# C — Remplacement de n lignes ( Change )

> La commande C permet de changer des lignes de texte dans un buffer.

## Syntaxe

```fred
(.,.)C<bl><texte>
```

## Paramètres

| Élément | Description |
|---|---|
| `(.,.)` | Adresse ou plage de lignes optionnelle, selon la forme indiquée. |
| `<bl>` | Un espace. |
| `<texte>` | Texte ou lignes de texte traités par la commande. |

## Description

La commande C permet de changer des lignes de texte dans un buffer. On peut changer ce texte après un numéro de ligne précis ou si aucun numéro de ligne n'est précisé le remplacement s'effectuera après la ligne courante. Il est également possible de remplacer un groupe de ligne.

## Exemples

```fred
b(buf)
1c
>
Change ligne de texte numero 1
>\F
1,2c
>
Change ligne de texte numero 1
Change ligne de texte numero 2
>\F
o-i\F
$c Change ligne de texte numero 4
*
Change ligne de texte numero 1
Change ligne de texte numero 2
Ajout  ligne de texte numero 3
Change ligne de texte numero 4
```

## Options

Si l'option O-I\F est validée, le texte pourra être entré directement derrière la commande C et il ne sera pas nécessaire de terminer l'entrée par un \F.

## Remarques

Aucune remarque spécifique n'est documentée.

## Compatibilité

| Implémentation | Statut |
|---|---|
| FRED historique | Compatible |
| FRED++ | À valider selon l'implémentation |

## Voir aussi

- [A](a.md)
- [I](i.md)
- [D](d.md)
