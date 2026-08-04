# C — Remplacement de lignes (Change)

> Remplace une ligne ou un groupe de lignes par un nouveau texte.

## Syntaxe

```fred
(.,.)C<bl><texte>
```

## Paramètres

| Élément | Description |
|---|---|
| `(.,.)` | Adresse ou plage de lignes optionnelle. |
| `<bl>` | Un espace. |
| `<texte>` | Texte de remplacement. |

## Description

La commande **C** permet de remplacer des lignes de texte dans un buffer.

Si une seule adresse est précisée, la ligne correspondante est remplacée. Si une plage est donnée, toutes les lignes de cette plage sont remplacées. Si aucune adresse n'est fournie, le remplacement s'applique à la ligne courante.

## Exemples

```fred
b(buf)
1c
Change ligne de texte numero 1
\F
1,2c
Change ligne de texte numero 1
Change ligne de texte numero 2
\F
o-i\F
$c Change ligne de texte numero 4
*
```

Résultat attendu :

```text
Change ligne de texte numero 1
Change ligne de texte numero 2
Ajout ligne de texte numero 3
Change ligne de texte numero 4
```

## Options

### `O-I\F`

Lorsque l'option `O-I\F` est activée, le texte peut être saisi directement après la commande `C`. Il n'est alors pas nécessaire de terminer l'entrée par `\F`.

## Remarques

La commande peut remplacer une seule ligne ou un groupe de lignes.

## Compatibilité

| Implémentation | Statut |
|---|---|
| FRED historique | Compatible |
| FRED++ | Compatible si le remplacement de plages est implémenté |

## Voir aussi

- [A — Append](a.md)
- [I — Insert](i.md)
- [D — Delete](d.md)
