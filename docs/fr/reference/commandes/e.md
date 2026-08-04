# E — Définition de modèle (Pattern)

> Associe un nom à un modèle de recherche.

## Syntaxe

```fred
E<nom de modèle>/<modèle>/
```

## Paramètres

| Élément | Description |
|---|---|
| `<nom de modèle>` | Nom logique attribué au modèle. |
| `<modèle>` | Expression décrivant le jeu de caractères recherché. |

## Description

La commande **E** permet d'associer un nom à un jeu de caractères représentant un modèle.

Le nom du modèle peut comporter jusqu'à 15 caractères et doit être placé entre parenthèses.

## Exemples

```fred
b(buf)
e(term par 1)/1$/
/\E(term par 1)/
```

Résultat :

```text
Ajout ligne de texte numero 1
```

## Options

Aucune option spécifique n'est documentée.

## Remarques

Un modèle nommé peut ensuite être réutilisé à l'aide de la directive `\E`.

## Compatibilité

| Implémentation | Statut |
|---|---|
| FRED historique | Compatible |
| FRED++ | Compatible si les modèles nommés sont implémentés |

## Voir aussi

- [FE — Informations sur les modèles](fe.md)
- [S — Substitution](s.md)
- [T — Test](t.md)
