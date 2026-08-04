# NL - Affectation du nombre de caractère au registre.

> Conversion initiale depuis DNB14A.docx. À restructurer.

```text
NL - Affectation du nombre de caractère au registre.
Syntaxe :
ld,lfN<nom registre>L
Exemples :
b(buf)
1,$n(reg1)l  n(reg1)p76
Descriptions :
La commande NL affecte le nombre de caractères total des lignes désignées au registre.
Options :
OI( - Buffer entre parenthèses ( Option Input )
Syntaxe :
O[+|-]I(
Exemples :
b(buf)
b0? buff/reg name invalido-i(b0
Descriptions :
L'option OI( spécifie si oui ou non les parenthèses sont obligatoires pour les noms de buffer d'un seul caractère.
Options :
Valeur implicite O+I(.
```
