# ZC - Efface l'indicateur "buffer modifié // fichier associé".

> Conversion initiale depuis DNB14A.docx. À restructurer.

```text
ZC - Efface l'indicateur "buffer modifié // fichier associé".
Syntaxe :
ZC
Exemples :
b(buf)
fb(buf) 4,4 /fred/parexb ? BCD cardzcfb(buf) 4,4 /fred/parexb BCD card
Descriptions :
La commande ZC efface l'indicateur "buffer modifié par rapport au fichier associé".
Le registre de condition est chargé à "FAUX" si l'indicateur n'était pas positionné.
Le registre de condition est positionné à "VRAI" (True) si l'indicateur était positionné, c'est à dire si le buffer n'était pas l'exact reflet du fichier associé.
Options :
```
