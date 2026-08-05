# ZU — Conversion en majuscules ( Zap Upper )

> La commande ZU convertit toutes les lettres en majuscules; le nombre de lignes modifiées est mis dans le registre compteur ( # ).

## Syntaxe

```fred
(.,.)ZU[/<modèle>/]
```

## Paramètres

| Élément | Description |
|---|---|
| `(.,.)` | Adresse ou plage de lignes optionnelle, selon la forme indiquée. |
| `<modèle>` | Modèle de recherche FRED. |

## Description

La commande ZU convertit toutes les lettres en majuscules; le nombre de lignes modifiées est mis dans le registre compteur ( # ).

La commande ZU/<modèle>/ met toutes les chaînes de caractères correspondant au modèle en majuscules; le nombre de chaînes converties est mis dans le registre compteur ( # ).

L'adresse de la ligne courante "." pointe sur la dernière ligne où des caractères ont été convertis.

## Exemples

```fred
b(buf)
1zu2zu/^./? no text changed1,2pAJOUT  LIGNE DE TEXTE NUMERO 1Ajout  ligne de texte numero 2
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
