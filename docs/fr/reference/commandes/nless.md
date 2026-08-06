# N< — Test d'infériorité d'une valeur avec un registre

> La commande N< effectue un test d'infériorité entre un nombre et le contenu d'un registre, et positionne l'indicateur booléen à "VRAI" (True) ou "FAUX" (False).

## Syntaxe

```fred
N<nom registre><<nombre>
```

## Paramètres

| Élément | Description |
|---|---|
| `<nom registre>` | Nom du registre numérique. |
| `<<nombre>` | Élément défini par la syntaxe historique de la commande. |

## Description

La commande N< effectue un test d'infériorité entre un nombre et le contenu d'un registre, et positionne l'indicateur booléen à "VRAI" (True) ou "FAUX" (False).

## Exemples

```fred
n(reg1):1
n(reg1)<0 j(inf0)t
..........
..........
@(inf0)
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
