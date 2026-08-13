# Sprint 2.22 — Compatibilité des procédures historiques

Base : FREDPP v0.0.16.

## Lot 1 — visibilité `FB` des buffers bootstrap

État : implémenté, à homologuer par build et tests.

### Problème observé

Après le bootstrap, `B(d)` et `B(t)` existaient et étaient utilisables, mais
`FB` ne les affichait pas. `FB` repose sur l'ordre MRU (`recent_names()`),
tandis que les buffers `d`, `t` et `u` étaient initialisés avec
`get_or_create()`, qui ne met volontairement pas à jour cet ordre.

### Correction

Chaque buffer système `d`, `t` et `u` est désormais explicitement sélectionné
après son initialisation afin de l'enregistrer dans l'ordre d'utilisation.

### Test de régression

`cli_bootstrap_facts` lance une vraie procédure contenant `FB` et exige la
présence de `B(d)`, `B(t)` et `B(u)`.

### Lots suivants prévus

- correction historique de `M(buffer)` ;
- `@(label)` et `J(label)T/F` ;
- famille `N` minimale utile aux procédures ;
- délimiteurs historiques pour `JM` / `JP`.

## Contraintes

- aucune modification sous `docs/fr/reference/commandes` ;
- aucune modification de version dans ce lot ;
- PRECHECK complet avant écriture ;
- application atomique avec rollback.

## Lot 2 — `M(buffer)` historique

État : implémenté, à homologuer par build, CTest et test réel.

Le Lot 2 remplace l'ancienne interprétation `M<adresse>` par la syntaxe FRED
historique :

```text
(.,.)M<nom de buffer>
```

Cas couverts :

- `M(buffer)` : ligne courante ;
- `1M(buffer)` : une ligne ;
- `1,2M(buffer)` : plage ;
- remplacement du contenu précédent du buffer destination ;
- suppression des lignes dans le buffer source ;
- buffer destination identique au buffer courant ;
- test CLI réel `cli_move_buffer`.

La commande `T` reste volontairement inchangée dans ce lot.

