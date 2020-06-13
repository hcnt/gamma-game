cmake_minimum_required(VERSION 3.0)
project(Gamma C)

if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, default to Release")
    set(CMAKE_BUILD_TYPE "Release")
endif ()

# Nie chcemy oglądać komunikatów o procentowym postępie kompilacji.
set_property(GLOBAL PROPERTY RULE_MESSAGES OFF)

# Chcemy zobaczyć polecenia wywołane przez make.
set(CMAKE_VERBOSE_MAKEFILE ON)

# Ustawiamy wspólne opcje kompilowania dla wszystkich wariantów projektu.
set(CMAKE_C_FLAGS "-std=c11 -Wall -Wextra")
# Domyślne opcje dla wariantów Release i Debug są sensowne.
# Jeśli to konieczne, ustawiamy tu inne.
#set(CMAKE_C_FLAGS_RELEASE "-O3 -std=c11 -Wall -Wextra")
#set(CMAKE_C_FLAGS_DEBUG "-O3 -g")

set(SOURCE_FILES_BACKEND
        src/gamma_backend/gamma.c
        src/gamma_backend/gamma.h
        src/gamma_backend/player.h
        src/gamma_backend/vector.c
        src/gamma_backend/vector.h
        src/gamma_backend/gamma_struct.h
        src/gamma_backend/gamma_struct.c
        src/gamma_backend/board.h
        src/gamma_backend/board.c)

# Wskazujemy pliki źródłowe.
set(SOURCE_FILES_PART2
        src/gamma_utils.h
        src/gamma_main.c
        src/gamma_interactive.c
        src/state.h src/batch_mode.c src/batch_mode.h src/gamma_utils.c src/batch_mode.c src/batch_mode.h)

# Wskazujemy plik wykonywalny.
add_executable(gamma ${SOURCE_FILES_PART2} ${SOURCE_FILES_BACKEND})
add_executable(test ${SOURCE_FILES_BACKEND} src/gamma_backend/gamma_test.c)

# Dodajemy obsługę Doxygena: sprawdzamy, czy jest zainstalowany i jeśli tak to:
find_package(Doxygen)
if (DOXYGEN_FOUND)
    # Wskazujemy lokalizacją pliku konfiguracyjnego i podajemy jego docelową lokalizację w folderze, gdzie następuje kompilacja.
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
    # Dodajemy cel doc: użycie make doc spowoduje wywołanie doxygena z odpowiednim plikiem konfiguracyjnym w folderze kompilacji.
    # Na wyjście zostanie wypisany odpowiedni komentarz.
    add_custom_target(doc
            ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen"
            )
endif (DOXYGEN_FOUND)

#if (CMAKE_BUILD_TYPE STREQUAL "Debug")
#    message("debug mode")
#    set(CMAKE_C_FLAGS_DEBUG "-g -I ${CMAKE_CURRENT_SOURCE_DIR}/src/gamma_backend")
#
#    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/debug )
#    enable_testing()
#    file(GLOB_RECURSE files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "tests/z2/part3/*.c")
#
#    set(CTEST_PARALLEL_LEVEL 4)
#    set(SOURCE_FILES_TESTING
#            src/gamma_backend/gamma.c
#            src/gamma_backend/gamma.h
#            src/gamma_backend/player.h
#            src/gamma_backend/gamma_struct.h
#            src/gamma_backend/vector.c
#            src/gamma_backend/vector.h
#            src/gamma_backend/gamma_struct.c
#            src/gamma_backend/board.h
#            src/gamma_backend/board.c)
#
#    add_library(source OBJECT ${SOURCE_FILES_TESTING})
##    foreach (file ${files})
##        message(${file})
##        string(REGEX REPLACE "(^.*/|\\.[^.]*$)" "" file_without_ext ${file})
##        string(REGEX REPLACE "tests/z2/part3/engine_tests|/[^/]*\\.c$" "" directory ${file})
##        add_executable(${file_without_ext} $<TARGET_OBJECTS:source> ${file})
##        add_test(${file_without_ext} ${file_without_ext})
##        set_tests_properties(${file_without_ext} PROPERTIES LABELS "${directory}")
##
##    endforeach ()
#endif (CMAKE_BUILD_TYPE STREQUAL "Debug")
