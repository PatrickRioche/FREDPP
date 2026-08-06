# OIR — Définition prompt pour return ( Option Input Return )

> L'option OIR permet de définir la chaîne de caractères envoyée à chaque return.

## Syntaxe

```fred
O[+|-]IR/<chaîne>/
```

## Paramètres

| Élément | Description |
|---|---|
| `<chaîne>` | Chaîne de caractères. |

## Description

L'option OIR permet de définir la chaîne de caractères envoyée à chaque return.

## Exemples

```fred
o+ir/<RETURN>/
<RETURN>
b(bufo)
<RETURN>
a
<RETURN>
>
Ajout ligne dans le buffer
<RETURN>
>
\F<RETURN>
p
<RETURN>
Ajout ligne dans le buffer
```

## Options

Valeur implicite O-IR.

## Remarques

Aucune remarque spécifique n'est documentée.

## Compatibilité

| Implémentation | Statut |
|---|---|
| FRED historique | Compatible |
| FRED++ | À valider selon l'implémentation |

## Voir aussi

Aucun lien associé n'est encore défini.
