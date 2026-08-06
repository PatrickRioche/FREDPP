# B — Positionnement sur un buffer ( Buffer )

> La commande B permet de se positionner sur un buffer ou de le créer s'il n'existe pas déjà.

## Syntaxe

```fred
B<nom de buffer>
```

fred## Paramètres

| Élément           | Description              |
| ------------------- | ------------------------ |
| `<nom de buffer>` | Nom du buffer concerné. |

## Description

La commande B permet de se positionner sur un buffer ou de le créer s'il n'existe pas déjà.

Le nom des buffers peut faire 15 caractères maximum et doit être entre parenthèses.

## Exemples

```fred
b(buf)
b0? buff/reg name invalid
o-i(
b0
```

## Options

Si l'option O-I( est validée, les parenthèses ne seront pas obligatoires pour les noms de buffers composés d'une seule lettre.

## Remarques

Aucune remarque spécifique n'est documentée.

## Compatibilité

| Implémentation | Statut                             |
| --------------- | ---------------------------------- |
| FRED historique | Compatible                         |
| FRED++          | À valider selon l'implémentation |

## Voir aussi

- [FB](fb.md)
- [F](f.md)
- [ZD](zd.md)
