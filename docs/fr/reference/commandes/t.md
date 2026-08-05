# T — Test de conformité à un modèle ( Test )

> La commande T permet de tester l'existence d'un modèle dans une ligne ou un groupe de lignes, dans ce cas le registre de condition est chargé à "VRAI" (True) si le modèle est trouvé, sinon il est mis à "FAUX" (False).

## Syntaxe

```fred
(.,.)T[~]/<modèle>/
```

## Paramètres

| Élément | Description |
|---|---|
| `(.,.)` | Adresse ou plage de lignes optionnelle, selon la forme indiquée. |
| `<modèle>` | Modèle de recherche FRED. |

## Description

La commande T permet de tester l'existence d'un modèle dans une ligne ou un groupe de lignes, dans ce cas le registre de condition est chargé à "VRAI" (True) si le modèle est trouvé, sinon il est mis à "FAUX" (False).

Si "~" précède <modèle>, le registre de condition est mis  à "FAUX" (False) si le modèle est trouvé, ou mis à "VRAI" (True) si le modèle n'est pas trouvé.

## Exemples

```fred
b(buf)
1pAjout  ligne de texte numero 11t/^ajout/ jf jm/Ya ajout en debut de ligne/Ya ajout en debut de ligne1t~/colonne/ jf jm/Ya pas colonne dans la ligne/Ya pas colonne dans la ligne
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
