# Integration test: interactive ELIZA through \R and the bundled library path.
if(NOT DEFINED FREDPP_EXE OR NOT DEFINED ELIZA_PROCEDURE OR
   NOT DEFINED TEST_ROOT)
    message(FATAL_ERROR "FREDPP_EXE, ELIZA_PROCEDURE and TEST_ROOT required")
endif()
if(NOT EXISTS "${ELIZA_PROCEDURE}")
    message(FATAL_ERROR "Missing ELIZA procedure: ${ELIZA_PROCEDURE}")
endif()

string(RANDOM LENGTH 10 ALPHABET 0123456789abcdef suffix)
set(root "${TEST_ROOT}/eliza-${suffix}")
file(MAKE_DIRECTORY "${root}/library")
file(COPY "${ELIZA_PROCEDURE}" DESTINATION "${root}/library")
file(WRITE "${root}/conversation.txt"
    "bonjour\nJe suis fatigue.\nJe veux partir\n"
    "je pense que cela fonctionne\nj'ai besoin de vacances\n"
    "j'ai peur\nmon ordinateur\n"
    "quelque chose d'inconnu\nune autre chose\nencore autre chose\n"
    "fin\n")

execute_process(
    COMMAND "${CMAKE_COMMAND}" -E env "FREDPP_INIT="
        "${FREDPP_EXE}" eliza
    WORKING_DIRECTORY "${root}"
    INPUT_FILE "${root}/conversation.txt"
    RESULT_VARIABLE status
    OUTPUT_VARIABLE output
    ERROR_VARIABLE errors
    TIMEOUT 20
)
string(REPLACE "\r\n" "\n" output "${output}")
string(REPLACE "\r\n" "\n" errors "${errors}")

if(NOT "${status}" STREQUAL "0" OR NOT "${errors}" STREQUAL "")
    message(FATAL_ERROR
        "ELIZA failed: ${status}\nstdout: ${output}\nstderr: ${errors}")
endif()

foreach(expected IN ITEMS
    "Bonjour. De quoi aimeriez-vous parler ?"
    "Depuis quand etes-vous fatigue ?"
    "Pourquoi voulez-vous partir ?"
    "Qu'est-ce qui vous fait penser que cela fonctionne ?"
    "Pourquoi avez-vous besoin de vacances ?"
    "Qu'est-ce qui vous fait peur ?"
    "Les ordinateurs occupent-ils une place importante"
    "Pouvez-vous m'en dire davantage ?"
    "Pourquoi dites-vous cela ?"
    "En quoi cela vous semble-t-il important ?"
    "Au revoir. Merci pour cet echange."
)
    string(FIND "${output}" "${expected}" pos)
    if(pos EQUAL -1)
        message(FATAL_ERROR
            "Missing ELIZA reply [${expected}]\nOutput:\n${output}")
    endif()
endforeach()

file(REMOVE_RECURSE "${root}")
message(STATUS "ELIZA interactive, rewriting, keyword and fallback checks passed")
