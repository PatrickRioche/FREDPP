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


## `\S(buffer)` DANS UN MODÈLE `G`

Le Sprint 2.22 permet d'utiliser `\S(buffer)` dans une commande globale `G`,
aussi bien dans une procédure qu'en mode interactif. Le cas de référence est :

```fred
b(model)
a
exe
\F

b(list)
g~/\S(model)/d
```

Avec `B(model)` contenant `exe`, cette commande est développée avant parsing en
une commande équivalente à :

```fred
g~/exe/d
```

Les formes adressées restent également reconnues, par exemple :

```fred
1,$g/\S(model)/p
```

Ce raccordement est volontairement limité à la commande `G`. Il évite de
repasser arbitrairement toutes les commandes dans le moteur de flot et prépare
la généralisation progressive des entrées FRED.

La sémantique structurelle complète du caractère `literal` à travers le lexer
et le parser de motifs reste une étape distincte pour les modèles contenant
eux-mêmes des métacaractères FRED.


### Validation interactive

Le même traitement est appliqué à la boucle de commandes interactive. Ainsi :

```fred
b(list)
g~/\S(model)/d
*
```

utilise la valeur courante de `B(model)` exactement comme lors de l'exécution
d'une procédure. Une commande peut donc être rejouée manuellement pendant le
debug avec le même comportement.


## CONTRÔLE DE FLOT `@(label)` ET `J(label)[T|F]`

Le Sprint 2.22 implémente un premier sous-ensemble historique du contrôle de
flot des procédures :

```fred
J(label)
J(label)T
J(label)F
@(label)
```

- `J(label)` saute inconditionnellement ;
- `J(label)T` saute si le registre de condition vaut vrai ;
- `J(label)F` saute si le registre de condition vaut faux ;
- `@(label)` définit la position cible ;
- le nom est limité à 15 caractères ;
- la comparaison des noms d'étiquette est insensible à la casse.

Dans ce lot, la recherche part de la ligne qui suit `J` et choisit la première
étiquette correspondante située plus loin dans le même buffer de procédure.
Il n'y a pas de retour implicite au début du buffer. Une cible absente produit
l'erreur historique `? label not found`.

Les formes `JT`/`JF` sans label, les commandes restantes sur la même ligne et
les variantes `JB`, `JE`, `JO` ne font pas partie de ce lot.

Le registre de condition existe déjà dans le runtime ; le prochain lot `N`
permettra notamment de le positionner à partir d'expressions numériques.


## REGISTRES NUMÉRIQUES `N` — SOUS-ENSEMBLE MINIMAL

Le Sprint 2.22 ajoute :

```fred
N(reg):valeur
N(reg)=valeur
N(reg)<valeur
N(reg)>valeur
```

Les opérandes disponibles sont un entier signé, `$`, `.` et `#`.

Le programme reste stocké dans `B(.)`, mais commence son exécution avec `B(0)`
comme buffer courant. Ainsi `$` représente le nombre de paramètres transmis.

Cas historique de référence :

```fred
N(np):$>0 J(param)T
```

Les comparaisons positionnent le registre de condition utilisé par
`J(label)T/F`. Après chaque opération `N`, la valeur du registre numérique est
recopiée dans `#`.

Un registre absent vaut `0` à sa première lecture. Son nom est limité à
14 caractères.

Les opérations arithmétiques, bit-à-bit et les autres variantes de `N` restent
hors de ce lot.


## ENCHAÎNEMENT DE COMMANDES SUR UNE MÊME LIGNE

Le Sprint 2.22 ajoute l'exécution séquentielle de plusieurs commandes FRED
ordinaires sur une même ligne.

Exemple :

```fred
B(buff) A
```

En interactif, `B(buff)` est d'abord exécuté ; `A` démarre ensuite la saisie
de texte dans ce nouveau buffer.

Plusieurs commandes peuvent aussi être enchaînées :

```fred
B(other) B(buff) *
```

Le parseur strict `CommandParser::parse()` conserve son contrat : il exige
toujours une commande complète jusqu'à la fin de l'entrée. Les moteurs
interactif et procédure utilisent `parse_one()` pour consommer une commande,
l'exécuter puis reprendre sur le même flux de tokens.

Cette première généralisation concerne les commandes dont la grammaire permet
de déterminer leur fin sans ambiguïté. Les constructions qui consomment le
reste de la ligne et certains contrôles de flot de procédure (`N`, `J`, `\B`)
conservent encore leur chemin spécialisé.

