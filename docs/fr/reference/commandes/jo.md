# JO - Saut hors du buffer courant ( Jump Out ).

> Conversion initiale depuis DNB14A.docx. À restructurer.

```text
JO - Saut hors du buffer courant ( Jump Out ).
Syntaxe :
JO[T|F]
Exemples :
b(exec) a
>b(buf) *t/^Ajout/ jf jm/Ya Ajout/ jojm/Ya pas Ajout/>\f\B(exec)Ya Ajout
Descriptions :
La commande JO permet d'abandonner l'exécution du buffer en cours ou l'abandon d'une itération dans une commande G ou U en fonction de l'indicateur booléen.
Options :
```
