# N- — Soustraction d'une valeur dans un registre

> La commande N- soustrait une valeur au contenu d'un registre et retourne le résultat dans ce registre.

## Syntaxe

```fred
N<nom registre>-<nombre>
```

## Paramètres

| Élément | Description |
|---|---|
| `<nom registre>` | Nom du registre numérique. |
| `<nombre>` | Valeur numérique. |

## Description

La commande N- soustrait une valeur au contenu d'un registre et retourne le résultat dans ce registre.

## Exemples

```fred
n(reg1):10
n(reg1)-6 n(reg1)p4n(reg2):10n(reg2)-\N(reg1) n(reg2)p6
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
