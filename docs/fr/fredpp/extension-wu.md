============================================================
WU — ÉCRITURE UTF-8 (EXTENSION FREDPP)
============================================================

WU est une extension moderne de FREDPP. Elle ne fait pas partie du FRED
historique.

SYNTAXE
------------------------------------------------------------

    WU <fichier>
    (.,.)WU <fichier>

DESCRIPTION
------------------------------------------------------------

    WU écrit le texte en UTF-8 sans marque BOM.

    W conserve l'encodage du fichier associé au buffer. Pour un nouveau
    fichier sans encodage connu, W utilise UTF-8.

    WA force l'écriture ASCII et refuse les caractères qui ne peuvent pas
    être représentés en ASCII.

    WB correspond au format historique GCOS/BCD. Cette forme est reconnue,
    mais elle n'est pas implémentée dans FREDPP.

EXEMPLES
------------------------------------------------------------

    WU rapport.txt
    *WU "rapport final.txt"

VOIR AUSSI
------------------------------------------------------------

    ?w      Commande historique W et forme WA
    ?r      Lecture d'un fichier
