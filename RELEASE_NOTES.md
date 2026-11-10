# FREDPP v0.1.0 — Préversion de référence

Cette première préversion rassemble les travaux effectués sur la branche `dev`
jusqu'au jalon `dev-0.0.103`. Elle préserve l'esprit de l'éditeur en ligne FRED
et propose un moteur C++20 portable avec un langage de manipulation de texte.

## Fonctionnalités

- Les **103 entrées** du tableau de référence historique sont reconnues et
  prises en charge, avec les adaptations et limites indiquées dans le
  [tableau de couverture](https://github.com/PatrickRioche/FREDPP/blob/v0.1.0/docs/project/COMMAND_STATUS.md) et les guides français.
- Buffers multiples, adresses de lignes, insertion, suppression, copie,
  déplacement, recherche et substitution, fichiers et commandes de consultation.
- Registres numériques, options de saisie, exécution de procédures, conditions,
  boucles contrôlées, modèles nommés, annulation et rétablissement.
- Aide embarquée, version traçable par `FV`, et extension de langage pour
  Visual Studio Code.

**Important :** 103/103 correspond à la couverture des *entrées répertoriées*,
pas à une équivalence fonctionnelle complète avec FRED sous GCOS. Certains cas
historiques, les directives avancées et les limites de charge restent à valider.

## Vérification disponible au jalon

La version de développement `0.0.103` a passé **71 tests CTest sur 71** sous
Windows/MSVC (91,26 s), ainsi qu'une recette interactive ciblée. Le pipeline de
publication compile et teste à nouveau ses plateformes avant de produire les
archives. La campagne renforcée sur les boucles infinies, délais d'exécution,
mémoire et taille des buffers est prévue **après cette préversion** ; elle
n'est pas présentée comme déjà réussie.

## Téléchargements

La release contient neuf fichiers fournis par le workflow :

- `FREDPP-Language-v0.1.0.vsix`
- `FREDPP-v0.1.0-debian13-amd64.tar.gz`
- `FREDPP-v0.1.0-debian13-arm64.tar.gz`
- `FREDPP-v0.1.0-macos-arm64.tar.gz`
- `FREDPP-v0.1.0-macos-x64.tar.gz`
- `FREDPP-v0.1.0-windows-x64.zip`
- `fredpp_0.1.0_amd64.deb`
- `fredpp_0.1.0_arm64.deb`
- `SHA256SUMS.txt`

GitHub ajoute automatiquement les deux archives du code source (`zip` et
`tar.gz`), soit **11 assets affichés**. Les empreintes SHA-256 couvrent les
huit packages téléchargeables créés par le workflow.

Les fichiers de cette release sont compilés depuis le tag `v0.1.0` sur `dev`,
**sans modifier `main`**. L'application Android suit son propre circuit CI et
n'est pas incluse dans cette matrice de livraison.
