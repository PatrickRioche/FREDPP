# Directives historiques du flot FRED — audit de FREDPP

Les neuf directives attendues sont **\B, \C, \E, \L, \N, \O, \R, \S et \W**.
Elles ne doivent pas être confondues avec les commandes portant les mêmes noms.

| Directive | Fonction historique | Statut FREDPP sur dev |
| --- | --- | --- |
| \B(buffer) | Injecte le contenu comme flot actif de commandes ou de texte | Partiel : certaines formes ne sont pas raccordées aux procédures |
| \C | Rend littéral le caractère suivant | Implémenté |
| \E(motif) | Injecte un motif nommé par E, seulement dans un motif | Partiel : développement préalable trop général |
| \L(buffer) | Injecte littéralement, en conservant les fins de ligne | Implémenté |
| \N(registre) | Injecte la valeur selon le format du registre | Partiel : substitution décimale actuelle |
| \O | Force l'interprétation spéciale du caractère suivant | Implémenté |
| \R | Lit une ligne du terminal, littéralement, sans fin de ligne | Non implémenté |
| \S(buffer) | Injecte littéralement, sans fins de ligne | Implémenté |
| \W | Injecte le nom du fichier associé au buffer courant | Implémenté dans le moteur de flot, tests Windows à confirmer |

## Exemple de \W

Associer un fichier au buffer courant avec la commande R, F ou W, puis saisir :

```fred
JM/Fichier courant : \W/
!echo \W
```

La directive \W ne sauvegarde aucun fichier : c'est la commande W
qui écrit sur disque. Les formes \W et \w sont reconnues. Les antislashs
contenus dans les noms de fichiers Windows restent littéraux.

Dans FREDPP, l'absence de nom de fichier associé provoque une erreur
explicite, pour éviter qu'un programme utilise silencieusement un chemin
vide ; le cas exact dans FRED historique n'est pas encore établi.

## Écarts à résoudre

- \E doit être reconnue seulement à l'intérieur des motifs.
- \R doit lire une ligne au terminal sans interpréter les directives.
- \N doit respecter les formats NB/ND/NF des registres.
- \B doit être complet en entrée de texte et en flot mixte.

La directive \F de fin de saisie de texte reste également nécessaire
mais elle ne figure pas dans la liste des neuf directives de cet audit.

Référence historique : https://www.thinkage.ca/gcos/expl/fred/stre.html
