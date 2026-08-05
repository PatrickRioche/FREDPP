# SPEC-022 — Commandes G et Z

## Sources normatives

- `docs/fr/reference/commandes/g.md`
- `docs/fr/reference/commandes/z.md`

Les fichiers normatifs ne sont pas modifiés par cette spécification.

## Commande G

### Syntaxe implémentée

```fred
(1,$)G[~]/<modèle>/<commande>
```

Sans adresse, la plage par défaut est `1,$`.

- `G/<modèle>/P` exécute `P` pour les lignes correspondant au modèle ;
- `G~/<modèle>/P` exécute `P` pour les lignes ne correspondant pas au modèle ;
- la même sélection est disponible avec `D` et `Z` ;
- la recherche est effectuée dans chaque ligne de la plage ;
- `#` reçoit en interne le nombre de lignes sélectionnées.

Cette étape accepte une seule commande imbriquée, sans adresse propre. Les
commandes prises en charge sont `P`, `D` et `Z`.

## Commande Z

### Syntaxe

```fred
(.)Z
```

`Z` positionne `.` sur la ligne désignée sans produire de sortie. Une plage de
lignes n'est pas admise.

## Point historique important

`Z` n'est pas l'inverse de `G`. La sélection inversée appartient à la forme
`G~`. La commande `Z` est la commande « Zap » de positionnement silencieux.
