# N} — Décalage à droite bit à bit du nombre

> La commande N} effectue un décalage bit à bit vers la droite sur le contenu du registre indiqué, autant de fois que spécifie le <nombre>.

## Syntaxe

```fred
N<nom registre>}<nombre>
```

## Paramètres

| Élément | Description |
|---|---|
| `<nom registre>` | Nom du registre numérique. |
| `<nombre>` | Valeur numérique. |

## Description

La commande N} effectue un décalage bit à bit vers la droite sur le contenu du registre indiqué, autant de fois que spécifie le <nombre>.

## Exemples

```fred
n(reg1):28
n(reg1)}2   n(reg1)p7
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
