# N: - Affectation de valeur à un registre ( Numeric ).

> Conversion initiale depuis DNB14A.docx. À restructurer.

```text
N: - Affectation de valeur à un registre ( Numeric ).
Syntaxe :
(.)N<nom de registre>:<nombre>
Exemples :
n(reg1):10
n(reg2):20fnn(reg1):10n(reg2):20b(buf)n(nbbuf):$      n(nbbuf)p4
Descriptions :
La commande N: permet l'affectation d'une valeur numérique à un registre. On peut aussi utiliser des caractères spéciaux associés à des registres comme nombre : "#" pour la valeur du registre compteur, "." pour le numéro de la ligne courante, "$" pour le numéro de la dernière ligne courante.
Options :

N| - Valeur absolue d'un registre.
Syntaxe :
N<nom registre>|
Exemples :
n(reg1):-5
n(reg1)|    n(reg1)p5
Descriptions :
La commande N| calcul la valeur absolue du registre et retourne le résultat dans ce registre.
Options :

N+ - Addition d'une valeur dans un registre.
Syntaxe :
N<nom registre>+<nombre>
Exemples :
n(reg1):6
n(reg1)+6 n(reg1)p12n(reg2):4n(reg2)+\N(reg1) n(reg2)p16
Descriptions :
La commande N+ additionne une valeur au contenu d'un registre et retourne le résultat dans ce registre.
Options :
```
