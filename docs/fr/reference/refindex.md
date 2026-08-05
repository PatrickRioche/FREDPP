# Référence des commandes FRED

> Référence alphabétique et fonctionnelle des commandes décrites dans le manuel historique DNB14A.

Cet index recense **103 commandes uniques**. Chaque lien ouvre la page Markdown correspondante dans le sous-dossier `commandes/`.

## Navigation rapide

- [Commandes principales](#commandes-principales)
- [Commandes spéciales](#commandes-speciales)
- [Famille F — Informations](#famille-f-informations)
- [Famille J — Sauts et branchements](#famille-j-sauts-et-branchements)
- [Famille N — Registres numériques](#famille-n-registres-numeriques)
- [Famille O — Options](#famille-o-options)
- [Famille Z — Commandes diverses](#famille-z-commandes-diverses)

## Commandes principales

| Commande | Fonction |
|---|---|
| [`A`](commandes/a.md) | Insertion  après la ligne courante ( Append) |
| [`B`](commandes/b.md) | Positionnement sur un buffer ( Buffer ) |
| [`C`](commandes/c.md) | Remplacement de n lignes ( Change ) |
| [`D`](commandes/d.md) | Suppression de n lignes ( Delete ) |
| [`E`](commandes/e.md) | Définition de modèle ( Pattern ) |
| [`F`](commandes/f.md) | Informations sur le buffer et association de fichier (Facts / File) |
| [`G`](commandes/g.md) | Global commande ( Global ) |
| [`I`](commandes/i.md) | Insertion de texte avant la ligne courante ( Insert ) |
| [`J`](commandes/j.md) | Sauts et branchements (Jump) |
| [`K`](commandes/k.md) | Copie de lignes vers un buffer désigné ( Kopie ) |
| [`L`](commandes/l.md) | Liste le contenu d'un fichier ( List ) |
| [`M`](commandes/m.md) | Déplacement de lignes vers un buffer (Move) |
| [`P`](commandes/p.md) | Impression ( Print ) |
| [`Q`](commandes/q.md) | Arrêt ( Quit ) |
| [`R`](commandes/r.md) | Lecture de fichier ( Read ) |
| [`S`](commandes/s.md) | ( Substitution ) |
| [`T`](commandes/t.md) | Test de conformité à un modèle ( Test ) |
| [`U`](commandes/u.md) | Répétition jusqu’à ce qu’une condition soit satisfaite (Until) |
| [`V`](commandes/v.md) | Défaire la dernière ligne de commandes ( Void ) |
| [`W`](commandes/w.md) | Ecriture dans un fichier ( Write ) |
| [`Y`](commandes/y.md) | Pourquoi ( whY ) |
| [`Z`](commandes/z.md) | Commandes diverses ( Zap ) |

## Commandes spéciales

| Commande | Fonction |
|---|---|
| [`!`](commandes/bang.md) | Appel d’un sous-système TSS |
| [`"`](commandes/quote.md) | Commentaire dans une procédure |
| [`#`](commandes/counter.md) | Impression du registre compteur |
| [`=`](commandes/equals.md) | Impression du numéro de ligne |
| [`:`](commandes/colon.md) | Imprime une fenêtre autour d'une ligne |
| [`@`](commandes/at.md) | Etiquette |
| [`\\`](commandes/backslashbackslash.md) | Attente pendant un nombre de secondes |

## Famille F — Informations

| Commande | Fonction |
|---|---|
| [`FB`](commandes/fb.md) | Impression d'informations sur les buffers |
| [`FE`](commandes/fe.md) | Impression d'informations sur les modèles |
| [`FF`](commandes/ff.md) | Impression d'informations sur les fichiers |
| [`FN`](commandes/fn.md) | Impression d'informations sur les registres |
| [`FO`](commandes/fo.md) | Impression d'informations sur les Options |
| [`F?`](commandes/fq.md) | Liste des buffers modifiés |
| [`FV`](commandes/fv.md) | Impression de la Version |

## Famille J — Sauts et branchements

| Commande | Fonction |
|---|---|
| [`JB`](commandes/jb.md) | Saut d'interruption ( Jump Break ) |
| [`JE`](commandes/je.md) | ( Jump Exit ) |
| [`JM`](commandes/jm.md) | ( Jump Message ) |
| [`JO`](commandes/jo.md) | Saut hors du buffer courant ( Jump Out ) |
| [`JP`](commandes/jp.md) | ( Jump Prompt ) |

## Famille N — Registres numériques

| Commande | Fonction |
|---|---|
| [`N:`](commandes/ncolon.md) | Affectation de valeur à un registre ( Numeric ) |
| [`N+`](commandes/nplus.md) | Addition d'une valeur dans un registre |
| [`N-`](commandes/nminus.md) | Soustraction d'une valeur dans un registre |
| [`N*`](commandes/nmultiply.md) | Multiplication d'une valeur dans un registre |
| [`N/`](commandes/ndivide.md) | Division entière d'une valeur dans un registre |
| [`N%`](commandes/nmodulo.md) | Reste de la division d'une valeur dans un registre |
| [`N~`](commandes/ncomplement.md) | Complément bit à bit d'une valeur d'un registre |
| [`N&`](commandes/nand.md) | ET bit à bit d'une valeur dans un registre |
| [`N\|`](commandes/npipe.md) | Valeur absolue d’un registre et OU inclusif bit à bit |
| [`N^`](commandes/nxor.md) | OU exclusif bit à bit d'une valeur dans un registre |
| [`N{`](commandes/nshiftleft.md) | Décalage à gauche bit à bit du nombre |
| [`N}`](commandes/nshiftright.md) | Décalage à droite bit à bit du nombre |
| [`N=`](commandes/nequals.md) | Test d'égalité d'une valeur avec un registre |
| [`N<`](commandes/nless.md) | Test d'infériorité d'une valeur avec un registre |
| [`N>`](commandes/ngreater.md) | Test de supériorité d'une valeur avec un registre |
| [`NA`](commandes/na.md) | Affectation du numéro de la ligne courante au registre |
| [`NB`](commandes/nb.md) | Changement de base d'un registre |
| [`ND`](commandes/nd.md) | Définition du nombre minimum de chiffres du registre |
| [`NF`](commandes/nf.md) | Définition du caractère de remplissage du registre |
| [`NL`](commandes/nl.md) | Affectation du nombre de caractère au registre |
| [`NP`](commandes/np.md) | Impression du contenu du registre |

## Famille O — Options

| Commande | Fonction |
|---|---|
| [`OB`](commandes/ob.md) | Nombre page en exécution |
| [`OC`](commandes/oc.md) | Changement de chaîne de caractères ( Option Change ) |
| [`OI(`](commandes/oi_parenthesis.md) | Buffer entre parenthèses ( Option Input ) |
| [`OIL`](commandes/oil.md) | Echappement minuscule en entrée ( Option Input Lower ) |
| [`OIU`](commandes/oiu.md) | Echappement majuscule en entrée ( Option Input Upper ) |
| [`OIE`](commandes/oie.md) | Définition caractère d'échappement ( Option Input Escape ) |
| [`OIF`](commandes/oif.md) | Avancement de ligne ( Option Input Feed ) |
| [`OIA`](commandes/oia.md) | Définition prompt mode entrée ( Option Input Append ) |
| [`OIC`](commandes/oic.md) | Définition prompt mode commande ( Option Input Cmd ) |
| [`OIR`](commandes/oir.md) | Définition prompt pour return ( Option Input Return ) |
| [`OI\F`](commandes/oi_backslash_f.md) | \F nécessaire ( Option Input \F ) |
| [`OOO`](commandes/ooo.md) | Impression caractère spécial octal ( Option Output Octal ) |
| [`OOL`](commandes/ool.md) | Echappement minuscule en sortie ( Option Output Lower ) |
| [`OOU`](commandes/oou.md) | Echappement majuscule en sortie ( Option Output Upper ) |
| [`OO`](commandes/oo.md) | Modification de caractère en sortie ( Option Output ) |
| [`OSD`](commandes/osd.md) | Différence majuscule et minuscule ( Option String Defaut ) |
| [`OS&`](commandes/os_ampersand.md) | & sens spécial dans la commande S ( Option Substitut & ) |
| [`OS`](commandes/os.md) | Définition caractère spécial ( Option Special ) |
| [`OS/`](commandes/os_separator.md) | Séparateur de fin de substitution ( Option Substitut / ) |
| [`OP`](commandes/op.md) | Taille de la page ( Option Page ) |
| [`OQ`](commandes/oq.md) | Abandon FRED si incident  ( Option Quit ) |
| [`OM`](commandes/om.md) | Passage mode trace ( Option Monitor ) |
| [`OR`](commandes/or.md) | Droit de lecture générale ( Option Read ) |
| [`OT`](commandes/ot.md) | Positionne des tabulations ( Option Tab ) |

## Famille Z — Commandes diverses

| Commande | Fonction |
|---|---|
| [`ZA`](commandes/za.md) | ( Zap Append ) |
| [`ZB`](commandes/zb.md) | Tri en ordre décroissant |
| [`ZC`](commandes/zc.md) | Efface l'indicateur "buffer modifié // fichier associé" |
| [`ZD`](commandes/zd.md) | Suppression de buffer |
| [`ZG`](commandes/zg.md) | Exécution des commandes et ramassage dans un buffer, de la production destinée à la visualisation |
| [`ZI`](commandes/zi.md) | Passage en mode tabulation |
| [`ZK`](commandes/zk.md) | Copie lignes dans un buffer désigné à la suite ( Zap Kopie ) |
| [`ZL`](commandes/zl.md) | Conversion en minuscules ( Zap Lower ) |
| [`ZM`](commandes/zm.md) | Déplacement lignes dans un buffer à la suite ( Zap Move ) |
| [`ZO`](commandes/zo.md) | Passage en mode non tabulation |
| [`ZP`](commandes/zp.md) | Impression de buffer   ( Zap Print ) |
| [`ZR`](commandes/zr.md) | Positionnement sur un buffer affecté à un fichier ( Zap Read ) |
| [`ZS`](commandes/zs.md) | Tri en ordre croissant ( Zap Sort ) |
| [`ZT`](commandes/zt.md) | Traduction ( Traduct ) |
| [`ZU`](commandes/zu.md) | Conversion en majuscules ( Zap Upper ) |
| [`ZV`](commandes/zv.md) | Défait la commande V ( Zap Void ) |
| [`ZW`](commandes/zw.md) | Ecriture à la suite ( Zap Write) |

## Remarque sur `N|`

Le manuel DNB14A contient deux notices portant le nom `N|` :

1. calcul de la valeur absolue d’un registre ;
2. OU inclusif bit à bit.

Ces deux formes sont réunies dans [`npipe.md`](commandes/npipe.md).

## Source

Documentation extraite du manuel historique **DNB14A** et organisée pour la documentation française de FRED++.
