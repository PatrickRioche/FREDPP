# N= - Test d'égalité d'une valeur avec un registre.

> Conversion initiale depuis DNB14A.docx. À restructurer.

```text
N= - Test d'égalité d'une valeur avec un registre.
Syntaxe :
N<nom registre>=<nombre>
Exemples :
n(reg1):28
n(reg1)=0 j(egal0)t....................@(egal0)
Descriptions :
La commande N= effectue un test d'égalité entre un nombre et le contenu d'un registre, et positionne l'indicateur booléen à "VRAI" (True) ou "FAUX" (False).
Options :

N< - Test d'infériorité d'une valeur avec un registre.
Syntaxe :
N<nom registre><<nombre>
Exemples :
n(reg1):1
n(reg1)<0 j(inf0)t....................@(inf0)
Descriptions :
La commande N< effectue un test d'infériorité entre un nombre et le contenu d'un registre, et positionne l'indicateur booléen à "VRAI" (True) ou "FAUX" (False).
Options :

N> - Test de supériorité d'une valeur avec un registre.
Syntaxe :
N<nom registre>><nombre>
Exemples :
n(reg1):1
n(reg1)>0 j(sup0)t....................@(sup0)
Descriptions :
La commande N> effectue un test de supériorité entre un nombre et le contenu d'un registre, et positionne l'indicateur booléen à "VRAI" (True) ou "FAUX" (False).
Options :
```
