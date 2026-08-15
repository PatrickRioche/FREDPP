============================================================
COMMANDES FREDPP
============================================================

Ces commandes et rubriques sont propres à FREDPP.
Elles ne font pas partie du langage historique FRED.

INSPECTION DES BUFFERS

    :print                   Afficher le buffer courant avec les numéros de ligne
    :flow <buffer>           Développer un buffer avec le moteur Flow

DIAGNOSTIC DU LANGAGE

    :lex <texte>             Afficher les jetons produits par le lexer
    :tokens <texte>          Parcourir et afficher le flux de jetons
    :address <adresse>       Analyser une adresse FRED
    :command <commande>      Analyser une commande FRED
    :pattern <modèle>        Analyser un modèle FRED

AIDE ET INFORMATIONS FREDPP

    ?:                       Afficher cette rubrique
    ?version                 Afficher la version, le commit Git et l'état des sources
    ?wu                      Afficher l'extension UTF-8 de la commande W
    ?procedure               Afficher l'aide des procédures FREDPP et de \B(buffer)

Pour l'aide des commandes FRED, utiliser ? ou ?commande.
