# OIA — Définition prompt mode entrée ( Option Input Append )

> L'option OIA permet de définir la chaîne de caractères représentant le prompt du mode entrée.

## Syntaxe

```fred
O[+|-]IA/<chaîne>/
```

## Paramètres

| Élément | Description |
|---|---|
| `<chaîne>` | Chaîne de caractères. |

## Description

L'option OIA permet de définir la chaîne de caractères représentant le prompt du mode entrée.

## Exemples

```fred
o+ia/Mode IN>/
b(bufo)
a
Mode IN>
Ajout ligne dans le buffer
Mode IN>
\F
p
Ajout ligne dans le buffer
```

## Options

Valeur implicite O+A/>/

## Remarques

Aucune remarque spécifique n'est documentée.

## Compatibilité

| Implémentation | Statut |
|---|---|
| FRED historique | Compatible |
| FRED++ | À valider selon l'implémentation |

## Voir aussi

Aucun lien associé n'est encore défini.
