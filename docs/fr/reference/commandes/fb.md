# FB — Impression d'informations sur les buffers

> La commande FB imprime l'ensemble des buffers ouverts par un utilisateur sous le sous-système FRED.

## Syntaxe

```fred
FB
```

## Paramètres

Aucun paramètre explicite n'est documenté.

## Description

La commande FB imprime l'ensemble des buffers ouverts par un utilisateur sous le sous-système FRED. Les buffers sont affichés par ordre décroissant selon la dernière utilisation ( c'est à dire : dernier ouvert, premier affiché )

## Exemples

```fred
fb
b(buf) 4,4 /fred/parex ?b(bufi) 4,4b(fo) 15,15
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

- [B](b.md)
- [F](f.md)
- [FF](ff.md)
