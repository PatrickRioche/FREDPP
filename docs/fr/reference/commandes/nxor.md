# N^ — OU exclusif bit à bit d'une valeur dans un registre

> La commande N^ fait un OU exclusif bit à bit du contenu du registre avec une valeur et met le résultat dans ce registre.

## Syntaxe

```fred
N<nom registre>^<nombre>
```

## Paramètres

| Élément | Description |
|---|---|
| `<nom registre>` | Nom du registre numérique. |
| `<nombre>` | Valeur numérique. |

## Description

La commande N^ fait un OU exclusif bit à bit du contenu du registre avec une valeur et met le résultat dans ce registre.

## Exemples

```fred
n(reg1):15
n(reg1)^8   n(reg1)p7
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
