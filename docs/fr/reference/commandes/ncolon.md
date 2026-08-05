# N: — Affectation de valeur à un registre ( Numeric )

> La commande N: permet l'affectation d'une valeur numérique à un registre.

## Syntaxe

```fred
(.)N<nom de registre>:<nombre>
```

## Paramètres

| Élément | Description |
|---|---|
| `(.)` | Adresse ou plage de lignes optionnelle, selon la forme indiquée. |
| `<nom de registre>` | Nom du registre numérique. |
| `<nombre>` | Valeur numérique. |

## Description

La commande N: permet l'affectation d'une valeur numérique à un registre. On peut aussi utiliser des caractères spéciaux associés à des registres comme nombre : "#" pour la valeur du registre compteur, "." pour le numéro de la ligne courante, "$" pour le numéro de la dernière ligne courante.

## Exemples

```fred
n(reg1):10
n(reg2):20fnn(reg1):10n(reg2):20b(buf)n(nbbuf):$      n(nbbuf)p4
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
