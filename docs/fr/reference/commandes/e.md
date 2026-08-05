# E — Définition de modèle ( Pattern )

> La commande E permet d'associer un nom à un jeu de caractères représentant un modèle.

## Syntaxe

```fred
E<nom de modèle>/<modèle>/
```

## Paramètres

| Élément | Description |
|---|---|
| `<nom de modèle>` | Nom attribué au modèle. |
| `<modèle>` | Modèle de recherche FRED. |

## Description

La commande E permet d'associer un nom à un jeu de caractères représentant un modèle.

Le nom des modèles peut faire 15 caractères maximum et doit être entre parenthèses.

## Exemples

```fred
b(buf)
e(term par 1)/1$//\E(term par 1)/Ajout  ligne de texte numero 1
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

- [FE](fe.md)
- [S](s.md)
- [T](t.md)
