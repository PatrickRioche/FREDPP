============================================================
COMMANDES SPÉCIALES DE FREDPP
============================================================

Ces commandes appartiennent à l'environnement de développement et de diagnostic de FREDPP.
Elles ne sont pas des commandes historiques de FRED.

GESTION DU PROGRAMME

    :quit                    Quitter FREDPP
    :help                    Afficher cette aide, comme ?:

GESTION DES BUFFERS

    :buffers                 Lister les buffers et identifier le buffer courant
    :print                   Afficher le buffer courant avec les numéros de ligne
    :new <nom>               Créer ou sélectionner un buffer
    :use <nom>               Sélectionner un buffer existant
    :append <texte>          Ajouter une ligne au buffer courant
    :insert <n> <texte>      Insérer une ligne avant la ligne n
    :delete <n>              Supprimer la ligne n
    :flow <buffer>           Développer un buffer avec le moteur Flow

DIAGNOSTIC DU LANGAGE

    :lex <texte>             Afficher les jetons produits par le lexer
    :tokens <texte>          Parcourir et afficher le flux de jetons
    :address <adresse>       Analyser une adresse FRED
    :command <commande>      Analyser une commande FRED
    :pattern <modèle>        Analyser un modèle FRED

AUTRES AIDES

    ?                        Afficher les commandes FRED implémentées
    ?:                       Afficher cette rubrique
    :help                    Afficher cette rubrique
    ?version                 Afficher la version, le commit Git et l'état des sources
