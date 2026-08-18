# FREDPP v0.0.22 — Application Android native

FREDPP v0.0.22 ajoute Android aux plateformes du projet sans réécrire le moteur :
l'application utilise directement `fredpp_core` C++20 via JNI/NDK.

## Android

L'application fournit une console FREDPP tactile, conserve la session et les buffers
entre les exécutions, et peut charger une procédure texte ou `.fredpp` depuis le
sélecteur de documents Android.

L'interface est réalisée en Kotlin / Jetpack Compose. Le logo complet reste la
référence graphique officielle ; l'icône Android est une déclinaison compacte
dérivée, utilisée uniquement pour les emplacements qui exigent une icône.

## Compatibilité du cœur

Le moteur FREDPP n'est pas dupliqué. Les mêmes composants de parsing, flot, buffers,
commandes et procédures que sur les plateformes desktop sont compilés dans la
bibliothèque native Android.

Le nouveau commutateur CMake `FREDPP_BUILD_CLI` reste activé par défaut ; il est
désactivé uniquement dans le sous-build Android afin de ne produire que la
bibliothèque nécessaire à l'APK.

## Distribution

La release ajoute :

    FREDPP-v0.0.22-android.apk

L'APK est inclus dans `SHA256SUMS.txt`. Les artefacts Windows, Debian, macOS et VS Code
continuent d'être produits par le même workflow.

## Signature

L'APK de cette première intégration est signé avec la configuration de débogage
Android afin d'être directement installable. Cette signature n'est pas une signature
de publication stable : une clé de release dédiée devra être ajoutée avant une
distribution pérenne ou sur le Play Store, sans modifier le moteur FREDPP.
