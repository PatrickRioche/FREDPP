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
    # Optional 4th argument selects solo; no argument preserves two players.
    set(game_arguments tictactoe)
    if(ARGC GREATER 3)
        list(APPEND game_arguments "${ARGV3}")
    endif()
    execute_process(
        COMMAND "${CMAKE_COMMAND}" -E env "FREDPP_INIT="
            "${FREDPP_EXE}" ${game_arguments}
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

# Regression from interactive user report: repeated occupied choices must
# stay responsive, keep the same player and accept FIN cleanly.
run_game("occupied_twice" "1\n2\n1\n2\nFIN\n" occupied_output)
string(REGEX MATCHALL "Cette case est deja occupee" occupied_matches "${occupied_output}")
list(LENGTH occupied_matches occupied_count)
if(NOT occupied_count EQUAL 2)
    message(FATAL_ERROR
        "Expected two occupied warnings, got ${occupied_count}:\n${occupied_output}")
endif()
require_text("${occupied_output}" "Partie interrompue." "occupied quit")

# Regression from interactive user report: X wins the 1-5-9 diagonal.
run_game("x_diagonal" "1\n4\n2\n3\n5\n6\n9\n" x_diagonal_output)
require_text("${x_diagonal_output}" "Le joueur X a gagne." "x_diagonal winner")
require_text("${x_diagonal_output}" "| O | X | O |" "x_diagonal middle board")


# Solo mode uses a precomputed perfect Minimax strategy, no C++ AI.
# X opens in corner: O always takes the centre, then waits for X.
run_game("solo_open" "1\nFIN\n" solo_open "solo")
require_text("${solo_open}" "Mode solo : vous jouez X" "solo chosen")
require_text("${solo_open}" "FREDPP (O) choisit la case 5." "solo takes centre")
require_text("${solo_open}" "Partie interrompue." "solo quit")

# If X takes the centre first, the perfect opponent takes a corner.
run_game("solo_centre" "5\nFIN\n" solo_centre "solo")
require_text("${solo_centre}" "FREDPP (O) choisit la case 1." "solo corners")

# Human mistakes permit O to win; no extra human inputs are required.
run_game("solo_win" "1\n2\n4\n" solo_win "solo")
require_text("${solo_win}" "Le joueur O a gagne." "solo computer win")

# Human perfect replies force a draw; the game must terminate.
run_game("solo_draw" "1\n2\n7\n6\n8\n" solo_draw "solo")
require_text("${solo_draw}" "Match nul : toutes les cases sont occupees." "solo draw")

# Invalid and occupied user choices must not consume an automatic turn.
run_game("solo_bad_input" "1\n5\n0\nFIN\n" solo_bad "solo")
require_text("${solo_bad}" "Cette case est deja occupee" "solo occupied")
require_text("${solo_bad}" "Choix invalide" "solo invalid")
require_text("${solo_bad}" "Partie interrompue." "solo fin")

# Reject unsupported mode arguments without reading from stdin.
run_game("bad_mode" "" bad_mode "ordinateur")
require_text("${bad_mode}" "Mode inconnu." "unsupported mode")

file(REMOVE_RECURSE "${root}")
message(STATUS "Tic-tac-toe 2-player and unbeatable solo Minimax checks passed")
