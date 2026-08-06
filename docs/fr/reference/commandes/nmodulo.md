# N% — Reste de la division d'une valeur dans un registre

> La commande N% retourne le reste de la division entière d'une valeur par le contenu d'une valeur ou d'un autre registre et met le résultat dans ce registre.

## Syntaxe

```fred
N<nom registre>%<nombre>
```

## Paramètres

| Élément | Description |
|---|---|
| `<nom registre>` | Nom du registre numérique. |
| `<nombre>` | Valeur numérique. |

## Description

La commande N% retourne le reste de la division entière d'une valeur par le contenu d'une valeur ou d'un autre registre et met le résultat dans ce registre.

## Exemples

```fred
n(reg1):20
n(reg1)%8 n(reg1)p
4
n(reg2):20
n(reg2)*\N(reg1) n(reg2)p
0
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
