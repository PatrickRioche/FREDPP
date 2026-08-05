# Index des commandes FRED

> Référence alphabétique et fonctionnelle des commandes décrites dans le manuel historique DNB14A.

Cet index recense **103 commandes uniques**. Chaque lien ouvre la page Markdown correspondante dans le même dossier.

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
| [`A`](a.md) | Insertion  après la ligne courante ( Append) |
| [`B`](b.md) | Positionnement sur un buffer ( Buffer ) |
| [`C`](c.md) | Remplacement de n lignes ( Change ) |
| [`D`](d.md) | Suppression de n lignes ( Delete ) |
| [`E`](e.md) | Définition de modèle ( Pattern ) |
| [`F`](f.md) | Informations sur le buffer et association de fichier (Facts / File) |
| [`G`](g.md) | Global commande ( Global ) |
| [`I`](i.md) | Insertion de texte avant la ligne courante ( Insert ) |
| [`J`](j.md) | Sauts et branchements (Jump) |
| [`K`](k.md) | Copie de lignes vers un buffer désigné ( Kopie ) |
| [`L`](l.md) | Liste le contenu d'un fichier ( List ) |
| [`M`](m.md) | Déplacement de lignes vers un buffer (Move) |
| [`P`](p.md) | Impression ( Print ) |
| [`Q`](q.md) | Arrêt ( Quit ) |
| [`R`](r.md) | Lecture de fichier ( Read ) |
| [`S`](s.md) | ( Substitution ) |
| [`T`](t.md) | Test de conformité à un modèle ( Test ) |
| [`U`](u.md) | Répétition jusqu’à ce qu’une condition soit satisfaite (Until) |
| [`V`](v.md) | Défaire la dernière ligne de commandes ( Void ) |
| [`W`](w.md) | Ecriture dans un fichier ( Write ) |
| [`Y`](y.md) | Pourquoi ( whY ) |
| [`Z`](z.md) | Commandes diverses ( Zap ) |

## Commandes spéciales

| Commande | Fonction |
|---|---|
| [`!`](bang.md) | Appel d’un sous-système TSS |
| [`"`](quote.md) | Commentaire dans une procédure |
| [`#`](counter.md) | Impression du registre compteur |
| [`=`](equals.md) | Impression du numéro de ligne |
| [`:`](colon.md) | Imprime une fenêtre autour d'une ligne |
| [`@`](at.md) | Etiquette |
| [`\\`](backslashbackslash.md) | Attente pendant un nombre de secondes |

## Famille F — Informations

| Commande | Fonction |
|---|---|
| [`FB`](fb.md) | Impression d'informations sur les buffers |
| [`FE`](fe.md) | Impression d'informations sur les modèles |
| [`FF`](ff.md) | Impression d'informations sur les fichiers |
| [`FN`](fn.md) | Impression d'informations sur les registres |
| [`FO`](fo.md) | Impression d'informations sur les Options |
| [`F?`](fq.md) | Liste des buffers modifiés |
| [`FV`](fv.md) | Impression de la Version |

## Famille J — Sauts et branchements

| Commande | Fonction |
|---|---|
| [`JB`](jb.md) | Saut d'interruption ( Jump Break ) |
| [`JE`](je.md) | ( Jump Exit ) |
| [`JM`](jm.md) | ( Jump Message ) |
| [`JO`](jo.md) | Saut hors du buffer courant ( Jump Out ) |
| [`JP`](jp.md) | ( Jump Prompt ) |

## Famille N — Registres numériques

| Commande | Fonction |
|---|---|
| [`N:`](ncolon.md) | Affectation de valeur à un registre ( Numeric ) |
| [`N+`](nplus.md) | Addition d'une valeur dans un registre |
| [`N-`](nminus.md) | Soustraction d'une valeur dans un registre |
| [`N*`](nmultiply.md) | Multiplication d'une valeur dans un registre |
| [`N/`](ndivide.md) | Division entière d'une valeur dans un registre |
| [`N%`](nmodulo.md) | Reste de la division d'une valeur dans un registre |
| [`N~`](ncomplement.md) | Complément bit à bit d'une valeur d'un registre |
| [`N&`](nand.md) | ET bit à bit d'une valeur dans un registre |
| [`N\|`](npipe.md) | Valeur absolue d’un registre et OU inclusif bit à bit |
| [`N^`](nxor.md) | OU exclusif bit à bit d'une valeur dans un registre |
| [`N{`](nshiftleft.md) | Décalage à gauche bit à bit du nombre |
| [`N}`](nshiftright.md) | Décalage à droite bit à bit du nombre |
| [`N=`](nequals.md) | Test d'égalité d'une valeur avec un registre |
| [`N<`](nless.md) | Test d'infériorité d'une valeur avec un registre |
| [`N>`](ngreater.md) | Test de supériorité d'une valeur avec un registre |
| [`NA`](na.md) | Affectation du numéro de la ligne courante au registre |
| [`NB`](nb.md) | Changement de base d'un registre |
| [`ND`](nd.md) | Définition du nombre minimum de chiffres du registre |
| [`NF`](nf.md) | Définition du caractère de remplissage du registre |
| [`NL`](nl.md) | Affectation du nombre de caractère au registre |
| [`NP`](np.md) | Impression du contenu du registre |

## Famille O — Options

| Commande | Fonction |
|---|---|
| [`OB`](ob.md) | Nombre page en exécution |
| [`OC`](oc.md) | Changement de chaîne de caractères ( Option Change ) |
| [`OI(`](oi_parenthesis.md) | Buffer entre parenthèses ( Option Input ) |
| [`OIL`](oil.md) | Echappement minuscule en entrée ( Option Input Lower ) |
| [`OIU`](oiu.md) | Echappement majuscule en entrée ( Option Input Upper ) |
| [`OIE`](oie.md) | Définition caractère d'échappement ( Option Input Escape ) |
| [`OIF`](oif.md) | Avancement de ligne ( Option Input Feed ) |
| [`OIA`](oia.md) | Définition prompt mode entrée ( Option Input Append ) |
| [`OIC`](oic.md) | Définition prompt mode commande ( Option Input Cmd ) |
| [`OIR`](oir.md) | Définition prompt pour return ( Option Input Return ) |
| [`OI\F`](oi_backslash_f.md) | \F nécessaire ( Option Input \F ) |
| [`OOO`](ooo.md) | Impression caractère spécial octal ( Option Output Octal ) |
| [`OOL`](ool.md) | Echappement minuscule en sortie ( Option Output Lower ) |
| [`OOU`](oou.md) | Echappement majuscule en sortie ( Option Output Upper ) |
| [`OO`](oo.md) | Modification de caractère en sortie ( Option Output ) |
| [`OSD`](osd.md) | Différence majuscule et minuscule ( Option String Defaut ) |
| [`OS&`](os_ampersand.md) | & sens spécial dans la commande S ( Option Substitut & ) |
| [`OS`](os.md) | Définition caractère spécial ( Option Special ) |
| [`OS/`](os_separator.md) | Séparateur de fin de substitution ( Option Substitut / ) |
| [`OP`](op.md) | Taille de la page ( Option Page ) |
| [`OQ`](oq.md) | Abandon FRED si incident  ( Option Quit ) |
| [`OM`](om.md) | Passage mode trace ( Option Monitor ) |
| [`OR`](or.md) | Droit de lecture générale ( Option Read ) |
| [`OT`](ot.md) | Positionne des tabulations ( Option Tab ) |

## Famille Z — Commandes diverses

| Commande | Fonction |
|---|---|
| [`ZA`](za.md) | ( Zap Append ) |
| [`ZB`](zb.md) | Tri en ordre décroissant |
| [`ZC`](zc.md) | Efface l'indicateur "buffer modifié // fichier associé" |
| [`ZD`](zd.md) | Suppression de buffer |
| [`ZG`](zg.md) | Exécution des commandes et ramassage dans un buffer, de la production destinée à la visualisation |
| [`ZI`](zi.md) | Passage en mode tabulation |
| [`ZK`](zk.md) | Copie lignes dans un buffer désigné à la suite ( Zap Kopie ) |
| [`ZL`](zl.md) | Conversion en minuscules ( Zap Lower ) |
| [`ZM`](zm.md) | Déplacement lignes dans un buffer à la suite ( Zap Move ) |
| [`ZO`](zo.md) | Passage en mode non tabulation |
| [`ZP`](zp.md) | Impression de buffer   ( Zap Print ) |
| [`ZR`](zr.md) | Positionnement sur un buffer affecté à un fichier ( Zap Read ) |
| [`ZS`](zs.md) | Tri en ordre croissant ( Zap Sort ) |
| [`ZT`](zt.md) | Traduction ( Traduct ) |
| [`ZU`](zu.md) | Conversion en majuscules ( Zap Upper ) |
| [`ZV`](zv.md) | Défait la commande V ( Zap Void ) |
| [`ZW`](zw.md) | Ecriture à la suite ( Zap Write) |

## Remarque sur `N|`

Le manuel DNB14A contient deux notices portant le nom `N|` :

1. calcul de la valeur absolue d’un registre ;
2. OU inclusif bit à bit.

Ces deux formes sont réunies dans [`npipe.md`](npipe.md).

## Source

Documentation extraite du manuel historique **DNB14A** et organisée pour la documentation française de FRED++.
