# OR — Droit de lecture générale ( Option Read )

> L'option OR permet de valider ou d'invalider les droits d'accès en lecture générale des fichiers créés par Fred.

## Syntaxe

```fred
O[+|-]R
```

## Paramètres

Aucun paramètre explicite n'est documenté.

## Description

L'option OR permet de valider ou d'invalider les droits d'accès en lecture générale des fichiers créés par Fred.

## Exemples

```fred
b(bufo)
a>Creation d'un fichier>\fwa /fic11,1,22 b(0) 65130/fic1!clis \w +vName	Dev	Perms		Size	Max	Create	Modify	Changefic1		r  e		1c	u	03/Aug/92	03/Aug/92	03/Aug/92!o-rwa /fic21,1,22 b(0) 65130/fic2!clis \w +vName	Dev	Perms		Size	Max	Create	Modify	Changefic2		s none	1c	u	03/Aug/92	03/Aug/92	03/Aug/92!
```

## Options

Valeur par défaut O+R.

## Remarques

Aucune remarque spécifique n'est documentée.

## Compatibilité

| Implémentation | Statut |
|---|---|
| FRED historique | Compatible |
| FRED++ | À valider selon l'implémentation |

## Voir aussi

Aucun lien associé n'est encore défini.
