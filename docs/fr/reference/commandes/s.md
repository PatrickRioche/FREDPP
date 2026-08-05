# S — ( Substitution )

> La commande S permet de substituer un modèle par une chaîne de caractères.

## Syntaxe

```fred
(.,.)S/<modèle>/<chaîne>/
```

## Paramètres

| Élément | Description |
|---|---|
| `(.,.)` | Adresse ou plage de lignes optionnelle, selon la forme indiquée. |
| `<modèle>` | Modèle de recherche FRED. |
| `<chaîne>` | Chaîne de caractères. |

## Description

La commande S permet de substituer un modèle par une chaîne de caractères.

L'adresse de la ligne courante "." pointe sur la dernière ligne modifiée par des substitutions.

Si aucune substitution n'est faite, le registre de condition est chargé à "FAUX" (False), sinon il est mis à "VRAI" (True). Il y aura génération d'un message d'erreur si aucune commande de substitution n'a été réalisée et si la commande n'a pas été exécutée à partir d'un buffer.

## Exemples

```fred
b(buf)
1s/ligne/colonne/pAjout  colonne de texte numero 11s/.*/& &/pAjout  colonne de texte numero 1 Ajout  colonne de texte numero 11s1/colonne/ligne/pAjout  ligne de texte numero 1 Ajout  colonne de texte numero 11s-1/de texte.*$//Ajout  ligne de texte numero 1 Ajout  colonne1s/paragraphe//? no text changed
```

## Options

L'option O-S/ permet d'invalider l'utilisation du séparateur de fin dans la commande S. La valeur par défaut est O+S/.

## Remarques

Aucune remarque spécifique n'est documentée.

## Compatibilité

| Implémentation | Statut |
|---|---|
| FRED historique | Compatible |
| FRED++ | À valider selon l'implémentation |

## Voir aussi

Aucun lien associé n'est encore défini.
