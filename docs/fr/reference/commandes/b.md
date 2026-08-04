# B — Positionnement sur un buffer (Buffer)

> Sélectionne un buffer existant ou crée un nouveau buffer.

## Syntaxe

```fred
B<nom de buffer>
```

## Paramètres

| Élément | Description |
|---|---|
| `<nom de buffer>` | Nom du buffer à sélectionner ou à créer. |

## Description

La commande **B** permet de se positionner sur un buffer. Si le buffer n'existe pas encore, il est créé automatiquement.

Le nom d'un buffer peut comporter jusqu'à 15 caractères et doit normalement être placé entre parenthèses.

## Exemples

```fred
b(buf)
b0
```

Si le nom n'est pas valide dans le mode courant :

```text
? buff/reg name invalid
```

Puis :

```fred
o-i(
b0
```

## Options

### `O-I(`

Lorsque l'option `O-I(` est activée, les parenthèses ne sont pas obligatoires pour les noms de buffers composés d'un seul caractère.

## Remarques

Le buffer sélectionné devient le buffer courant.

## Compatibilité

| Implémentation | Statut |
|---|---|
| FRED historique | Compatible |
| FRED++ | Compatible si la gestion des buffers est implémentée |

## Voir aussi

- [A — Append](a.md)
- [ZD — Suppression de buffer](zd.md)
- [FB — Informations sur les buffers](fb.md)
