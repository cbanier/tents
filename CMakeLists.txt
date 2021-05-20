cmake_minimum_required(VERSION 3.0.0)
project("game_text" C)

include(CTest)
enable_testing()

set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -std=c99 -g --coverage")

# sdl2 for linux
if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  include(FindPkgConfig) # pkg-config --list-all  | grep -i sdl2
  pkg_search_module(SDL2 REQUIRED sdl2)
  pkg_search_module(SDL2_IMG REQUIRED SDL2_image)
  pkg_search_module(SDL2_TTF REQUIRED SDL2_ttf)
  set(SDL2_ALL_LIBS ${SDL2_LIBRARIES} ${SDL2_IMG_LIBRARIES} ${SDL2_TTF_LIBRARIES})
  set(SDL2_ALL_INC ${SDL2_INCLUDE_DIRS} ${SDL2_IMG_INCLUDE_DIRS} ${SDL2_TTF_INCLUDE_DIRS})
  find_package(SDL2 REQUIRED)
  set(SDL2_ALL_INC ${SDL2_INCLUDE_DIRS})
  set(SDL2_ALL_LIBS ${SDL2_LIBRARIES} -lSDL2_ttf -lSDL2_image) # add extra SDL2 libraries
endif()

# sdl2 for mac
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(SDL2_ALL_INC ${SDL2_INCLUDE_DIRS} /usr/local/Cellar/sdl2_image/2.0.5/include/SDL2 /usr/local/Cellar/sdl2_ttf/2.0.15/include/SDL2)
  set(SDL2_ALL_LIBS -L/usr/local/lib  -lSDL2 -lSDL2_ttf -lSDL2_image) # add extra SDL2 libraries
endif()

# sdl2 message
message(STATUS "SDL2 include dir: ${SDL2_ALL_INC}")
message(STATUS "SDL2 all libraries: ${SDL2_ALL_LIBS}")

# compilation rules to game_text
add_library(game game.c game_aux.c game_ext.c game_prof.c queue.c game_tools.c)
add_executable(game_text game_text.c)
target_link_libraries(game_text game)

# compilation rules to game_random
add_executable(game_random game_random.c)
target_link_libraries(game_random game)

# compilation rules to game_solve
add_executable(game_solve game_solve.c)
target_link_libraries(game_solve game)

# compilation rules to game_sdl
find_package(SDL2 CONFIG REQUIRED)
include_directories(${SDL2_ALL_INC})
add_executable(game_sdl game_sdl.c model.c game.c game_aux.c game_ext.c game_prof.c queue.c game_tools.c)
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
target_link_libraries(game_sdl ${SDL2_ALL_LIBS} SDL2::SDL2main SDL2::SDL2-static)
endif()
target_link_libraries(game_sdl ${SDL2_ALL_LIBS} m)
target_link_libraries(game_sdl game)

## copy useful data files in build dir
foreach(file "resources/tree.png" "resources/tent.png" "resources/grass.png" "resources/0.png" "resources/1.png" 
  "resources/2.png" "resources/3.png" "resources/4.png" "resources/5.png" "resources/6.png" "resources/7.png" "resources/8.png" 
  "resources/9.png" "resources/help.png" "resources/next.png" "resources/Roboto-Regular.ttf")
  file(COPY ${file} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endforeach(file)

# compilation rules to tests
## by Corentin Banier
add_executable(game_test_cbanier game_test_cbanier.c)
add_test(test_cbanier_game_new ./game_test_cbanier game_new)
add_test(test_cbanier_game_new_empty ./game_test_cbanier game_new_empty)
add_test(test_cbanier_game_check_move ./game_test_cbanier game_check_move)
add_test(test_cbanier_game_check_move_2x2 ./game_test_cbanier game_check_move_2x2)
add_test(test_cbanier_game_play_move ./game_test_cbanier game_play_move)
add_test(test_cbanier_game_fill_grass_row ./game_test_cbanier game_fill_grass_row)
add_test(test_cbanier_game_fill_grass_col ./game_test_cbanier game_fill_grass_col)
add_test(test_cbanier_game_equal ./game_test_cbanier game_equal)
add_test(test_cbanier_game_default ./game_test_cbanier game_default)
target_link_libraries(game_test_cbanier game)

## by Alexandre Bousquet
add_executable(game_test_alebousquet game_test_alebousquet.c)
add_test(test_alebousquet_delete ./game_test_alebousquet game_delete)
add_test(test_alebousquet_get_current_nb_tents_row ./game_test_alebousquet game_get_current_nb_tents_row)
add_test(test_alebousquet_set_square ./game_test_alebousquet game_set_square)
add_test(test_alebousquet_get_square ./game_test_alebousquet game_get_square)
add_test(test_alebousquet_is_over ./game_test_alebousquet game_is_over)
add_test(test_alebousquet_restart ./game_test_alebousquet game_restart)
add_test(test_alebousquet_save ./game_test_alebousquet game_save)
target_link_libraries(game_test_alebousquet game)

## by Anass Bousseaden
add_executable(game_test_abousseaden game_test_abousseaden.c)
add_test(test_abousseaden_game_set_expected_nb_tents_row ./game_test_abousseaden game_set_expected_nb_tents_row)
add_test(test_abousseaden_game_set_expected_nb_tents_col ./game_test_abousseaden game_set_expected_nb_tents_col)
add_test(test_abousseaden_game_get_expected_nb_tents_col ./game_test_abousseaden game_get_expected_nb_tents_col)
add_test(test_abousseaden_game_get_expected_nb_tents_row ./game_test_abousseaden game_get_expected_nb_tents_row)
add_test(test_abousseaden_game_get_expected_nb_tents_all ./game_test_abousseaden game_get_expected_nb_tents_all)
add_test(test_abousseaden_game_default_solution ./game_test_abousseaden game_default_solution)
add_test(test_abousseaden_game_undo ./game_test_abousseaden game_undo)
add_test(test_abousseaden_game_redo ./game_test_abousseaden game_redo)
add_test(test_abousseaden_game_new_empty_ext ./game_test_abousseaden game_new_empty_ext)
add_test(test_abousseaden_game_new_ext ./game_test_abousseaden game_new_ext)
add_test(test_abousseaden_game_game_equal ./game_test_abousseaden game_equal)
add_test(test_abousseaden_game_game_restart ./game_test_abousseaden game_restart)
add_test(test_abousseaden_game_check_move_1 ./game_test_abousseaden game_check_move_1)
add_test(test_abousseaden_game_check_move_MANITAS ./game_test_abousseaden game_check_move_2)
add_test(test_abousseaden_game_game_memory ./game_test_abousseaden game_memory)
add_test(test_abousseaden_game_load ./game_test_abousseaden game_load)
add_test(test_abousseaden_game_solve ./game_test_abousseaden game_solve)
target_link_libraries(game_test_abousseaden game)

## by Baptiste Bédouret
add_executable(game_test_bbedouret game_test_bbedouret.c)
add_test(test_bbedouret_copy ./game_test_bbedouret game_copy)
add_test(test_bbedouret_print ./game_test_bbedouret game_print)
add_test(test_bbedouret_equal ./game_test_bbedouret game_equal)
add_test(test_bbedouret_get_current_nb_tents_all ./game_test_bbedouret game_get_current_nb_tents_all)
add_test(test_bbedouret_get_current_nb_tents_col ./game_test_bbedouret game_get_current_nb_tents_col)
target_link_libraries(game_test_bbedouret game)