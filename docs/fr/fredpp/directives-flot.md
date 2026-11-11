# Directives historiques du flot FRED — audit de conformité FREDPP

Les neuf directives sont **\B, \C, \E, \L, \N, \O, \R, \S et \W**.
Une directive de flot ne doit pas être confondue avec la commande portant la même lettre.

| Directive | Fonction | État de l'implémentation |
| --- | --- | --- |
| \B(buffer) | Injecte le buffer comme flot actif | Moteur de flot, appels autonomes, commandes séparées par espaces et blocs A/I/C couverts ; expressions arbitrairement imbriquées restant à étudier |
| \C | Protège le caractère suivant | Implémenté |
| \E(motif) | Référence un motif nommé, uniquement dans les motifs | Développement contextuel dans S/G/T, noms imbriqués et inconnus gérés ; récursion cyclique encore limitée |
| \L(buffer) | Injecte littéralement les lignes, fins de ligne conservées | Implémenté |
| \N(registre) | Injecte le registre avec ses options NB/ND/NF | Implémenté |
| \O | Force le caractère spécial suivant | Implémenté |
| \R | Lit une seule ligne du terminal, littéralement | Implémenté (commandes et blocs A/I/C) |
| \S(buffer) | Injecte littéralement les lignes sans fins de ligne | Implémenté |
| \W | Injecte le fichier associé au buffer courant | Implémenté et testé sur Windows |

## \N : formatage numérique

La valeur injectée par `\N(nom)` respecte la base (`NB`), la largeur
minimale (`ND`) et le caractère de remplissage (`NF`) du registre.
Un registre non défini vaut zéro dans le format par défaut.

```fred
N(valeur):42
N(valeur)B16
N(valeur)D4
JM!Valeur formatée : \N(valeur)!
```

Le message affiche `Valeur formatée : 002A`.
Attention : une substitution formatée en base 16 ne correspond pas
nécessairement à un opérande décimal valide pour une autre commande N.

## \E : uniquement à l'intérieur d'un motif

```fred
E(chiffres)/[0-9]+/
T/\E(chiffres)/
JM!La séquence \E(chiffres) reste littérale dans ce message.!
```

Seuls les paramètres de motif de `S`, `G` et `T` interprètent
`\E(nom)`. Les références définies sont regroupées pour préserver
les priorités des alternatives. Un nom inconnu ne peut jamais produire
de correspondance. Les définitions imbriquées finies sont prises en charge.

**Limites conservées :** les références cycliques sont arrêtées explicitement
au lieu d'autoriser toute la récursion historique ; les options de motif
fixées lors d'une ancienne définition et le raccourci historique
`E(nom)//` ne bénéficient pas encore d'une émulation complète.

## \B : flot actif

L'appel autonome `\B(buffer)` exécute le contenu comme une procédure,
avec pile d'appels, garde de profondeur et budget d'instructions.
L'insertion peut aussi être placée entre deux commandes complètes
séparées par des espaces. Dans les blocs de texte `A`, `I` ou `C`,
les lignes injectées et les directives imbriquées sont développées.

```fred
JP!Avant ! \B(sous-procedure) JM!Après!
```

Le buffer `sous-procedure` est exécuté après `JP`, puis le flux reprend
avec `JM`.

Les insertions `\B` au milieu de délimiteurs de messages, de motifs ou
d'expressions non séparées par des espaces ne sont pas assimilées
à des appels de procédure. Leur émulation historique n'est pas garantie.

## \R : lecture terminal littérale

La directive `\R` lit une seule ligne sur l'entrée standard du processus,
sans lire la procédure en cours. Elle élimine la fin de ligne et ne
développe aucune directive contenue dans la réponse.

```fred
JP!Saisissez une réponse : !
A
\R
\F
1P
```

Si la réponse est `Bonjour \N(x)`, le buffer reçoit exactement
cette chaîne. Une entrée absente ou fermée entraîne une erreur contrôlée.

## \W : chemin du buffer courant

Une fois un fichier associé au buffer par `F`, `R` ou `W` :

```fred
JM!Fichier courant : \W!
```

Le délimiteur `!` évite que les barres obliques d'un chemin soient
prises pour des fins de message. `\W` ne sauvegarde rien (à la différence
de la commande `W`). Un buffer sans fichier associé provoque une erreur.

Référence historique : https://www.thinkage.ca/gcos/expl/fred/stre.html
