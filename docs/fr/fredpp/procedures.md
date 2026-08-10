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

## LIMITES DU BOOTSTRAP COURANT

Restent à intégrer dans les lots suivants :

- `B(d)` : date ;
- `B(t)` : heure ;
- `B(u)` : utilisateur ;
- `.init`.

Les commandes FRED non encore implémentées restent indisponibles.
