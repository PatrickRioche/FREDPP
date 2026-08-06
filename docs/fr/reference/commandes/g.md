# G — Global commande ( Global )

> La commande G copie les commandes à l'intérieur d'un buffer secret et les exécute pour chaque ligne correspondant au modèle.

## Syntaxe

```fred
(1,$)G[~]/<modèle>/<commandes><nl>
```

## Paramètres

| Élément | Description |
|---|---|
| `(1,$)` | Adresse ou plage de lignes optionnelle, selon la forme indiquée. |
| `<modèle>` | Modèle de recherche FRED. |
| `<commandes>` | Commandes FRED à exécuter. |
| `<nl>` | Fin de ligne. |

## Description

La commande G copie les commandes à l'intérieur d'un buffer secret et les exécute pour chaque ligne correspondant au modèle.

Si "~" précède le modèle, on exécute les commandes pour chaque ligne ne correspondant pas au modèle.

A chaque itération le modèle courant (//) est chargé avec le modèle de la global. Le nombre de lignes répondant au modèle est mis dans le registre compteur (#).

L'adresse de la ligne courante (".") est chargée à la valeur qu'elle avait après l'exécution de la dernière commande.

## Exemples

```fred
b(buf)
g/ligne/p
Ajout  ligne de texte numero 1
Ajout  ligne de texte numero 2
Ajout  ligne de texte numero 3
Ajout  ligne de texte numero 4
g~/1/p
Ajout  ligne de texte numero 2
Ajout  ligne de texte numero 3
Ajout  ligne de texte numero 4
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

- [T](t.md)
- [U](u.md)
- [J](j.md)
