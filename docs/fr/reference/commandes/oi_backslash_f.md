# OI\F — \F nécessaire ( Option Input \F )

> L'option OI\F permet de rendre obligatoire ou non le \F pour délimiter la fin du mode "entrée" pour les commandes du type A<bl><texte>, I<bl><texte>, et C<bl><texte>.

## Syntaxe

```fred
O[+|-]I\F
```

## Paramètres

Aucun paramètre explicite n'est documenté.

## Description

L'option OI\F permet de rendre obligatoire ou non le \F pour délimiter la fin du mode "entrée" pour les commandes du type A<bl><texte>, I<bl><texte>, et C<bl><texte>.

## Exemples

```fred
b(buf)
a Ajout  d'une ligne  1 dans un buffer>\fo-i\fa Ajout  d'une ligne 2 dans un buffer*Ajout  d'une ligne  1 dans un bufferAjout  d'une ligne  2 dans un buffer
```

## Options

Valeur implicite O+I\F.

## Remarques

Aucune remarque spécifique n'est documentée.

## Compatibilité

| Implémentation | Statut |
|---|---|
| FRED historique | Compatible |
| FRED++ | À valider selon l'implémentation |

## Voir aussi

Aucun lien associé n'est encore défini.
