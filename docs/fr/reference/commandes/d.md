# D — Suppression de lignes (Delete)

> Supprime une ligne ou un groupe de lignes du buffer courant.

## Syntaxe

```fred
(.,.)D
```

## Paramètres

| Élément | Description |
|---|---|
| `(.,.)` | Adresse ou plage de lignes optionnelle. |

## Description

La commande **D** permet de supprimer une ligne ou un groupe de lignes dans le buffer courant.

Sans adresse explicite, la commande s'applique à la ligne courante.

## Exemples

```fred
b(buf)
4d
*
```

Résultat :

```text
Ajout ligne de texte numero 1
Ajout ligne de texte numero 2
Ajout ligne de texte numero 3
```

Suppression de toutes les lignes :

```fred
*d
*
```

Résultat :

```text
? empty buffer
```

## Options

Aucune option spécifique n'est documentée.

## Remarques

Après suppression de toutes les lignes, le buffer est vide.

## Compatibilité

| Implémentation | Statut |
|---|---|
| FRED historique | Compatible |
| FRED++ | Compatible |

## Voir aussi

- [A — Append](a.md)
- [C — Change](c.md)
- [V — Annulation](v.md)
