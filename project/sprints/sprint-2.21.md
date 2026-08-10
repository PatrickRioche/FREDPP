# Sprint 2.21 — Élargissement du bootstrap historique

## Lot 1 — appel des procédures et B(0)

`fredpp hello` résout `hello.fredpp`.

Sous Windows :

```text
.\hello.fredpp
C:\fredpp\library\hello.fredpp
```

Les chemins explicites restent acceptés.

`fredpp aide hello` exécute `aide.fredpp` et initialise :

```text
B(0)
hello
```

Chaque argument supplémentaire occupe une ligne de `B(0)`. Le programme est
chargé dans `B(.)`.

## Lot 2 — prévu

- `B(d)` : date ;
- `B(t)` : heure ;
- `B(u)` : utilisateur.

## Lot 3 — prévu

- `.init`.

## Contraintes

- références `docs/fr/reference/commandes` inchangées ;
- PRECHECK complet avant écriture ;
- écriture atomique et rollback ;
- aucun changement automatique du numéro de release.
