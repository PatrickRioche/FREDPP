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
| Version du projet | `0.0.22` (développement) |
| Branche active | `dev` |
| Dernier commit du lot K validé | [`d723877`](https://github.com/PatrickRioche/FREDPP/commit/d723877a4f0e545cd506fc7bfe6704e3520e493c) |
| Nouveau lot | `ZK` implémenté ; CI Windows 12/12 réussie, validation locale en attente |
| Branche stable | `main`, non modifiée par ces travaux |
| Écart de `dev` sur `main` | Évolutif ; consulter `git log main..dev` |
| Dernière CI Windows/MSVC ciblée | [10/10 tests réussis](https://github.com/PatrickRioche/FREDPP/actions/runs/42417481573) |
| Validation locale Windows 11 — T | Confirmée par l'utilisateur (scénario et CTest 1/1) |
| Validation locale Windows 11 — K | Confirmée par l'utilisateur (sortie exacte et CTest 2/2) |
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

## Détails des lots et des validations

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
- **Validation locale Windows 11 :** en attente du retour utilisateur.
- **Commit du lot :** [`c1f0fc0`](https://github.com/PatrickRioche/FREDPP/commit/c1f0fc03ea1af89441e9ace002ff949ff17301a4).
- **Statut :** candidat en `dev`, version `0.0.22` inchangée ;
  pas de fusion `main`, de tag stable ni de release.

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
