# N* — Multiplication d'une valeur dans un registre

> La commande N* multiplie un registre par une valeur ou le contenu d'un autre registre et retourne le résultat dans ce registre.

## Syntaxe

```fred
N<nom registre>*<nombre>
```

## Paramètres

| Élément | Description |
|---|---|
| `<nom registre>` | Nom du registre numérique. |
| `<nombre>` | Valeur numérique. |

## Description

La commande N* multiplie un registre par une valeur ou le contenu d'un autre registre et retourne le résultat dans ce registre.

## Exemples

```fred
n(reg1):6
n(reg1)*6 n(reg1)p
36
n(reg2):2n(reg2)*\N(reg1) n(reg2)p
72
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
