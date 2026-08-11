# FREDPP — Livrable Sprint 2.9

## Objet

Implémentation des commandes historiques `G` et `Z`, ajout du moteur de
correspondance des modèles et réparation de l'aide française embarquée.

## Correction historique

La forme inversée de la commande globale est `G~`. La commande `Z` ne constitue
pas une globale inversée : elle positionne la ligne courante sans l'afficher.

## Fonctions livrées

- parsing de `G/<modèle>/<commande>` et `G~/<modèle>/<commande>` ;
- plage par défaut `1,$` pour `G` ;
- commandes imbriquées initiales : `P`, `D` et `Z` ;
- moteur de correspondance pour littéraux, `.`, `^`, `$`, séquences,
  alternatives, répétitions, groupes et classes de caractères ;
- commande `(.)Z` ;
- compteur interne chargé avec le nombre de lignes sélectionnées ;
- `?g` et `?z` à partir des pages françaises existantes ;
- sélection explicite des seules documentations intégrées ;
- aucun changement sous `docs/fr/reference/commandes/` ;
- README sans numéro de version en dur ;
- version consultable avec `?version`.

## Tests

La suite contient désormais 22 tests.

- GCC : 22/22 réussis ;
- Clang : 22/22 réussis, aucun avertissement ;
- MSVC : validation à effectuer sous Windows après intégration.

## Exemple d'homologation

```fred
A
alpha
recherche
beta recherche
omega
\F
1,$G~/recherche$/D
1,$P
```

Résultat attendu :

```text
recherche
beta recherche
```
