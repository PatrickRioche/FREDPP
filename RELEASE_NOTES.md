# FREDPP v0.1.3 — Préversion de substitutions numérotées

Cette préversion est construite depuis la branche `dev`.

## Évolutions depuis v0.1.2

- `S[n]` remplace une occurrence par ligne. Un rang positif compte depuis la gauche et un rang négatif depuis la droite. La forme sans rang remplace toutes les occurrences.
- Les formes `1s1/chat/A/p` et `1s-2/chat/B/p` sont prises en charge dans le terminal et dans les procédures.
- Les substitutions numérotées respectent les options de casse, l'expansion de `&` et le séparateur final facultatif.
- L'aide `?s` présente la syntaxe et un exemple de substitution depuis la droite.
- Les tests couvrent les plages de lignes, les rangs invalides ou absents, les motifs vides et les combinaisons d'options.

## Exemple dans FREDPP

Sur une première ligne contenant `chat chat chat chat`, exécuter successivement les deux commandes suivantes.

```text
1s1/chat/A/p
1s-2/chat/B/p
```

Les résultats affichés sont les suivants.

```text
A chat chat chat
A chat B chat
```

## Fichiers distribués

- `FREDPP-v0.1.3-windows-x64.zip`
- `FREDPP-v0.1.3-debian13-amd64.tar.gz`
- `FREDPP-v0.1.3-debian13-arm64.tar.gz`
- `FREDPP-v0.1.3-macos-x64.tar.gz`
- `FREDPP-v0.1.3-macos-arm64.tar.gz`
- `fredpp_0.1.3_amd64.deb`
- `fredpp_0.1.3_arm64.deb`
- `FREDPP-Language-v0.1.3.vsix`
- `SHA256SUMS.txt`

GitHub fournit également deux archives source (ZIP et TAR.GZ). Le script Python de génération de la politique Minimax est destiné aux développeurs, pas aux utilisateurs du jeu.

## Utilisation de l'alias dans Windows PowerShell

Dans le dossier du kit Windows décompressé :

```powershell
.\fp.exe --version
.\fp.exe eliza
.\fp.exe tictactoe solo
.\fredpp.exe tictactoe
```

Pour lancer simplement `fp` à partir d'un autre répertoire, le dossier des exécutables doit figurer dans la variable d'environnement `PATH`.

## Limites connues

- Les 103 entrées historiques reconnues ne correspondent pas encore à une émulation GCOS exhaustive.
- Certaines formes avancées des directives de flot doivent encore être homologuées.
- Les délais d'exécution sont contrôlés entre les instructions, pas au milieu d'une commande externe bloquante.
- Les binaires Windows ne sont pas signés ; SmartScreen peut signaler une application inconnue. Ne désactivez pas les protections pour lancer le programme.
- Android dispose d'un workflow indépendant et n'est pas inclus dans cette matrice de release.
