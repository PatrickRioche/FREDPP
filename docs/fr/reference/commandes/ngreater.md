# N> — Test de supériorité d'une valeur avec un registre

> La commande N> effectue un test de supériorité entre un nombre et le contenu d'un registre, et positionne l'indicateur booléen à "VRAI" (True) ou "FAUX" (False).

## Syntaxe

```fred
N<nom registre>><nombre>
```

## Paramètres

| Élément | Description |
|---|---|
| `<nom registre>` | Nom du registre numérique. |
| `<nombre>` | Valeur numérique. |

## Description

La commande N> effectue un test de supériorité entre un nombre et le contenu d'un registre, et positionne l'indicateur booléen à "VRAI" (True) ou "FAUX" (False).

## Exemples

```fred
n(reg1):1
n(reg1)>0 j(sup0)t....................@(sup0)
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
