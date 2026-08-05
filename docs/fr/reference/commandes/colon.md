# : — Imprime une fenêtre autour d'une ligne

> La commande : imprime une fenêtre de <n> ligne autour de la ligne courante.

## Syntaxe

```fred
:<n>
```

## Paramètres

| Élément | Description |
|---|---|
| `<n>` | Nombre entier. |

## Description

La commande : imprime une fenêtre de <n> ligne autour de la ligne courante. L'adresse de la ligne courante prend la valeur de la dernière ligne affichée. La commande : est équivalent à ".-5,.+5p" en implicite.

## Exemples

```fred
b(buf)
:1Ajout  ligne de texte numero 1 Ajout  ligne de texte numero 2 :1Ajout  ligne de texte numero 1 Ajout  ligne de texte numero 2 Ajout  ligne de texte numero 3 :1Ajout  ligne de texte numero 2 Ajout  ligne de texte numero 3 Ajout  ligne de texte numero 4 :1Ajout  ligne de texte numero 3 Ajout  ligne de texte numero 4
```

## Options

Valeur implicite de <n> est de 5 lignes.

## Remarques

Aucune remarque spécifique n'est documentée.

## Compatibilité

| Implémentation | Statut |
|---|---|
| FRED historique | Compatible |
| FRED++ | À valider selon l'implémentation |

## Voir aussi

Aucun lien associé n'est encore défini.
