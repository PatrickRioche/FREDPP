# FREDPP v0.1.1 — Préversion multi-plateforme

Cette préversion consolide l'éditeur FREDPP C++20 depuis v0.1.0. Elle est
préparée depuis la branche `dev` et ne promeut pas le code vers `main`.

## Évolutions depuis v0.1.0

- Directives du flot : améliorations de `\B`, `\E`, `\N` et `\R`, avec
  maintien des limitations historiques documentées.
- Exécution des procédures : corrections fonctionnelles, protections contre
  les répétitions sans borne et contrôles de récupération après erreur.
- Suite de non-régression portée de **71 à 79 tests CTest**. La candidate
  v0.1.1 a validé **79/79 tests en CI Windows/MSVC et Linux/GCC**.
- Tests de lots : traitement de **100 et 1 000 fichiers**, avec vérification
  des données, transformations successives, intégrité des fichiers sources
  et gestion d'une source manquante.
- Nouvelle procédure générale **`ecfp.fredpp`**, fournie dans `library/` :
  applique un programme de commandes FREDPP aux fichiers énumérés dans une
  liste. `VOIR` affiche les transformations sans écriture automatique d'ECFP ;
  `FAIRE` écrit les transformations sur les originaux.
- Les procédures `library/*.fredpp` sont incluses automatiquement dans les
  archives portables Windows, Debian et macOS, ainsi que dans les paquets
  natifs Debian.
- La CI Android construit à nouveau un APK Debug ; **Android n'est pas
  inclus dans la présente release desktop**.

## Téléchargements

La chaîne de publication construit les neuf fichiers suivants :

- `FREDPP-v0.1.1-windows-x64.zip`
- `FREDPP-v0.1.1-debian13-amd64.tar.gz`
- `FREDPP-v0.1.1-debian13-arm64.tar.gz`
- `FREDPP-v0.1.1-macos-x64.tar.gz`
- `FREDPP-v0.1.1-macos-arm64.tar.gz`
- `fredpp_0.1.1_amd64.deb`
- `fredpp_0.1.1_arm64.deb`
- `FREDPP-Language-v0.1.1.vsix`
- `SHA256SUMS.txt`

Le workflow vérifie chaque package et ses empreintes SHA-256 avant
publication. GitHub fournit en plus les deux archives du code source.

## Essayer ECFP

Placez un nom de fichier par ligne dans `liste.txt` et un programme de
commandes FREDPP dans `transformations.fredpp`. Depuis un kit décompressé :

```text
fredpp ecfp liste.txt transformations.fredpp VOIR
fredpp ecfp liste.txt transformations.fredpp FAIRE
```

**Sauvegardez les fichiers avant FAIRE.** Le mode VOIR ne garantit pas
l'absence d'effets secondaires du programme fourni : les commandes externes
ou autonomes d'écriture ne sont pas isolées. N'exécutez que des scripts fiables.

## Limites connues

- Les **103 entrées** du tableau de couverture sont reconnues ; cela ne
  garantit pas une émulation complète de FRED sous GCOS.
- Certaines formes avancées des directives, dont `\B` et `\E`, restent
  incomplètes ou non homologuées.
- Les délais de sécurité sont contrôlés entre les instructions, pas pendant
  l'exécution d'une commande bloquante. Aucun plafond global de mémoire des
  buffers n'est actuellement appliqué.
- La résistance aux coupures pendant une écriture et aux volumes
  exceptionnellement importants n'est pas entièrement validée.
- L'exécutable Windows est non signé ; SmartScreen peut afficher un
  avertissement. Ne désactivez pas les protections Windows pour le lancer.

La présente version est une **préversion** destinée aux tests et à
l'expérimentation. L'APK Android suit une diffusion distincte.
