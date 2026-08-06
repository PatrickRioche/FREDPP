# ZD — Suppression de buffer

> La commande ZD supprime le contenu du buffer, puis le buffer lui-même.

## Syntaxe

```fred
ZD<nom de buffer>
```

## Paramètres

| Élément | Description |
|---|---|
| `<nom de buffer>` | Nom du buffer concerné. |

## Description

La commande ZD supprime le contenu du buffer, puis le buffer lui-même.

## Exemples

```fred
b(buf)
fb(buf) 4,4 /fred/parexb ? BCD card
zd(buf)
f
b(buf) 0,0
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
