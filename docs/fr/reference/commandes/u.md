# U - Répétition jusqu'à ... ( Until ).

> Conversion initiale depuis DNB14A.docx. À restructurer.

```text
U - Répétition jusqu'à ... ( Until ).
Syntaxe :
(.)U[<n>][T|F|E]<commandes><nl> 
Exemples :
o-i\F (bufu) a Ajout de ligne dans buffer
b(bufa)u3 $za(bufu)*Ajout de ligne dans bufferAjout de ligne dans bufferAjout de ligne dans bufferu1e 20dn(err):#=-6 jf jm/La ligne n'existe pas!/La ligne n'existe pas!
Descriptions :
La commande "U<n>" répète les commandes  n  fois, les commandes listées sont copiées dans un buffer secret, qui  est ensuite  exécuté à plusieurs reprises, jusqu'à ce que la condition d'arrêt soit satisfaite. La commande "UT" répète les commandes jusqu'à ce que le registre de condition soit "VRAI" à la fin d'une itération. La commande "UF" répète les commandes jusqu'à ce que le registre de condition soit "FAUX" à la fin d'une itération. La commande "UE" répète les commandes jusqu'à ce qu'une erreur se produise. - si <n> est spécifié avec "UT", "UF", ou "UE", les commandes sont répétées  n  fois, à moins qu'une condition ne soit satisfaite avant ( Limite <n> = 262143 ).
Options :
Liste des erreurs possibles :
 0   -- internal error			-10   -- file error
-1   -- context search failed		-11   -- too many alternatives
-2   -- unknown command		-12   -- internal table overflow
-3   -- syntax error in pattern 		-14   -- callss not allowed
-4   -- incorrectly constructed address   -15   -- add/cmd conflict
-5   -- address wrap around		-16   -- cannot grow workfile
-6   -- value out of range			-17   -- invalid parameter
-7   -- buff/reg name invalid		-18   -- missing closing delimiter
-9   -- command syntax error		-19   -- BREAK key was pushed
```
