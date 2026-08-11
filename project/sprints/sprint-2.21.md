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

## Lot 2 — buffers système du bootstrap

État : implémenté, à homologuer par build et tests.

- `B(d)` : date locale au format historique `MM/DD/YY` ;
- `B(t)` : heure locale au format historique `HH:MM` ;
- `B(u)` : utilisateur courant du système ;
- conservation de `B(0)` même lorsqu'il est vide.

## Lot 3 — `.init` utilisateur et ordre historique

État : implémenté, à homologuer par build et tests.

Référence principale : `BOOTSTRAP - the FRED Bootstrap program`.

Ordre retenu :

1. `B(d)`, `B(t)`, `B(u)` ;
2. `.init.fredpp` utilisateur éventuel ;
3. `B(0)` ;
4. résolution et chargement de la procédure dans `B(.)` ;
5. exécution de `B(.)`.

Adaptation multiplateforme :

```text
Windows : %USERPROFILE%\fredpp\.init.fredpp
Unix    : $HOME/fredpp/.init.fredpp
```

L'absence du fichier par défaut n'est pas une erreur. Une erreur réelle de
lecture ou d'exécution interrompt le bootstrap. `FREDPP_INIT` permet de
surcharger le chemin.

## Contraintes

- références `docs/fr/reference/commandes` inchangées ;
- PRECHECK complet avant écriture ;
- écriture atomique et rollback ;
- aucun changement automatique du numéro de release.
