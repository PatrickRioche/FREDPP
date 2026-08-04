# J - Abandon du reste de la ligne ( Jump ).

> Conversion initiale depuis DNB14A.docx. À restructurer.

```text
J - Abandon du reste de la ligne ( Jump ).
Syntaxe :
J[T|F]
Exemples :
b(buf)
*t/^Ajout/ jf j(YaAjout)....................j(FinSi)@(YaAjout)....................@(Finsi)
Descriptions :
La commande JT abandonne le reste de la ligne si le registre de condition est "VRAI" (True).
La commande JF abandonne le reste de la ligne si le registre de condition est "FAUX" (False).
Options :
```
