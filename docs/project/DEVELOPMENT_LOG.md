# Journal chronologique de développement — FREDPP

Ce journal suit les **travaux sur la branche `dev`** avant leur intégration
éventuelle dans `main`. Il complète, sans les remplacer :

- [`CHANGELOG.md`](../../CHANGELOG.md) : synthèse fonctionnelle par version ;
- [`ROADMAP.md`](../../ROADMAP.md) : objectifs et jalons à venir ;
- [l'historique Git de `dev`](https://github.com/PatrickRioche/FREDPP/commits/dev/) :
  source de vérité des dates, auteurs et changements de fichiers.

Les heures ci-dessous sont en **heure de Paris (CET)**. Les commits Git
conservent leurs horodatages originaux en UTC.

## État mis à jour le 8 novembre 2026

| Élément | État vérifié |
|---|---|
| Version du projet | `0.0.24` (développement ; 24/103 commandes) |
| Branche active | `dev` |
| Dernier commit du lot K validé | [`d723877`](https://github.com/PatrickRioche/FREDPP/commit/d723877a4f0e545cd506fc7bfe6704e3520e493c) |
| Nouveau lot | `ZK` implémenté ; CI Windows Release 12/12 et validation locale Windows 11 Debug 2/2 réussies |
| Branche stable | `main`, non modifiée par ces travaux |
| Écart de `dev` sur `main` | Évolutif ; consulter `git log main..dev` |
| Dernière CI Windows/MSVC ciblée | [16/16 tests Release, 3 × 2/2 Debug et 1/1 R réussis](https://github.com/PatrickRioche/FREDPP/actions/runs/42468744288) |
| Validation locale Windows 11 — T | Confirmée par l'utilisateur (scénario et CTest 1/1) |
| Validation locale Windows 11 — K | Confirmée par l'utilisateur (sortie exacte et CTest 2/2) |
| Validation locale Windows 11 — ZK | Confirmée (scénario Debug, CTest 2/2 et relance via `test.ps1` 2/2 en 0,55 s) |
| Validation locale Windows 11 — R (liste de fichiers) | Confirmée par l'utilisateur : `test_file_io` 1/1, CTest total 1,53 s |
| Homologation historique complète | Reportée |
| Nouvelle release officielle | Aucune ; pas de fusion dans `main` |

## Chronologie des commits (ordre chronologique)

| Date et heure (CET) | Commit | Modification principale |
|---|---|---|
| 07/11/2026 14:12 | [`5ad4960`](https://github.com/PatrickRioche/FREDPP/commit/5ad4960700ec8dbd6a7ac62076e362f35566fd1b) | Sécurisation des adresses relatives ; restauration du moteur Flow après erreur |
| 07/11/2026 14:25 | [`f287f31`](https://github.com/PatrickRioche/FREDPP/commit/f287f3112bed55ec83eb51443f8fc11a57692e67) | Politique des branches `dev` / `main` et des releases |
| 07/11/2026 14:37 | [`4aceb8b`](https://github.com/PatrickRioche/FREDPP/commit/4aceb8bbbf9b2934db2f3ed19ac6411fc7c65cf9) | Rétablissement du `T` historique (test de modèle, condition, `T~`) |
| 07/11/2026 22:30 | [`33e9275`](https://github.com/PatrickRioche/FREDPP/commit/33e927583d5ccf87f9a4cd680cd15582542890ce) | Test visible `T` + sauts conditionnels dans une procédure |
| 07/11/2026 22:43 | [`cf4b811`](https://github.com/PatrickRioche/FREDPP/commit/cf4b811e934e2243363fdc0e20c14e8efb152f08) | Implémentation candidate de `K`, tests et documentation |
| 07/11/2026 22:46 | [`d723877`](https://github.com/PatrickRioche/FREDPP/commit/d723877a4f0e545cd506fc7bfe6704e3520e493c) | Correctif du répartiteur d'exécution pour `K` |
| 08/11/2026 01:07 | [`c1f0fc0`](https://github.com/PatrickRioche/FREDPP/commit/c1f0fc03ea1af89441e9ace002ff949ff17301a4) | Implémentation candidate `ZK`, tests et documentation |
| 08/11/2026 01:10 | [`58308e7`](https://github.com/PatrickRioche/FREDPP/commit/58308e7144d728729d90f42a81d24af5459bc6d9) | Résultat CI Windows 12/12 consigné |
| 08/11/2026 01:14 | [`2b9bfd7`](https://github.com/PatrickRioche/FREDPP/commit/2b9bfd781443d9e8d1e3ba8b3c8ee3aa5882831e) | Anomalie MSVC Debug `LNK1163` rapportée |
| 08/11/2026 01:18 | [`ef78cf7`](https://github.com/PatrickRioche/FREDPP/commit/ef78cf7a0536cf232751beb92582236d6287b7c6) | Validation locale de `ZK` 2/2 et procédure de compilation isolée |
| 08/11/2026 07:37 | [`7fad17a`](https://github.com/PatrickRioche/FREDPP/commit/7fad17a31b8410df577e507c0c44c8b8d30b4867) | Version `0.0.24` synchronisée et scripts Windows externes |
| 08/11/2026 | [`cd6e07c`](https://github.com/PatrickRioche/FREDPP/commit/cd6e07c7506e2f43d4a24e171a3a1c17a1569f69) | Premier correctif de `test.bat` ; CI PowerShell échouée (voir bilan) |
| 08/11/2026 08:00 | [`e56bb70`](https://github.com/PatrickRioche/FREDPP/commit/e56bb70e04e8e76cbb8887943ef8e0cd3441a8d1) | `test.ps1` natif ; appels PowerShell et batch testés, CI Windows verte |
| 08/11/2026 10:48 | [`05585a3`](https://github.com/PatrickRioche/FREDPP/commit/05585a32adf5ea067f520c1a3ed006e9b913ac96) | `R` multi-candidats, tests de repli et documentation Windows |

## Détails des lots et des validations

### Lot R — recherche ordonnée de fichiers (8 novembre 2026)

- **Référence :** `docs/fr/reference/commandes/r.md` demande de parcourir
  une liste de fichiers jusqu'au premier lisible.
- **Réalisation sur `dev` :** lecture prioritaire du nom complet (compatibilité
  des chemins Windows contenant des espaces), puis essai ordonné des candidats
  séparés par espaces. `R` complet associe le fichier réellement ouvert ;
  `R` adressé insère sans modifier l'association.
- **Sécurité :** aucun changement de buffer si tous les candidats échouent.
  Les erreurs de fichier/décodage permettent de poursuivre la recherche.
- **Tests :** `test_file_io` étendu (succès sur le deuxième candidat, insertion,
  échec total sans mutation) ; test ajouté au ciblage CI Release et Debug.
- **Limite :** les noms contenant des espaces au sein d'une liste ne sont pas
  encore gérés ; le support `WX` attend une définition fidèle du comptage
  historique des blocs.
- **Version :** `0.0.24` inchangée : amélioration de `R`, déjà comptabilisé.
- **Validation CI Windows :** [exécution 42468744288](https://github.com/PatrickRioche/FREDPP/actions/runs/42468744288) réussie : compilation MSVC Release et **16/16 tests**, compilation Debug, trois passages ciblés **2/2**, puis **1/1** pour `test_file_io` via PowerShell ; toutes les étapes vertes.
- **Validation locale Windows 11 :** **confirmée le 8 novembre 2026** par l'utilisateur sous PowerShell, dans le build Debug persistant `%LOCALAPPDATA%\\FREDPP\\build\\x64-Debug` : `./scripts/test.ps1 -R "^test_file_io$"` réussit **1/1**, `test_file_io` en **0,90 s**, CTest total **1,53 s**. Aucun échec.

### Correctif test.bat — arguments PowerShell (8 novembre 2026)

- **Retour utilisateur :** après un build de `0.0.24` réussi (commit
  `b1f7def`), l'appel PowerShell
  `scripts/test.bat -R "^(test_zap_copy|cli_zap_copy)$"` échoue avant CTest :
  `'cli_zap_copy)$' n’est pas reconnu`. Le shell `cmd.exe` interprète le
  `|` de l'expression lorsque `%*` est injecté sans réencadrement.
- **Premier correctif insuffisant :** `test.bat` transmet désormais
  `-R` par `"%~2"`, sans `%*`. La [CI Windows
  42456579501](https://github.com/PatrickRioche/FREDPP/actions/runs/42456579501)
  confirme cependant que le `|` est interprété par `cmd.exe` **avant**
  le démarrage du batch depuis PowerShell. Les validations Release 15/15
  et Debug 2/2 ont réussi, mais l'étape PowerShell a échoué.
- **Correction ciblée :** `scripts/test.ps1` appelle CTest directement,
  sans passer par `cmd.exe`. La CI vérifie ce scénario exact, ainsi que
  l'appel batch avec un filtre simple `-R zap_copy`.
- **Documentation :** exemples PowerShell `K`, `ZK`, `T` et guide
  développeur corrigés ; usages batch et PowerShell distingués.
- **Version :** inchangée à `0.0.24` (aucune nouvelle commande FRED).
- **Validation automatique :** [CI Windows 42456990368](https://github.com/PatrickRioche/FREDPP/actions/runs/42456990368) **réussie** : compilation complète Release, 15/15 tests ciblés, compilation Debug, 2/2 tests depuis `cmd.exe`, 2/2 via `test.ps1` depuis PowerShell avec alternance, et 2/2 via `test.bat -R zap_copy` depuis PowerShell.
- **Validation locale Windows 11 :** **confirmée par l'utilisateur le 8 novembre 2026** après `git pull --ff-only origin dev` (déjà à jour) : `./scripts/test.ps1 -R "^(test_zap_copy|cli_zap_copy)$"` exécute `test_zap_copy` et `cli_zap_copy` depuis `%LOCALAPPDATA%\FREDPP\build\x64-Debug`. Résultat **2/2 réussis**, aucune défaillance, **0,55 s**. L'incident de citation des arguments PowerShell est clos.

### Lot numérotation dev et scripts Windows (8 novembre 2026)

- **Décision utilisateur :** aligner le dernier nombre de la version de
  développement sur les commandes historiques prises en charge : 24 =
  `0.0.24` ; commande supplémentaire comptabilisée = `0.0.25`.
- **Garantie CMake :** le nombre `N` de `0.0.N` est comparé au total du tableau
  `docs/project/COMMAND_STATUS.md` pendant la configuration ; un décalage
  empêche la génération du projet.
- **Scripts Windows :** `build.bat`, `test.bat` et `rebuild.bat` utilisent
  désormais un même dossier Debug persistant dans `%LOCALAPPDATA%`, avec
  `FREDPP_BUILD_DIR` configurable ; les wrappers PowerShell suivent cette
  logique. La compilation reste sérialisée, les sources restent dans Google
  Drive et `rebuild.bat` évite la suppression globale des dossiers de sortie.
- **Validation automatique :** [CI Windows/MSVC 42455541228](https://github.com/PatrickRioche/FREDPP/actions/runs/42455541228) réussie : compilation Release et **15/15 tests ciblés** (dont `cli_smoke`, `test_version` et `test_release_manifest`) ; compilation Debug intégrale via `scripts/build.bat` et **2/2 tests** via `scripts/test.bat` dans un répertoire externe au dépôt. La garde CMake de version a passé la configuration.
- **Validation locale Windows 11 :** en attente du premier lancement du nouveau `scripts/build.bat` / `scripts/test.bat` par l'utilisateur ; l'ancienne compilation Debug manuelle isolée de `ZK` reste validée.
- **Gouvernance :** `dev` uniquement ; pas de changement de `main`, de tag
  officiel ni de release.


### Lot ZK — copie avec insertion (8 novembre 2026)

- **Objectif :** `ZK(destination)` conserve le contenu du destinataire et
  insère les lignes source après sa ligne courante, contrairement à `K`.
- **Code :** nouveau type AST `ZapCopyCommandNode`, reconnaissance de
  `ZK` dans le parseur, dispatch et exécution avec snapshot des lignes ;
  protection du recouvrement dans les copies sur un même buffer.
- **Tests :** `test_zap_copy`, `cli_zap_copy`, couverture du parseur et
  extension de la CI Windows ciblée à 12 tests.
- **Documentation :** `docs/specs/SPEC-029-ZAP-COPY.md`,
  `docs/developer/TESTING-ZK-COPY.md`, documentation Runtime et `?zk`.
  Le manuel historique français n'a pas été modifié.
- **Validation CI Windows/MSVC :** [exécution 42428354349](https://github.com/PatrickRioche/FREDPP/actions/runs/42428354349), compilation réussie et **12/12 tests passés** (0,29 s pour CTest ciblé), dont `test_zap_copy` et `cli_zap_copy`.
- **Validation locale Windows 11 :** confirmée par l'utilisateur le 8 novembre. Après un premier échec `LNK1163` dans l'ancien répertoire de build synchronisé, configuration propre dans `%TEMP%` : compilation MSVC Debug réussie, sortie complète de `cli_zap_copy.fredpp` strictement conforme, puis `test_zap_copy` et `cli_zap_copy` réussis **2/2** (5,42 s). Avertissement MSBuild `MSB8029` sans effet bloquant, car le nouveau répertoire se trouve dans `%TEMP%`.
- **Commit du lot :** [`c1f0fc0`](https://github.com/PatrickRioche/FREDPP/commit/c1f0fc03ea1af89441e9ace002ff949ff17301a4).
- **Statut :** candidat en `dev`, version `0.0.22` inchangée ;
  pas de fusion `main`, de tag stable ni de release.

### Anomalie locale de compilation Debug — 8 novembre 2026

L'utilisateur a transmis le journal de `scripts/build.bat` après `git pull
--ff-only origin dev` : configuration Visual Studio 18 2026 réussie, puis
plusieurs échecs d'édition de liens `LNK1163` (« sélection non valide pour
la section COMDAT »). Le binaire Debug n'existe pas après cette compilation.
Une seconde tentative, avec les mêmes sources dans Google Drive mais un
**nouveau répertoire de build hors de Google Drive** (`%TEMP%`), a réussi en
MSVC Debug. `fredpp.exe` et `test_zap_copy.exe` ont été produits ; le scénario
CLI affichait exactement les douze lignes attendues et CTest a confirmé
**2/2 tests réussis en 5,42 s**. La compilation Windows/MSVC **Release** en
CI avait déjà réussi ses 12 tests.

**Diagnostic :** l'erreur n'est pas reproductible avec un répertoire de build
neuf et distinct. Cela oriente vers un problème de cache/objets ou de dossier
intermédiaire dans la première tentative, mais ne permet pas d'isoler la
cause précise. Le compilateur MSVC a émis `MSB8029` dans `%TEMP%` : MSBuild
déconseille ce dossier pour les sorties intermédiaires à cause des compilations
incrémentales. Pour les prochains builds réguliers, privilégier un dossier
persistant hors de Google Drive **et hors de `%TEMP%`** (par exemple
`C:\dev\FREDPP-build-debug`), sans déplacer le dépôt source. Aucun nettoyage
des journaux Android non suivis n'est requis.

### Lot K — copie de lignes vers un buffer (7 novembre 2026)

- **Objectif :** `K(destination)` copie une ligne ou plage vers un buffer
  destinataire dont le contenu précédent est remplacé. Les lignes source
  restent présentes, contrairement à `M`.
- **Code :** ajout de l'AST, de la reconnaissance de `K` dans le parseur et de
  l'exécution dans le Runtime.
- **Tests :** `test_copy` (unitaire) et `cli_copy` (procédure affichable).
- **Documentation :** `docs/specs/SPEC-028-COPY.md`,
  `docs/developer/TESTING-K-COPY.md`, aide française `?k` embarquée.
- **Traçabilité des anomalies :** la [première CI](https://github.com/PatrickRioche/FREDPP/actions/runs/42417244166)
  a échoué avec « unsupported command node » ; le commit `d723877`
  a raccordé `CopyCommand` au répartiteur Runtime.
- **Validation automatique finale :** [Windows/MSVC, 10/10 tests](https://github.com/PatrickRioche/FREDPP/actions/runs/42417481573),
  dont `test_copy` et `cli_copy`, réussis.
- **Validation utilisateur Windows 11 :** confirmée le 8 novembre 2026 :
  transcription de `cli_copy.fredpp` conforme au résultat attendu ;
  `test_copy` et `cli_copy` réussis (2/2, durée totale 0,66 seconde).
- **Statut :** scénario Windows 11 validé localement et en CI ;
  homologation historique complète reportée.

### Lot T — test de modèle et embranchements (7 novembre 2026)

- **Objectif :** rétablir le sens historique de `T` et `T~`, qui modifient
  seulement le registre de condition ; ils n'affichent rien et ne déplacent
  aucune ligne.
- **Tests :** `test_test_command`, puis `cli_test_condition_t` avec
  `J(label)T/F` et `JM` pour rendre le résultat visible.
- **Documentation :** `docs/specs/SPEC-020-TEST.md`,
  `docs/developer/TESTING-T-CONDITION.md` et commentaires des sources C++.
- **Validation automatique :** [CI du correctif T](https://github.com/PatrickRioche/FREDPP/actions/runs/42377787267),
  puis [CI du test visible](https://github.com/PatrickRioche/FREDPP/actions/runs/42416161675),
  réussies.
- **Validation utilisateur Windows 11 :** sortie exacte du scénario
  `cli_test_condition_t.fredpp` confirmée ; `ctest` rapporte
  « 100% tests passed out of 1 ».
- **Statut :** scénario de développement validé localement et en CI ;
  homologation historique complète reportée.

### Fondation et gouvernance de développement (7 novembre 2026)

- **Robustesse :** garde contre le dépassement arithmétique des adresses
  relatives ; nettoyage des sources actives du FlowEngine après erreur.
- **Branches :** `dev` pour les modifications et tests ; `main` pour les
  versions approuvées. Les tags intermédiaires sont facultatifs.
- **Publication :** aucune fusion ni release automatique depuis `dev`.
  Une version officielle suppose validation, fusion approuvée et tag stable.

## Retour de validation locale — 8 novembre 2026

L'utilisateur a transmis la sortie PowerShell de son poste Windows 11,
depuis le dépôt FREDPP situé dans Google Drive :

- build Debug de FREDPP `0.0.22`, commit `706253b` : compilation terminée ;
- exécution de `tests/fixtures/cli_copy.fredpp` : sortie strictement conforme
  au scénario documenté dans `docs/developer/TESTING-K-COPY.md` ;
- `ctest -R "^(test_copy|cli_copy)$"` : **2 tests sur 2 réussis**
  (`test_copy` : 0,60 s ; `cli_copy` : 0,05 s ; total : 0,66 s).

Cette validation locale est rapportée par l'utilisateur et complète la CI
Windows [10/10 réussie](https://github.com/PatrickRioche/FREDPP/actions/runs/42417481573).
Elle n'implique ni fusion dans `main` ni publication officielle.

## Règle de mise à jour pour les prochains lots

Pour chaque lot significatif sur `dev`, ajouter une entrée datée précisant :

1. le périmètre et les différences de comportement ;
2. les fichiers sources, tests et documentations concernés ;
3. le ou les commits Git ;
4. les résultats CI (avec lien) et la validation locale Windows 11,
   **distingués explicitement** ;
5. les erreurs rencontrées et leur correction ;
6. les limites non couvertes, la décision de validation et la situation de
   `main` / de la release.

Le journal est maintenu en complément des commentaires C++ en anglais et de
la documentation technique de `docs/developer/`. Les informations reprises
de Git restent vérifiables par commit ; une validation locale non automatisée
est toujours présentée comme rapportée par l'utilisateur.
