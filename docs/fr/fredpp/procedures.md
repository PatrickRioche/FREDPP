# PROCÉDURES FREDPP — BOOTSTRAP

FREDPP peut exécuter une procédure directement depuis la ligne de commande.

## LANCEMENT PAR NOM

```text
fredpp hello
```

Le fichier physique reste `hello.fredpp`. L'extension `.fredpp` sert notamment
à l'association avec Visual Studio Code ; elle n'a pas besoin d'être saisie
dans l'usage courant du CLI.

Pour un nom simple, FREDPP recherche dans cet ordre :

```text
.\hello.fredpp
.\library\hello.fredpp
<dossier_de_fredpp.exe>\library\hello.fredpp
```

Sous Windows, `C:\fredpp\library` reste également pris en charge.
Sur Linux, les répertoires d'installation
`/usr/local/share/fredpp/library` et `/usr/share/fredpp/library`
sont consultés. Le premier fichier existant est exécuté.
Les chemins explicites restent acceptés :

```text
fredpp .\mesprocs\hello.fredpp
fredpp C:\travail\hello.fredpp
```

Un chemin explicite sans extension est également essayé avec `.fredpp`.

## ECFP — TRAITEMENT D'UNE LISTE DE FICHIERS

`library/ecfp.fredpp` est une procédure d'intérêt général fournie
avec les kits Windows, Debian et macOS. Elle exécute un programme de commandes
FREDPP sur chaque fichier dont le chemin figure dans une liste (un chemin par
ligne).

```powershell
fredpp ecfp .\fichiers.txt .\transformations.fredpp VOIR
fredpp ecfp .\fichiers.txt .\transformations.fredpp FAIRE
```

- **VOIR** : affiche les résultats transformés en mémoire, sans écriture
  automatique de la procédure ECFP sur les fichiers traités.
- **FAIRE** : applique les commandes puis écrit sur les fichiers d'origine
  avec `W`. **Faire une sauvegarde** avant de lancer ce mode.
- Les fichiers de la liste et le programme de commandes doivent exister ;
  les chemins contenant des espaces sont pris en charge.
- **Sécurité :** le mode VOIR n'est pas un bac à sable. La procédure de
  commandes fournie doit être fiable et ne doit pas contenir d'écriture
  autonome (`W`, `ZW`), de commande système (`!`, `ZG`) ou de `QQ`.
  L'appel d'autres procédures peut également produire des effets secondaires.

La bibliothèque est automatiquement incluse dans les archives portables ;
les paquets Debian installent les procédures sous
`/usr/share/fredpp/library` (selon le préfixe d'installation).

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

## BOUCLES ET LIMITES DE SÉCURITÉ

La commande `fredpp --limits` affiche les plafonds réellement actifs et la
provenance de leurs valeurs (défaut ou environnement), sans lancer l'éditeur.
La rubrique `?limites` est également intégrée à l'aide française de FREDPP.
Pour un tableau complet, consulter [les limites d'exécution](limites.md).


Les sauts arrière `J(étiquette)` restent autorisés : ils sont nécessaires aux
boucles historiques, notamment lorsqu'un registre `N(...)` est décrémenté puis
testé pour décider de la sortie. Un saut arrière n'est pas une erreur en soi.

Pour éviter qu'une boucle sans fin bloque l'éditeur, chaque lancement d'une
procédure dispose de deux limites indépendantes, **partagées entre la procédure
principale et les appels `\B` imbriqués** :

- **1 000 000 de lignes exécutées**, par défaut ;
- **60 secondes écoulées**, par défaut (vérification entre les lignes).

La première limite atteinte arrête la procédure avec un message explicite,
signalant une *possible* boucle infinie. Le mécanisme habituel d'inspection
après erreur reste disponible et la sémantique des sauts ne change pas.

Pour augmenter les limites avant de lancer FREDPP sous PowerShell :

```powershell
$env:FREDPP_PROCEDURE_MAX_STEPS = '5000000'
$env:FREDPP_PROCEDURE_MAX_SECONDS = '300'
fredpp ma_procedure
```

Les deux variables doivent contenir un entier strictement positif. La durée
maximale configurable est de 86 400 secondes. Ces garde-fous n'interrompent
pas une commande bloquante *pendant* son exécution : le temps est contrôlé
quand FREDPP passe à une nouvelle ligne de procédure.

Les commandes de répétition `U` possèdent aussi une protection **partagée entre
tous les `U` imbriqués** : 1 000 000 d'itérations cumulées ou 60 secondes, par
défaut. Cette limite évite qu'une imbrication de répétitions pourtant bornées
individuellement provoque une exécution démesurée. Elle s'applique également
aux `U` lancés directement en mode interactif. Pour un traitement plus long :

```powershell
$env:FREDPP_REPEAT_MAX_STEPS = '5000000'
$env:FREDPP_REPEAT_MAX_SECONDS = '300'
```

Ces limites sont distinctes de celles qui comptent les lignes de procédure.
Elles se réinitialisent entre deux commandes `U` indépendantes. Les dépassements
ne sont pas absorbés par le mode `UE` (« arrêter sur erreur »).

## DIRECTIVES DU FLOT

Les neuf directives historiques `\B`, `\C`, `\E`, `\L`, `\N`,
`\O`, `\R`, `\S` et `\W` sont présentées avec leur état de conformité
dans [directives-flot.md](directives-flot.md), également accessible par
`?directives`.

## SUBSTITUTION NUMÉRIQUE `\N(registre)`

La directive `\N(nom)` remplace sa propre écriture par la valeur formatée
du registre numérique `N(nom)`, conformément aux options `NB`, `ND` et `NF`,
y compris dans les arguments de commandes et les messages des procédures :

```fred
N(total):12
JM/Total : \N(total)/
N(copie):\N(total)
```

Le message affiche `Total : 12` et le registre `copie` reçoit la valeur `12`.
La substitution lit la valeur numérique courante sans modifier le registre ni
le compteur. Un registre non initialisé vaut `0`. Les options de présentation
`NB`, `ND` et `NF` concernent l'impression d'un registre (`NP`) et
l'injection par `\N`. Si un format non décimal est choisi, le résultat
n'est pas nécessairement un opérande numérique décimal valide.

Le nom du registre est limité à 14 caractères. Il peut être construit avec
une substitution imbriquée comme `\N(\S(nom))`. La directive `\C` peut
protéger le caractère `\` pour empêcher l'expansion.

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


## `JM` / `JP` — DÉLIMITEURS HISTORIQUES

Le Sprint 2.22 complète `JM` et `JP` avec la syntaxe historique délimitée.

Tout caractère non alphanumérique placé immédiatement après `JM` ou `JP` peut
servir de délimiteur. Le même caractère ferme le message :

```fred
JM!Mode d'emploi : !
JP:Input A/B:
JM?Terminé?
JP;Question ?;
```

Le caractère `/` reste compatible :

```fred
JM/Message/
JP/Invite : /
```

Après le délimiteur fermant, l'exécution continue sur la commande suivante de
la même ligne grâce au moteur générique du Lot 4.1 :

```fred
JM!Début! JP:Suite: B(data)
```

La forme alternative terminée par la fin de ligne reste disponible lorsqu'un
espace suit `JM` ou `JP` :

```fred
JM Message avec retour
JP Invite :
```

Les directives de flot sont développées par le runtime avant affichage. La
limite historique de 2000 caractères est vérifiée avant et après développement.

L'omission du délimiteur fermant sous l'option historique `O-S/` reste hors
périmètre tant que cette option n'est pas implémentée.

