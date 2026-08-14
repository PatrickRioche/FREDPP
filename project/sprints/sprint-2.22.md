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


## Lot 2.1 — `\S(buffer)` dans les commandes système

Le cas réel `zg(list)!dir \S(chemin) /B /S` est désormais pris en charge.
`ZG` bénéficie du correctif via son `SystemCommand` imbriqué.

Le traitement est ciblé sur `\S(...)` pour ne pas interpréter les backslashes
de chemins Windows comme des directives FRED.

Tests : extension de `test_system_zg` et ajout de `cli_flow_s_system`.


## Lot 2.2 — récupération interactive après erreur de procédure

État : implémenté, à homologuer.

Une erreur pendant une procédure principale ne termine plus immédiatement
FREDPP. Le moteur :

- arrête la procédure ;
- affiche au maximum trois lignes à partir du point d'erreur ;
- affiche `...` si d'autres lignes restent ;
- conserve les buffers et l'état courant ;
- revient dans la boucle interactive pour permettre le débogage.

Les erreurs de bootstrap, `.init` ou résolution de procédure restent distinctes
et interrompent toujours le lancement.

Test ajouté : `cli_procedure_debug`.


## Lot 2.3 — `\S(buffer)` dans les modèles `G`

État : implémenté, à homologuer.

Ce lot ajoute le cas de procédure :

```fred
g~/\S(model)/d
```

Les directives `\S(...)` sont développées de manière ciblée avant le parsing
d'une commande `G`. Les autres familles de commandes ne sont pas modifiées.

Test ajouté : `cli_flow_s_global`.


### Lot 2.3 v3 — procédure et interactif

Après validation du cas procédure, un test manuel a montré que la boucle
interactive contournait l'expansion ciblée de `\S(...)`.

Le helper est désormais factorisé dans
`fred/flow/CommandInputExpansion.hpp` et utilisé par :

- `ProcedureRunner` ;
- la branche interactive réelle du CLI avant `fred::Lexer`.

Test ajouté : `cli_flow_s_global_interactive`.


## Lot 3 — `@(label)` et `J(label)[T|F]`

État : implémenté, à homologuer.

Sous-ensemble historique ajouté au moteur de procédures :

- `@(label)` ;
- `J(label)` ;
- `J(label)T` ;
- `J(label)F`.

Les labels sont locaux au buffer exécuté, limités à 15 caractères et comparés
sans tenir compte de la casse. La recherche est avant uniquement dans ce lot :
première étiquette correspondante après la ligne `J`, sans wrap.

Tests ajoutés :

- `test_procedure_jump` ;
- `cli_jump_label`.

Le lot ne modifie aucun fichier sous `docs/fr/reference/commandes`.


## Lot 4 — `N(np):$>0` minimal

État : implémenté, à homologuer.

Le programme principal est chargé dans `B(.)` et exécuté avec `B(0)` comme
buffer courant au lancement.

Sous-ensemble :

- `N(reg):valeur`
- `N(reg)=valeur`
- `N(reg)<valeur`
- `N(reg)>valeur`

Opérandes : entier signé, `$`, `.`, `#`.

Cas cible :

```fred
N(np):$>0 J(param)T
```

Tests ajoutés : `test_procedure_numeric` et `cli_numeric_jump`.
`test_runtime_context` est étendu.

