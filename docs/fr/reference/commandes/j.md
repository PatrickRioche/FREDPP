# J — Sauts et branchements (Jump)

> La commande J est un saut inconditionnel à une étiquette.

## Syntaxe

```fred
J(label)

J<label>[T|F ]<commandes><nl>

J[T|F]
```

## Paramètres

| Élément | Description |
|---|---|
| `(label)` | Adresse ou plage de lignes optionnelle, selon la forme indiquée. |
| `<label>` | Nom d'étiquette. |
| `<commandes>` | Commandes FRED à exécuter. |
| `<nl>` | Fin de ligne. |

## Description

La commande J est un saut inconditionnel à une étiquette. Cette commande est similaire au GOTO des autres langages.

Quand FRED rencontre la commande J(label) il recherche la première étiquette @(label) à partir de la ligne courante et s'y positionne pour continuer son traitement sinon il y aura génération d'un message d'erreur  : "? label not found".

Le nom des étiquettes peut faire 15 caractères maximum et doit être entre parenthèses.

La commande J(label)T va à l'étiquette @(label) si le registre de condition est "VRAI" (True).

La commande J(label)F va à l'étiquette @(label) si le registre de condition est "FAUX" (False).

La commande JT abandonne le reste de la ligne si le registre de condition est "VRAI" (True).

La commande JF abandonne le reste de la ligne si le registre de condition est "FAUX" (False).

## Exemples

```fred
j(label)
..............................@(label)

b(buf)
*t/^Ajout/ j(YaAjout)t                 j(YaPasAjout)@(YaAjout)....................j(FinSi)@(YaPasAjout)....................@(FinSi)

b(buf)
*t/^Ajout/ jf j(YaAjout)....................j(FinSi)@(YaAjout)....................@(Finsi)
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

- [JM](jm.md)
- [JP](jp.md)
- [JE](je.md)
- [JO](jo.md)
- [JB](jb.md)
