# T - Test de conformité à un modèle ( Test ).

> Conversion initiale depuis DNB14A.docx. À restructurer.

```text
T - Test de conformité à un modèle ( Test ).
Syntaxe :
(.,.)T[~]/<modèle>/
Exemples :
b(buf)
1pAjout  ligne de texte numero 11t/^ajout/ jf jm/Ya ajout en debut de ligne/Ya ajout en debut de ligne1t~/colonne/ jf jm/Ya pas colonne dans la ligne/Ya pas colonne dans la ligne
Descriptions :
La commande T permet de tester l'existence d'un modèle dans une ligne ou un groupe de lignes, dans ce cas le registre de condition est chargé à "VRAI" (True) si le modèle est trouvé, sinon il est mis à "FAUX" (False).
Si "~" précède <modèle>, le registre de condition est mis  à "FAUX" (False) si le modèle est trouvé, ou mis à "VRAI" (True) si le modèle n'est pas trouvé.
Options :
```
