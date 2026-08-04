# A — Insertion après la ligne courante (Append)

> Insère une ou plusieurs lignes de texte après la ligne courante ou après une adresse explicitement indiquée.

## Syntaxe

```fred
(.)A<bl><texte>
```

## Paramètres

| Élément | Description |
|---|---|
| `(.)` | Adresse optionnelle. Si elle est omise, l'insertion s'effectue après la ligne courante. |
| `<bl>` | Un espace. |
| `<texte>` | Une ou plusieurs lignes de texte à insérer. |

## Description

La commande **A** permet d'entrer du texte dans un buffer.

Si une adresse de ligne est précisée, le texte est inséré après cette ligne. Si aucune adresse n'est fournie, l'insertion s'effectue après la ligne courante.

## Exemples

```fred
b(buf)
a
Ajout ligne de texte numero 1
\F
$a
Ajout ligne de texte numero 2
Ajout ligne de texte numero 3
\F
o-i\F
$a Ajout ligne de texte numero 4
*
```

Résultat attendu :

```text
Ajout ligne de texte numero 1
Ajout ligne de texte numero 2
Ajout ligne de texte numero 3
Ajout ligne de texte numero 4
```

## Options

### `O-I\F`

Lorsque l'option `O-I\F` est activée, le texte peut être saisi directement après la commande `A`. Il n'est alors pas nécessaire de terminer l'entrée par `\F`.

## Remarques

Le buffer `buf` et son contenu servent de base aux exemples des autres commandes du manuel.

## Compatibilité

| Implémentation | Statut |
|---|---|
| FRED historique | Compatible |
| FRED++ | À valider selon l'implémentation |

## Voir aussi

- [I — Insertion avant la ligne courante](i.md)
- [C — Remplacement de lignes](c.md)
- [D — Suppression de lignes](d.md)
