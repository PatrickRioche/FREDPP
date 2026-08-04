# R - Lecture de fichier ( Read ).

> Conversion initiale depuis DNB14A.docx. À restructurer.

```text
R - Lecture de fichier ( Read ).
Syntaxe :
R[X] [{d|d-f|d,w}] [<bl><liste fichiers>]<nl>
Exemples :
b(bufr)r library/fred/time
40,1291 b(bufr) library/fred/timeb(bufr)r library/fred/time jt jm/Pb de lecture du fichier/? buffer not empty
Descriptions :
La commande R permet de charger un fichier dans un buffer.
S'il s'agit d'une liste de fichier, Fred essaie de lire le premier fichier de la liste, passe au suivant si le fichier ne peut pas être lu ou si le nom du fichier comporte une erreur de syntaxe, continue sa recherche dans toute la liste jusqu'à ce qu'il trouve la fin de la liste ou un fichier qu'il puisse lire.
Options :
```
