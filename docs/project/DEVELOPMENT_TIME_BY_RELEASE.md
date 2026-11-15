# FREDPP — Bilan du développement, indexé par numéro de release

> Instantané établi le 15 novembre 2026 sur la branche `dev`, après le commit [`1e0498cf`](https://github.com/PatrickRioche/FREDPP/commit/1e0498cf8a61ce03b60f927410c57a23c96ef911) du 14 novembre 2026. Les dates et heures sont interprétées en fuseau Europe/Paris.

Ce document met en correspondance les **numéros de release**, les **tags techniques** et les travaux visibles dans l'historique GitHub. Il complète les [notes chronologiques](DEVELOPMENT_LOG.md) et le [CHANGELOG](../../CHANGELOG.md).

## 1. Synthèse générale

| Indicateur | Valeur |
|---|---:|
| Première trace de commit | 29 juillet 2026 |
| Dernier commit analysé | 14 novembre 2026 |
| Période calendaire couverte | 109 dates incluses (108 jours d'écart) |
| Commits uniques (branche dev) | **143** |
| Journées avec au moins un commit | **28** |
| Commits juillet / août / novembre | **13 / 66 / 64** |
| GitHub Releases répertoriées | **10** |
| Tags jalons de développement | **8** |
| Commits après v0.1.1 | **2**, hors release |

### Quelle durée de travail retenir ?

**Estimation très prudente de gestion : environ 100 à 180 heures au total, soit un point central indicatif voisin de 140 heures.** Cette fourchette reprend l'hypothèse de 3,5 à 6,5 heures consacrées au projet lors des 28 journées avec commits. **GitHub ne mesure pas les heures réellement travaillées.** Cette hypothèse peut sous-estimer du travail non commité et surestimer des journées à simple publication.

Dans l'historique, les intervalles entre commits rapprochés (écart maximal de deux heures) totalisent environ **43,3 heures** réparties en 50 grappes d'activité. Ils ne constituent pas des heures productives mesurées : il peut y avoir des pauses entre deux commits et du travail avant le premier commit d'une session.

**Important :** il serait trompeur de convertir mécaniquement les commits en heures *par version* ; un commit peut représenter une journée entière ou quelques minutes. Le tableau ci-dessous donne donc les **périodes, jours actifs et volumes de commits vérifiables** par release, plutôt que d'inventer des heures précises.

## 2. Index des versions et jalons

Chaque intervalle correspond aux commits **depuis le tag précédent exclu jusqu'au tag considéré inclus**. Les 143 commits sont ainsi répartis sans doublon, y compris les travaux hors tag après v0.1.1.

| Version | Type | Dates du travail enregistré | Commits | Jours actifs¹ | Diff Git |
|---|---|---|---:|---:|---|
| [`v0.0.9`](#v009) | GitHub Release | 29/07/2026 → 06/08/2026 | 33 | 8 | [Voir les commits](https://github.com/PatrickRioche/FREDPP/commits/v0.0.9/) |
| [`v0.0.15`](#v0015) | GitHub Release | 07/08/2026 → 10/08/2026 | 7 | 4 | [Voir les commits](https://github.com/PatrickRioche/FREDPP/compare/v0.0.9...v0.0.15) |
| [`v0.0.16`](#v0016) | GitHub Release | 10/08/2026 → 11/08/2026 | 7 | 2 | [Voir les commits](https://github.com/PatrickRioche/FREDPP/compare/v0.0.15...v0.0.16) |
| [`v0.0.17`](#v0017) | GitHub Release | 14/08/2026 | 9 | 1 | [Voir les commits](https://github.com/PatrickRioche/FREDPP/compare/v0.0.16...v0.0.17) |
| [`v0.0.18`](#v0018) | GitHub Release | 15/08/2026 | 5 | 1 | [Voir les commits](https://github.com/PatrickRioche/FREDPP/compare/v0.0.17...v0.0.18) |
| [`v0.0.19`](#v0019) | GitHub Release | 16/08/2026 | 6 | 1 | [Voir les commits](https://github.com/PatrickRioche/FREDPP/compare/v0.0.18...v0.0.19) |
| [`v0.0.20`](#v0020) | GitHub Release | 17/08/2026 | 2 | 1 | [Voir les commits](https://github.com/PatrickRioche/FREDPP/compare/v0.0.19...v0.0.20) |
| [`v0.0.21`](#v0021) | GitHub Release | 17/08/2026 | 2 | 1 | [Voir les commits](https://github.com/PatrickRioche/FREDPP/compare/v0.0.20...v0.0.21) |
| [`dev-0.0.30`](#dev-0030) | Jalon `dev` | 18/08/2026 → 09/11/2026 | 43 | 6 | [Voir les commits](https://github.com/PatrickRioche/FREDPP/compare/v0.0.21...dev-0.0.30) |
| [`dev-0.0.40`](#dev-0040) | Jalon `dev` | 09/11/2026 | 3 | 1 | [Voir les commits](https://github.com/PatrickRioche/FREDPP/compare/dev-0.0.30...dev-0.0.40) |
| [`dev-0.0.50`](#dev-0050) | Jalon `dev` | 09/11/2026 | 1 | 1 | [Voir les commits](https://github.com/PatrickRioche/FREDPP/compare/dev-0.0.40...dev-0.0.50) |
| [`dev-0.0.60`](#dev-0060) | Jalon `dev` | 09/11/2026 | 1 | 1 | [Voir les commits](https://github.com/PatrickRioche/FREDPP/compare/dev-0.0.50...dev-0.0.60) |
| [`dev-0.0.70`](#dev-0070) | Jalon `dev` | 10/11/2026 | 1 | 1 | [Voir les commits](https://github.com/PatrickRioche/FREDPP/compare/dev-0.0.60...dev-0.0.70) |
| [`dev-0.0.80`](#dev-0080) | Jalon `dev` | 10/11/2026 | 1 | 1 | [Voir les commits](https://github.com/PatrickRioche/FREDPP/compare/dev-0.0.70...dev-0.0.80) |
| [`dev-0.0.90`](#dev-0090) | Jalon `dev` | 10/11/2026 | 1 | 1 | [Voir les commits](https://github.com/PatrickRioche/FREDPP/compare/dev-0.0.80...dev-0.0.90) |
| [`dev-0.0.103`](#dev-00103) | Jalon `dev` | 10/11/2026 | 1 | 1 | [Voir les commits](https://github.com/PatrickRioche/FREDPP/compare/dev-0.0.90...dev-0.0.103) |
| [`v0.1.0`](#v010) | Préversion GitHub | 10/11/2026 | 1 | 1 | [Voir les commits](https://github.com/PatrickRioche/FREDPP/compare/dev-0.0.103...v0.1.0) |
| [`v0.1.1`](#v011) | GitHub Release | 11/11/2026 → 13/11/2026 | 17 | 3 | [Voir les commits](https://github.com/PatrickRioche/FREDPP/compare/v0.1.0...v0.1.1) |
| Après `v0.1.1` | Hors tag, `dev` | 14/11/2026 | 2 | 1 | [Voir les commits](https://github.com/PatrickRioche/FREDPP/compare/v0.1.1...1e0498cf8a61ce03b60f927410c57a23c96ef911) |

¹ Les jours actifs sont calculés **à l'intérieur de chaque intervalle** et ne peuvent pas être additionnés : plusieurs tags peuvent correspondre à une seule date. Les commits sont, eux, exclusifs d'un intervalle à l'autre.

## 3. Détail par release ou jalon

### v0.0.9

**Publication GitHub** — [Page de la release](https://github.com/PatrickRioche/FREDPP/releases/tag/v0.0.9).

**33 commits** sur **8 date(s) active(s)**, du 29/07/2026 à 09:55 au 06/08/2026 à 21:36.

Fondations C++20, commandes d'édition, buffers, aide française, premiers tests et paquets Windows/Debian.

[Comparer avec le tag précédent](https://github.com/PatrickRioche/FREDPP/commits/v0.0.9/).

### v0.0.15

**Publication GitHub** — [Page de la release](https://github.com/PatrickRioche/FREDPP/releases/tag/v0.0.15).

**7 commits** sur **4 date(s) active(s)**, du 07/08/2026 à 00:11 au 10/08/2026 à 00:25.

Options/buffers, messages JM/JP, procédures, flot S, commande système, capture ZG, ARM64/VS Code.

[Comparer avec le tag précédent](https://github.com/PatrickRioche/FREDPP/compare/v0.0.9...v0.0.15).

### v0.0.16

**Publication GitHub** — [Page de la release](https://github.com/PatrickRioche/FREDPP/releases/tag/v0.0.16).

**7 commits** sur **2 date(s) active(s)**, du 10/08/2026 à 22:14 au 11/08/2026 à 09:54.

Bootstrap des procédures, arguments CLI, buffers système et initialisation utilisateur.

[Comparer avec le tag précédent](https://github.com/PatrickRioche/FREDPP/compare/v0.0.15...v0.0.16).

### v0.0.17

**Publication GitHub** — [Page de la release](https://github.com/PatrickRioche/FREDPP/releases/tag/v0.0.17).

**9 commits** sur **1 date(s) active(s)**, du 14/08/2026 à 00:22 au 14/08/2026 à 21:34.

Sauts de procédure, registres numériques, commandes chaînées, flot, distribution macOS.

[Comparer avec le tag précédent](https://github.com/PatrickRioche/FREDPP/compare/v0.0.16...v0.0.17).

### v0.0.18

**Publication GitHub** — [Page de la release](https://github.com/PatrickRioche/FREDPP/releases/tag/v0.0.18).

**5 commits** sur **1 date(s) active(s)**, du 15/08/2026 à 09:34 au 15/08/2026 à 10:45.

Aide interactive FRED/FREDPP, présentation terminal et nettoyage de l'interface.

[Comparer avec le tag précédent](https://github.com/PatrickRioche/FREDPP/compare/v0.0.17...v0.0.18).

### v0.0.19

**Publication GitHub** — [Page de la release](https://github.com/PatrickRioche/FREDPP/releases/tag/v0.0.19).

**6 commits** sur **1 date(s) active(s)**, du 16/08/2026 à 14:39 au 16/08/2026 à 20:33.

Directives historiques de flot S/L/C/O et interprétation des caractères.

[Comparer avec le tag précédent](https://github.com/PatrickRioche/FREDPP/compare/v0.0.18...v0.0.19).

### v0.0.20

**Publication GitHub** — [Page de la release](https://github.com/PatrickRioche/FREDPP/releases/tag/v0.0.20).

**2 commits** sur **1 date(s) active(s)**, du 17/08/2026 à 00:13 au 17/08/2026 à 00:32.

Sauts arrière J(label) dans les procédures.

[Comparer avec le tag précédent](https://github.com/PatrickRioche/FREDPP/compare/v0.0.19...v0.0.20).

### v0.0.21

**Publication GitHub** — [Page de la release](https://github.com/PatrickRioche/FREDPP/releases/tag/v0.0.21).

**2 commits** sur **1 date(s) active(s)**, du 17/08/2026 à 21:51 au 17/08/2026 à 22:20.

Sémantique historique de lecture R et préparation de la release.

[Comparer avec le tag précédent](https://github.com/PatrickRioche/FREDPP/compare/v0.0.20...v0.0.21).

### dev-0.0.30

**Jalon technique (tag sans GitHub Release)** — [Tag Git](https://github.com/PatrickRioche/FREDPP/tree/dev-0.0.30).

**43 commits** sur **6 date(s) active(s)**, du 18/08/2026 à 15:15 au 09/11/2026 à 13:39.

Android natif, documentation d'architecture, reprise en novembre ; commandes T/K/ZK/FF/F?/F/FN/N: et corrections associées.

[Comparer avec le tag précédent](https://github.com/PatrickRioche/FREDPP/compare/v0.0.21...dev-0.0.30).

### dev-0.0.40

**Jalon technique (tag sans GitHub Release)** — [Tag Git](https://github.com/PatrickRioche/FREDPP/tree/dev-0.0.40).

**3 commits** sur **1 date(s) active(s)**, du 09/11/2026 à 14:48 au 09/11/2026 à 14:57.

Dix opérateurs numériques supplémentaires ; campagne de tests Windows.

[Comparer avec le tag précédent](https://github.com/PatrickRioche/FREDPP/compare/dev-0.0.30...dev-0.0.40).

### dev-0.0.50

**Jalon technique (tag sans GitHub Release)** — [Tag Git](https://github.com/PatrickRioche/FREDPP/tree/dev-0.0.50).

**1 commit** sur **1 date(s) active(s)**, du 09/11/2026 à 16:10 au 09/11/2026 à 16:10.

Dix entrées numériques historiques supplémentaires (41–50).

[Comparer avec le tag précédent](https://github.com/PatrickRioche/FREDPP/compare/dev-0.0.40...dev-0.0.50).

### dev-0.0.60

**Jalon technique (tag sans GitHub Release)** — [Tag Git](https://github.com/PatrickRioche/FREDPP/tree/dev-0.0.60).

**1 commit** sur **1 date(s) active(s)**, du 09/11/2026 à 22:36 au 09/11/2026 à 22:36.

Dix commandes historiques de famille Z (51–60).

[Comparer avec le tag précédent](https://github.com/PatrickRioche/FREDPP/compare/dev-0.0.50...dev-0.0.60).

### dev-0.0.70

**Jalon technique (tag sans GitHub Release)** — [Tag Git](https://github.com/PatrickRioche/FREDPP/tree/dev-0.0.70).

**1 commit** sur **1 date(s) active(s)**, du 10/11/2026 à 09:47 au 10/11/2026 à 09:47.

Jalon d'implémentation des entrées historiques 61–70.

[Comparer avec le tag précédent](https://github.com/PatrickRioche/FREDPP/compare/dev-0.0.60...dev-0.0.70).

### dev-0.0.80

**Jalon technique (tag sans GitHub Release)** — [Tag Git](https://github.com/PatrickRioche/FREDPP/tree/dev-0.0.80).

**1 commit** sur **1 date(s) active(s)**, du 10/11/2026 à 10:52 au 10/11/2026 à 10:52.

Jalon d'implémentation des entrées historiques 71–80.

[Comparer avec le tag précédent](https://github.com/PatrickRioche/FREDPP/compare/dev-0.0.70...dev-0.0.80).

### dev-0.0.90

**Jalon technique (tag sans GitHub Release)** — [Tag Git](https://github.com/PatrickRioche/FREDPP/tree/dev-0.0.90).

**1 commit** sur **1 date(s) active(s)**, du 10/11/2026 à 12:29 au 10/11/2026 à 12:29.

Jalon d'implémentation des entrées historiques 81–90.

[Comparer avec le tag précédent](https://github.com/PatrickRioche/FREDPP/compare/dev-0.0.80...dev-0.0.90).

### dev-0.0.103

**Jalon technique (tag sans GitHub Release)** — [Tag Git](https://github.com/PatrickRioche/FREDPP/tree/dev-0.0.103).

**1 commit** sur **1 date(s) active(s)**, du 10/11/2026 à 13:25 au 10/11/2026 à 13:25.

Treize dernières entrées historiques ; couverture annoncée 103/103, sans émulation GCOS exhaustive.

[Comparer avec le tag précédent](https://github.com/PatrickRioche/FREDPP/compare/dev-0.0.90...dev-0.0.103).

### v0.1.0

**Publication GitHub** — [Page de la release](https://github.com/PatrickRioche/FREDPP/releases/tag/v0.1.0).

**1 commit** sur **1 date(s) active(s)**, du 10/11/2026 à 14:27 au 10/11/2026 à 14:27.

Préversion de distribution contenant le jalon historique 103/103.

[Comparer avec le tag précédent](https://github.com/PatrickRioche/FREDPP/compare/dev-0.0.103...v0.1.0).

### v0.1.1

**Publication GitHub** — [Page de la release](https://github.com/PatrickRioche/FREDPP/releases/tag/v0.1.1).

**17 commits** sur **3 date(s) active(s)**, du 11/11/2026 à 09:11 au 13/11/2026 à 09:33.

Directives de flot N/W/R/E/B, budgets de sécurité, tests de fiabilité 100 et 1 000 fichiers, bibliothèque ECFP et release.

[Comparer avec le tag précédent](https://github.com/PatrickRioche/FREDPP/compare/v0.1.0...v0.1.1).

### Travaux postérieurs à v0.1.1

**2 commits** le 14/11/2026, exclusivement documentaires :

- docs: map 103 FRED commands to C++ implementation and tests ([`576462c2`](https://github.com/PatrickRioche/FREDPP/commit/576462c2166246aaadb1ec4ca0a8b17505abfe39)).
- docs: add command source map to developer index ([`1e0498cf`](https://github.com/PatrickRioche/FREDPP/commit/1e0498cf8a61ce03b60f927410c57a23c96ef911)).

Ces deux commits sont **exclus du périmètre de v0.1.1**, puisqu'ils sont postérieurs au tag.

## 4. Précautions méthodologiques

- Le premier intervalle (`v0.0.9`) comprend toutes les fondations antérieures, dont des mentions de versions internes `v0.0.1` et `v0.0.2-r1`, faute de tags distincts présents dans l'inventaire examiné.
- Un commit intitulé `Release v0.0.22: add native Android application` existe le 18 août, mais **aucun tag `v0.0.22`** n'a été trouvé dans cet inventaire. Il est rattaché à l'intervalle aboutissant à `dev-0.0.30`.
- Le jalon `dev-0.0.30` contient des commits d'août puis de novembre. L'intervalle n'indique **pas une activité continue** entre ces périodes.
- Les 18 tags ont été résolus vers les commits qu'ils référencent ; les tags annotés ne sont donc pas confondus avec les dates de publication de leurs annotations.
- Les commits partagés entre `main` et `dev` ne sont comptés qu'une fois, puisque seul l'historique ascendant de `dev` est utilisé.
- La mention `103/103` désigne la couverture des entrées de référence, **pas** la prise en charge intégrale de toutes les formes historiques GCOS.
- L'estimation globale n'est **ni une feuille de temps, ni une preuve comptable**, et ne prend pas en compte de façon fiable les sessions locales non commitées, discussions, préparations, temps de compilation, travail en branches non fusionnées, ni l'assistance de Codex.
- Le résultat est un **instantané au 14 novembre 2026** ; les futures versions nécessiteront un recalcul.

## 5. Sources

- [Historique GitHub de dev](https://github.com/PatrickRioche/FREDPP/commits/dev/)
- [Releases officielles](https://github.com/PatrickRioche/FREDPP/releases)
- [Journal chronologique](DEVELOPMENT_LOG.md)
- [Suivi des 103 commandes](COMMAND_STATUS.md)
- [Organisation des releases](RELEASES.md)
- [Carte du code source des commandes](../developer/COMMAND-SOURCE-MAP.md)
