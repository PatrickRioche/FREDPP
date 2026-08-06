# N| — Valeur absolue d'un registre

> La commande N| calcul la valeur absolue du registre et retourne le résultat dans ce registre.

## Syntaxe

```fred
N<nom registre>|

N<nom registre>|<nombre>
```

## Paramètres

| Élément | Description |
|---|---|
| `<nom registre>` | Nom du registre numérique. |
| `<nombre>` | Valeur numérique. |

## Description

La commande N| calcul la valeur absolue du registre et retourne le résultat dans ce registre.

La commande N| fait un OU inclusif bit à bit du contenu du registre avec une valeur et met le résultat dans ce registre.

## Exemples

```fred
n(reg1):-5
n(reg1)|    n(reg1)p
5
n(reg1):7
n(reg1)|8   n(reg1)p
15
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
