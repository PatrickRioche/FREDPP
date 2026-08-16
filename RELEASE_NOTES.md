# FREDPP v0.0.19 — Sémantique historique du mode flot

Cette release consolide le moteur de flot de FREDPP et introduit une
représentation structurée de l'interprétation des caractères. Elle constitue
un jalon intermédiaire avant l'extension des autres directives historiques.

## Développement transversal de `\S(buffer)`

`\S(buffer)` n'est plus traité seulement dans quelques commandes particulières.
Son développement est maintenant réalisé transversalement dans les arguments
des commandes lorsque la syntaxe historique l'autorise.

Les noms de buffers calculés peuvent eux-mêmes contenir un développement
imbriqué de `\S`. Le contenu injecté par le développement externe reste
littéral et n'est pas réinterprété comme une nouvelle directive de flot.

## Directive `\L(buffer)`

`\L(buffer)` injecte le contenu du buffer en conservant ses retours de ligne.

Comme pour `\S`, les caractères injectés sont littéraux : une séquence telle
que `\S(other)` contenue dans le buffer est restituée telle quelle et n'est pas
développée une seconde fois.

## Directive `\C<caractère>`

`\C` force le caractère immédiatement suivant à être interprété littéralement.

Exemples validés :

```text
\C.    -> point littéral dans un modèle
\C^    -> caractère ^ littéral
\C)    -> parenthèse littérale dans un nom de buffer
```

FREDPP reproduit ici la sémantique logique de FRED. Il ne reproduit pas
l'ancienne transcription physique propre à GCOS/TSS, dans laquelle certains
caractères pouvaient être représentés par des valeurs octales.

## Directive `\O<caractère>`

`\O` effectue l'opération inverse de `\C` : le caractère suivant conserve ou
reprend sa signification spéciale.

La release valide notamment les cas historiques suivants dans les modèles :

```text
\O.              -> n'importe quel caractère
\O^ABC           -> ancrage en début de ligne
\O^A\O+\O$       -> ancrages + répétition
```

## Pipeline d'interprétation des caractères

FREDPP transporte maintenant explicitement trois états :

```text
Normal
Literal
ForcedSpecial
```

Cette information est conservée à travers le moteur de flot, le flux de
caractères, le lexer, les tokens et le parseur de modèles.

Cette architecture permet de distinguer un caractère spécial ordinaire, un
caractère neutralisé par `\C` et un caractère explicitement rendu spécial par
`\O`, sans effectuer des substitutions de chaînes fragiles commande par
commande.

## Limites et garde-fous

- longueur maximale historique documentée pour un nom de buffer : 15
  caractères ;
- extension FREDPP : jusqu'à 64 caractères ;
- profondeur maximale d'expansion du flot : 256 niveaux.

Cette release ne prétend pas encore couvrir l'ensemble des directives du mode
flot historique. Les autres familles restent intégrées progressivement dans la
roadmap vers FREDPP 1.0.0.

## Validation

Avant préparation de cette release sous Windows 11 :

- **50/50 tests réussis** ;
- validation end-to-end de `\S`, `\L`, `\C` et `\O` ;
- validation de `\C` / `\O` dans les modèles de la commande `G` ;
- dépôt Git propre et synchronisé avec `origin/main`.

La documentation historique située sous `docs/fr/reference/commandes` reste
inchangée.

## Paquets de release

Le workflow de release existant produit les artefacts supportés du projet :

- Windows x64 ;
- Debian/Linux amd64 ;
- Debian/Linux ARM64 ;
- macOS Intel x64 ;
- macOS Apple Silicon ARM64 ;
- extension Visual Studio Code FREDPP ;
- sommes de contrôle SHA-256.

### Windows 11

`fredpp.exe` n'est pas encore signé numériquement. Selon la configuration de
Windows 11, Smart App Control ou Microsoft Defender SmartScreen peut donc
bloquer son exécution.

Télécharger FREDPP uniquement depuis les Releases GitHub officielles. En cas de
blocage, vérifier l'alerte dans **Sécurité Windows** avant d'autoriser
l'exécution.

### macOS

Les exécutables macOS restent non signés et non notarisés dans cette version.
