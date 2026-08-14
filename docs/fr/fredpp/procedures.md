# PROCÉDURES FREDPP — BOOTSTRAP

FREDPP peut exécuter une procédure directement depuis la ligne de commande.

## LANCEMENT PAR NOM

```text
fredpp hello
```

Le fichier physique reste `hello.fredpp`. L'extension `.fredpp` sert notamment
à l'association avec Visual Studio Code ; elle n'a pas besoin d'être saisie
dans l'usage courant du CLI.

Pour un nom simple, sous Windows, FREDPP recherche dans cet ordre :

```text
.\hello.fredpp
C:\fredpp\library\hello.fredpp
```

Le premier fichier trouvé est exécuté. Les chemins explicites restent acceptés :

```text
fredpp .\mesprocs\hello.fredpp
fredpp C:\travail\hello.fredpp
```

Un chemin explicite sans extension est également essayé avec `.fredpp`.

## PARAMÈTRES DE PROCÉDURE : B(0)

Les arguments après le nom de la procédure sont chargés dans `B(0)`, un
argument par ligne.

```text
fredpp aide hello
```

initialise :

```text
B(0)
hello
```

et :

```text
fredpp exemple premier second "troisième argument"
```

initialise :

```text
B(0)
premier
second
troisième argument
```

FREDPP utilise directement les arguments transmis au programme par le système ;
il ne redécoupe pas lui-même la ligne de commande sur les espaces.

## PROGRAMME : B(.)

Le fichier sélectionné est chargé dans `B(.)`, puis exécuté.

```text
fredpp nom [arg1] [arg2] ...
        |
        +--> B(0) = arguments, un par ligne
        +--> B(.) = contenu de nom.fredpp
        +--> exécution de B(.)
```

## EXÉCUTION D'UN BUFFER

```fred
\B(monbuffer)
```

Dans l'implémentation actuelle, `\B(buffer)` doit être seul sur sa ligne. Les
appels peuvent être imbriqués et une profondeur maximale protège contre la
récursion infinie.

## COMMANDES DE TEXTE

`A`, `I` et `C` utilisent les lignes suivantes jusqu'à `\F`.

```fred
B(data)
A
première ligne
deuxième ligne
\F
1,$P
```

## COMMENTAIRES ET MESSAGES

Les commentaires `"` ainsi que `JM` et `JP` sont utilisables. Les formes
délimitées `JM/.../` et `JP/.../` peuvent être enchaînées :

```fred
JP/Chargement... / JM/OK/
```

## MONITORAGE

`O-M` est l'état implicite. `O+M` active le monitorage et `O-M` le désactive.
`FO` affiche l'état de `OI(` et de `OM`.

## BUFFERS SYSTÈME DU BOOTSTRAP

Avant l'exécution de `B(.)`, FREDPP initialise maintenant les buffers
historiques suivants :

```text
B(d) = MM/DD/YY
B(t) = HH:MM
B(u) = utilisateur courant
```

Ces formats reproduisent le bootstrap décrit par DNB11A : date `mm/jj/aa`,
heure `hh:mn` et USER-ID courant.

`B(0)` continue de contenir les paramètres de la ligne de commande et existe
également lorsqu'aucun paramètre n'est fourni.

## `FB` ET LES BUFFERS DU BOOTSTRAP

À partir du Sprint 2.22, les buffers système créés au démarrage d'une procédure
sont tous enregistrés dans l'ordre d'utilisation des buffers. La commande
`FB` doit donc notamment pouvoir afficher :

```text
b(d)
b(t)
b(u)
```

en plus des autres buffers ouverts comme `B(0)` et `B(.)`.

Ce correctif ne modifie pas le contenu de `B(d)`, `B(t)` ou `B(u)` ; il corrige
uniquement leur visibilité dans l'état des buffers.

## FICHIER D'INITIALISATION UTILISATEUR

Le bootstrap UWTOOLS exécute l'initialisation utilisateur après `B(d)`,
`B(t)` et `B(u)`, mais avant la construction de `B(0)` et avant le programme
principal. FREDPP reproduit cet ordre.

Chemin par défaut :

```text
Windows : %USERPROFILE%\fredpp\.init.fredpp
Unix    : $HOME/fredpp/.init.fredpp
```

L'extension `.fredpp` conserve l'association avec Visual Studio Code.

Si le fichier par défaut n'existe pas, le démarrage continue normalement.
`FREDPP_INIT` permet de fournir explicitement un autre fichier. Une valeur vide
désactive l'init. Un fichier explicitement demandé mais absent, illisible ou
en erreur pendant son exécution interrompt le bootstrap.

Le fichier est exécuté dans un buffer temporaire interne `__init`, supprimé
ensuite. Les autres buffers et options créés par l'init restent disponibles
pour le programme principal.

## ORDRE DU BOOTSTRAP FREDPP

```text
1. B(d), B(t), B(u)
2. .init.fredpp utilisateur éventuel
3. B(0) = paramètres CLI
4. résolution de la procédure
5. B(.) = programme
6. exécution de B(.)
```

Les commandes FRED non encore implémentées restent indisponibles.

## COMMANDE HISTORIQUE `M(buffer)`

La commande `M` suit la syntaxe historique :

```text
(.,.)M<nom de buffer>
```

Exemples :

```text
M(chemin)
1M(chemin)
1,2M(chemin)
```

Les lignes sélectionnées remplacent le contenu précédent du buffer destination,
puis sont supprimées du buffer source. Le buffer destination peut être le
buffer courant.


## `\S(buffer)` DANS `!` ET `ZG`

Le Sprint 2.22 raccorde `\S(buffer)` au texte des commandes système :

```fred
!echo \S(valeur)
zg(sortie)!dir \S(chemin) /B /S
```

Le traitement vise uniquement les occurrences `\S(...)`. Le reste du texte
n'est pas repassé globalement dans le moteur de flot, afin de préserver les
backslashes ordinaires des chemins Windows comme `C:\Backup`.

Le contenu injecté conserve la sémantique historique de `\S` : suppression des
retours de ligne et caractères injectés littéraux.

La généralisation complète aux autres familles de commandes reste distincte.


## ERREUR DE PROCÉDURE ET MODE DEBUG INTERACTIF

Lorsqu'une erreur survient pendant l'exécution du programme principal, FREDPP
n'abandonne plus immédiatement vers le shell. Le comportement reprend le
principe historique de FRED :

1. la procédure est arrêtée ;
2. jusqu'à trois lignes du reste de l'entrée sont affichées ;
3. `...` indique qu'il reste davantage de lignes ;
4. les buffers et l'état de travail sont conservés ;
5. FREDPP revient dans sa boucle de commandes interactive.

Cela permet notamment d'utiliser immédiatement `FB`, `B(buffer)` et `*` pour
examiner l'état laissé par la procédure.

Les erreurs antérieures au lancement du programme principal restent fatales
pour le lancement : bootstrap, `.init` utilisateur ou résolution de fichier de
procédure.

