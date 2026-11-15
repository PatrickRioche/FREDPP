# Integration regression: real interactive 2-player tic-tac-toe using \R.
if(NOT DEFINED FREDPP_EXE OR NOT DEFINED TICTACTOE_PROCEDURE OR
   NOT DEFINED TEST_ROOT)
    message(FATAL_ERROR
        "FREDPP_EXE, TICTACTOE_PROCEDURE and TEST_ROOT are required")
endif()
if(NOT EXISTS "${TICTACTOE_PROCEDURE}")
    message(FATAL_ERROR "Missing game procedure: ${TICTACTOE_PROCEDURE}")
endif()

string(RANDOM LENGTH 10 ALPHABET 0123456789abcdef suffix)
set(root "${TEST_ROOT}/tictactoe-${suffix}")
file(MAKE_DIRECTORY "${root}/library")
file(COPY "${TICTACTOE_PROCEDURE}" DESTINATION "${root}/library")

function(run_game name input_text output_variable)
    file(WRITE "${root}/${name}.txt" "${input_text}")
    execute_process(
        COMMAND "${CMAKE_COMMAND}" -E env "FREDPP_INIT="
            "${FREDPP_EXE}" tictactoe
        WORKING_DIRECTORY "${root}"
        INPUT_FILE "${root}/${name}.txt"
        RESULT_VARIABLE status
        OUTPUT_VARIABLE stdout
        ERROR_VARIABLE stderr
        TIMEOUT 20
    )
    string(REPLACE "\r\n" "\n" stdout "${stdout}")
    string(REPLACE "\r\n" "\n" stderr "${stderr}")
    if(NOT "${status}" STREQUAL "0" OR NOT "${stderr}" STREQUAL "")
        message(FATAL_ERROR
            "Tic-tac-toe [${name}] failed: ${status}\n"
            "stdout:\n${stdout}\nstderr:\n${stderr}")
    endif()
    set(${output_variable} "${stdout}" PARENT_SCOPE)
endfunction()

function(require_text output expected scenario)
    string(FIND "${output}" "${expected}" found_at)
    if(found_at EQUAL -1)
        message(FATAL_ERROR
            "Tic-tac-toe [${scenario}] expected [${expected}], got:\n${output}")
    endif()
endfunction()

# X wins first row: X=1,2,3; O=4,5.
run_game("x_row" "1\n4\n2\n5\n3\n" x_output)
require_text("${x_output}" "| X | X | X |" "x_row board")
require_text("${x_output}" "Le joueur X a gagne." "x_row winner")

# O wins diagonal: O=1,5,9; X=2,3,4.
run_game("o_diagonal" "2\n1\n3\n5\n4\n9\n" o_output)
require_text("${o_output}" "Le joueur O a gagne." "o_diagonal winner")
require_text("${o_output}" "| O | X | X |" "o_diagonal board")

# Draw with full board and no winning alignment.
run_game("draw" "1\n2\n3\n5\n4\n6\n8\n7\n9\n" draw_output)
require_text("${draw_output}" "Match nul : toutes les cases sont occupees." "draw")

# Invalid input and occupied cell must not advance O's turn.
run_game("invalid" "0\n1\n1\nFIN\n" invalid_output)
require_text("${invalid_output}" "Choix invalide" "invalid entry")
require_text("${invalid_output}" "Cette case est deja occupee" "occupied cell")
require_text("${invalid_output}" "Joueur O - case" "unchanged turn")
require_text("${invalid_output}" "Partie interrompue." "quit")

file(REMOVE_RECURSE "${root}")
message(STATUS "Tic-tac-toe X/O wins, draw, input guards and quit passed")
