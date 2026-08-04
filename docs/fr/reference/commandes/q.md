# Q - Arrêt ( Quit )

> Conversion initiale depuis DNB14A.docx. À restructurer.

```text
Q - Arrêt ( Quit )
Syntaxe :
Q<nl>				 Arrêt.
QQ<nl> 			Arrêt immédiat.
Q!<commande TSS><nl>	 Arrêt avec appel à un autre sous-système TSS.
Exemples :
fred
q*fredq!bye$$ 0400 AU REVOIR
Descriptions :
La commande Q permet l'arrêt sous réserve qu'il n'y ait pas de buffers modifiés par rapport à leurs fichiers associés respectifs.
La commande QQ permet l'arrêt immédiat, sans considération de l'état des buffers (modifiés ou non, par rapport à leurs fichiers associés respectifs).
La commande Q! permet l'arrêt immédiat de FRED, sans considération pour l'état des buffers, en enchaînant sur un autre sous-système TSS.
Options :
```
