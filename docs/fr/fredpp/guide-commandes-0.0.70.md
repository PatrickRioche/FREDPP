# FREDPP 0.0.70 — Guide de recette du lot 61–70

Ce lot ajoute **10 commandes historiques** aux 60 commandes de `0.0.60` :
`FV`, `JE`, `ZR`, `ZW`, `OQ`, `OS&`, `OSD`, `OT`, `ZI` et `ZO`.

La documentation `docs/fr/reference/commandes/` est conservée à l'identique.
La présente fiche précise les formes réellement disponibles sous FREDPP.

| Commande | Forme validable | Effet dans FREDPP |
|---|---|---|
| `FV` | `FV` | Date de compilation, version et commit au lieu du file-code GCOS historique |
| `JE` | `JE/Erreur/` ou `JE Erreur` | Imprime une erreur utilisateur ; déclenche une demande d'arrêt sous `O+Q` |
| `ZR` | `ZR fichier.txt` | Réutilise un buffer déjà associé à ce chemin, ou lit le fichier dans `b(a)` puis `b(b)`, etc. |
| `ZW` | `1,$ZW fichier.txt`, `ZWA`, `ZWU` | Ajoute les lignes au fichier, contrôle le codage, et ne marque pas le buffer comme sauvegardé |
| `OQ` | `O+Q` / `O-Q` | Arrêt sur incident dans l'éditeur ou une procédure ; désactivé par défaut |
| `OS&` | `O+S&` / `O-S&` | Active ou désactive le remplacement de `&` par la correspondance trouvée dans `S` |
| `OSD` | `O+SD` / `O-SD` | Recherche ASCII insensible / sensible à la casse dans `S`, `T`, `G`, `ZL` et `ZU` |
| `OT` | `O+T10,20,30` / `O-T` | Définit les colonnes de tabulation ; taquets par défaut 5 et 9 |
| `ZI` | `1,$ZI` | Compresse les suites d'espaces alignées en tabulations |
| `ZO` | `1,$ZO` | Développe les tabulations en espaces selon les taquets configurés |

## Scénario interactif de vérification

```fred
FV
FO
B(essai)
A
Alpha
Bravo
\F
O+SD
1T/alpha/
O-SD
1T/alpha/
O-S&
1S/Alpha/& &/P
O+S&
O+T10,20
1,$ZI
1,$ZO
JE/Erreur volontaire sans arrêt/
FO
```

Pour tester `ZR`/`ZW`, créer préalablement un petit fichier texte de test
dans le répertoire temporaire Windows (avec une sauvegarde de son contenu),
puis vérifier qu'une seconde exécution de `ZR` sélectionne le même buffer et
ne supprime pas les modifications en mémoire. `ZW` doit préserver le contenu
préexistant du fichier et y ajouter les lignes sélectionnées.

## Comportements à homologuer historiquement

- `FV` adapte le file-code GCOS à la date de compilation et au commit FREDPP.
- `ZR` utilise les noms automatiques `a` à `z` et compare les chemins textuels ;
  l'identité de chemins relatifs, absolus, liens symboliques ou avec casse
  différente n'est pas normalisée.
- `ZW` ajoute un saut de ligne si un fichier non vide se termine sans séparateur.
  Les variantes BCD restent indisponibles, comme pour `W`.
- `OSD` ne convertit que les caractères ASCII ; le comportement par défaut
  reste sensible à la casse dans cette version pour éviter une rupture avec
  les expressions existantes de FREDPP.
- Les taquets `OT` sont appliqués aux conversions `ZI`/`ZO`; le rendu interactif
  des tabulations par tous les types de terminal n'est pas encore homologué.
- `O+Q` ne doit pas être activé pour une recette interactive contenant des
  commandes volontairement invalides : une erreur demande l'arrêt du programme.

## Validation et publication

Exécuter la construction et toute la suite CTest sur Windows/MSVC (Debug et
Release), ainsi que le scénario ci-dessus. Le tag `dev-0.0.70` n'est créé
qu'après cette validation explicite de l'utilisateur. Aucun merge vers `main`
ni publication stable ne fait partie de ce lot.
