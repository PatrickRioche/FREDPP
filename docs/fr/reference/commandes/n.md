# N- - Soustraction d'une valeur dans un registre.

> Conversion initiale depuis DNB14A.docx. À restructurer.

```text
N- - Soustraction d'une valeur dans un registre.
Syntaxe :
N<nom registre>-<nombre>
Exemples :
n(reg1):10
n(reg1)-6 n(reg1)p4n(reg2):10n(reg2)-\N(reg1) n(reg2)p6
Descriptions :
La commande N- soustrait une valeur au contenu d'un registre et retourne le résultat dans ce registre.
Options :
N* - Multiplication d'une valeur dans un registre.
Syntaxe :
N<nom registre>*<nombre>
Exemples :
n(reg1):6
n(reg1)*6 n(reg1)p36n(reg2):2n(reg2)*\N(reg1) n(reg2)p72
Descriptions :
La commande N* multiplie un registre par une valeur ou le contenu d'un autre registre et retourne le résultat dans ce registre.
Options :

N/ - Division entière d'une valeur dans un registre.
Syntaxe :
N<nom registre>/<nombre>
Exemples :
n(reg1):6
n(reg1)/2 n(reg1)p3n(reg2):36n(reg2)*\N(reg1) n(reg2)p12
Descriptions :
La commande N/ divise un registre par une valeur ou le contenu d'un autre registre et retourne la valeur entière du résultat dans ce registre.
Options :

N% - Reste de la division d'une valeur dans un registre.
Syntaxe :
N<nom registre>%<nombre>
Exemples :
n(reg1):20
n(reg1)%8 n(reg1)p4n(reg2):20n(reg2)*\N(reg1) n(reg2)p0
Descriptions :
La commande N% retourne le reste de la division entière d'une valeur par le contenu d'une valeur ou d'un autre registre et met le résultat dans ce registre.
Options :

N~ - Complément bit à bit d'une valeur d'un registre.
Syntaxe :
N<nom registre>~
Exemples :
n(reg1):1
n(reg1)~    n(reg1)p-2
Descriptions :
La commande N~ prend le complément bit à bit de la valeur du registre et retourne le résultat dans ce registre.
Options :

N& - ET bit à bit d'une valeur dans un registre.
Syntaxe :
N<nom registre>&<nombre>
Exemples :
n(reg1):-1
n(reg1)&7   n(reg1)p7
Descriptions :
La commande N& fait un ET bit à bit du contenu du registre avec une valeur et met le résultat dans ce registre.
Options :

N| - OU inclus bit à bit d'une valeur dans un registre.
Syntaxe :
N<nom registre>|<nombre>
Exemples :
n(reg1):7
n(reg1)|8   n(reg1)p15
Descriptions :
La commande N| fait un OU inclusif bit à bit du contenu du registre avec une valeur et met le résultat dans ce registre.
Options :

N^ - OU exclusif bit à bit d'une valeur dans un registre.
Syntaxe :
N<nom registre>^<nombre>
Exemples :
n(reg1):15
n(reg1)^8   n(reg1)p7
Descriptions :
La commande N^ fait un OU exclusif bit à bit du contenu du registre avec une valeur et met le résultat dans ce registre.
Options :

N} - Décalage à droite bit à bit du nombre.
Syntaxe :
N<nom registre>}<nombre>
Exemples :
n(reg1):28
n(reg1)}2   n(reg1)p7
Descriptions :
La commande N} effectue un décalage bit à bit vers la droite sur le contenu du registre indiqué, autant de fois que spécifie le <nombre>.
Options :

N{ - Décalage à gauche bit à bit du nombre.
Syntaxe :
N<nom registre>{<nombre>
Exemples :
n(reg1):7
n(reg1){2   n(reg1)p28
Descriptions :
La commande N{ effectue un décalage bit à bit vers la gauche sur le contenu du registre indiqué, autant de fois que spécifie le <nombre>.
Options :
```
