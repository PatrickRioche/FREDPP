# OC — Changement de chaîne de caractères ( Option Change )

> L'option OC transforme la <chaîne1> en <chaîne2> partout où elle apparait en entrée.

## Syntaxe

```fred
O[+|-]C/<ch1>/<ch2>/
```

## Paramètres

| Élément | Description |
|---|---|
| `<ch1>` | Élément défini par la syntaxe historique de la commande. |
| `<ch2>` | Élément défini par la syntaxe historique de la commande. |

## Description

L'option OC transforme la <chaîne1> en <chaîne2> partout où elle apparait en entrée. La transformation ne s'effectue que pour l'ensemble des caractères de la chaîne. L'option O-C annule les dernières transformations.

## Exemples

```fred
o+c/Bonjour/Au revoir/
b(bufo)a>Bonjour>\fpAu revoir
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
