# ZK - Copie lignes dans un buffer désigné à la suite ( Zap Kopie ).

> Conversion initiale depuis DNB14A.docx. À restructurer.

```text
ZK - Copie lignes dans un buffer désigné à la suite ( Zap Kopie ).
Syntaxe :
(.,.)ZK<nom de buffer>
Exemples :
b(buf)
1zk(bufk)3zk(bufk)b(bufk)*Ajout  ligne de texte numero 1Ajout  ligne de texte numero 3
Descriptions :
 La commande ZK copie les lignes spécifiées du buffer courant dans le buffer désigné, après la ligne courante "." du buffer désigné.
Le <nom de buffer> peut désigner le buffer courant, mais les lignes spécifiées ne peuvent alors inclure la ligne courante. Les adresses des lignes courantes "." des buffers émetteur et récepteur pointent sur la dernière ligne copiée, respectivement.
Options :
```
