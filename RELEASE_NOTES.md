# FREDPP v0.1.1 — Version candidate Windows x64 (essais utilisateur)

Cette version candidate sert à tester sur Windows le moteur FREDPP C++20 après
les travaux de fiabilisation menés depuis la préversion v0.1.0.

**Statut : version candidate, non encore publiée comme GitHub Release officielle.**
Le ZIP de test est produit par GitHub Actions pour la pull request de
préparation, sans tag v0.1.1 et sans modification de la branche main.

## Évolutions depuis v0.1.0

- Directives de flot : \R (saisie terminal littérale), \N (format du
  registre NB/ND/NF), \E (références nommées uniquement dans les motifs,
  définitions imbriquées), \B (flot actif de commandes et saisie A/I/C).
  Les autres directives \C, \L, \O, \S et \W restent couvertes.
- Exécution des procédures et commandes : corrections de compatibilité
  historique et protections contre les cas non bornés.
- Non-régression : passage de 71 à **78 tests CTest** depuis v0.1.0,
  validés en CI Windows/MSVC et Linux/GCC, puis localement sous Windows.
- Fichiers : deux campagnes automatisées sur 100 et 1 000 fichiers ;
  vérification octet pour octet des sorties, des traitements R/S/WU/*D,
  de l'absence de modification des sources et de la récupération après
  fichier manquant.
- CI Android : installation du SDK réparée, APK Debug construit et
  publié séparément dans GitHub Actions.

## Télécharger et tester sur Windows

Récupérer l'artefact **FREDPP-Windows-x64-Candidate-PR-N** depuis
GitHub Actions (exécution réussie de la pull request préparatoire).
L'artefact contient l'archive `FREDPP-v0.1.1-windows-x64.zip`.

1. Décompresser l'artefact GitHub, puis le ZIP du kit FREDPP.
2. Exécuter `fredpp.exe --version` : il doit afficher **FREDPP v0.1.1**.
3. Essayer `fredpp.exe hello` depuis le répertoire contenant
   la bibliothèque de procédures fournie ; vérifier que les commandes
   et les scripts personnels fonctionnent normalement.
4. Conserver une copie de sauvegarde des documents avant toute édition
   importante ; signaler les erreurs avec la commande, le résultat
   attendu et le résultat observé.

L'exécutable Windows est **non signé**. Windows SmartScreen ou Smart App
Control peut avertir ou bloquer son lancement. Ne désactivez pas
globalement les protections Windows pour exécuter FREDPP.

## Limitations connues

- Reconnaissance des 103 entrées du tableau historique, **sans garantie
  d'équivalence GCOS complète**.
- Certaines formes avancées de \B et \E restent non homologuées ; les
  références cycliques de motifs sont interrompues par une limite.
- Les budgets de durée sont vérifiés entre instructions : une commande
  bloquante n'est pas interrompue ; les buffers n'ont pas encore de
  plafond global de mémoire.
- La résistance à une coupure brutale pendant une écriture et aux
  fichiers exceptionnellement volumineux n'est pas entièrement validée.
- La livraison Windows candidate ne contient pas l'APK Android ; la
  compilation Android suit un workflow CI séparé.

## Diffusion ultérieure

Après retour des essais Windows, les notes pourront être finalisées et
le tag v0.1.1 créé sur le sommet exact de dev pour publier une nouvelle
**préversion**. Le workflow officiel produira les archives Windows,
Debian, macOS, l'extension VS Code et le manifeste de sommes SHA-256.
