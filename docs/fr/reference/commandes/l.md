# L - Liste le contenu d'un fichier ( List ).

> Conversion initiale depuis DNB14A.docx. À restructurer.

```text
L - Liste le contenu d'un fichier ( List ).
Syntaxe :
L[{d|d-f|d,w}][<bl><nom de fichier>] 
Exemples :
l2-4 /fred/parexemple
$      program fred$      limits  10,40k$      prmfl   **,q,r,cmdlib/etc/qstarl2,2 /fred/parexemple$      program fred$      limits  10,40k
Descriptions :
La commande L permet de lister un fichier au terminal. Cette commande ne modifie par l'adresse des pointeurs du buffer courant.
Options :
d	: Lister le fichier à partir de l'adresse de (d)ébut de ligne.
d-f	: Lister le fichier de l'adresse de (d)ébut de ligne à l'adresse de (f)in de ligne.
d,w	: Lister le fichier à partir de l'adresse de (d)ébut de ligne sur une fenêtre de (w) lignes.
```
