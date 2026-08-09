# PROCÉDURES FREDPP — BOOTSTRAP MINIMAL

FREDPP peut exécuter un fichier de commandes directement depuis la ligne de commande.

## LANCEMENT

```text
fredpp nomscript.fredpp
```

L'extension `.fredpp` est la convention recommandée. Le moteur accepte toutefois un autre nom de fichier.

## BOOTSTRAP v0.0.12

Le bootstrap minimal effectue les opérations suivantes :

1. création et positionnement sur le buffer `.` ;
2. chargement du fichier demandé dans `B(.)` ;
3. exécution du contenu comme par `\B(.)` ;
4. fin du processus à la fin normale de la procédure, selon le principe du `QQ` final du bootstrap.

Le bootstrap historique complet (arguments dans `B(0)`, `.init`, buffers utilisateur/date/heure et recherche dans les bibliothèques FRED) sera ajouté progressivement.

## EXÉCUTION D'UN BUFFER

Une ligne de procédure peut appeler un autre buffer :

```fred
\B(monbuffer)
```

Dans cette première version, `\B(buffer)` doit être seul sur sa ligne. Les appels peuvent être imbriqués et une profondeur maximale protège contre la récursion infinie.

## DIRECTIVE DE FLOT `\S(buffer)`

Selon la documentation historique DNB11A, `\S` injecte le contenu du buffer
désigné dans l'entrée courante en supprimant tous les retours de ligne. Les
caractères provenant du buffer sont pris littéralement : une séquence telle que
`\B(...)` contenue dans le buffer injecté n'est donc pas réinterprétée comme une
directive de flot.

Dans FREDPP v0.0.13, la forme prise en charge est :

```fred
\S(buffer)
```

Dans le Sprint 2.18, cette expansion est raccordée aux entrées textuelles des
messages `JM/.../` et `JP/.../`. Les caractères injectés par `\S` restent des
données littérales même s'ils contiennent des caractères qui seraient spéciaux
dans l'entrée originale.

Cette implémentation constitue la première brique du traitement historique des
directives de flot littérales. La généralisation de cette entrée structurée à
toutes les commandes, ainsi que `\L`, `\C`, `\E`, `\N`, `\O`, `\R` et `\W`,
restent reportées.

## COMMANDES DE TEXTE

Les commandes `A`, `I` et `C` utilisent directement les lignes suivantes jusqu'à :

```fred
\F
```

Exemple :

```fred
B(data)
A
première ligne
deuxième ligne
\F
1,$P
```

## COMMENTAIRES ET MESSAGES

Les commentaires `"` ainsi que `JM` et `JP` sont utilisables dans les procédures.

Les formes délimitées `JM/.../` et `JP/.../` peuvent être enchaînées sur une même ligne de procédure :

```fred
JP/Chargement... / JM/OK/
```

## MONITORAGE

`O-M` est l'état implicite. Avec :

```fred
O+M
```

les commandes suivantes sont imprimées avant leur exécution. `O-M` désactive le monitorage. `FO` affiche l'état de `OI(` et de `OM`.

## LIMITES DE CE BOOTSTRAP

- `\B(buffer)` est pour l'instant une directive de ligne autonome ;
- les suites générales de commandes sur une même ligne ne sont pas encore disponibles, sauf les suites délimitées `JM/JP` ;
- le bootstrap historique complet n'est pas encore reproduit ;
- les commandes FRED non encore implémentées restent naturellement indisponibles.
