# W - Ecriture dans un fichier ( Write ).

> Conversion initiale depuis DNB14A.docx. À restructurer.

```text
W - Ecriture dans un fichier ( Write ).
Syntaxe :
(.,.)W[X][A|B][<nom de fichier>]<nl>
Exemples :
b(buf)
fb(buf) 4,4 /fred/parex ?w4,123 b(buf) /fred/parexwa4,123 b(buf) /fred/parexwb4,123 b(buf) /fred/parex BCD cardwa /fred/parexa4,123 b(buf) /fred/parexawb /fred/parexb4,123 b(buf) /fred/parexb BCD card
Descriptions :
La commande W écrit le contenu du buffer courant dans le fichier associé au buffer.
La commande WX imprime les statistiques d'écriture (nombre de blocs, de lignes, et de caractères écrits) même si la commande est exécutée à partir d'un buffer. 
La commande WA écrit en ASCII; WB écrit en BCD. Si ni A, ni B, ne sont spécifiés, le fichier est écrit dans le même format que le fichier associé. S'il n'y a pas de fichier associé, l'écriture se fait en ASCII. L'adresse de la ligne courante "." n'est pas modifiée par l'exécution d'une commande W et le nombre de blocs écrits est chargé dans le registre compteur ( # ).
Options :
```
