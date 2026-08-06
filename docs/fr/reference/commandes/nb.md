# NB — Changement de base d'un registre

> La commande NB change de base le registre et son contenu.

## Syntaxe

```fred
N<nom registre>B<nombre>
```

## Paramètres

| Élément | Description |
|---|---|
| `<nom registre>` | Nom du registre numérique. |
| `<nombre>` | Valeur numérique. |

## Description

La commande NB change de base le registre et son contenu. La base maximum est la base 36.

## Exemples

```fred
n(reg1):7
n(reg1)b2   n(reg1)p
111
n(reg1)b8   n(reg1)p
7
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
