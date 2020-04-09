cmake_minimum_required(VERSION 3.0)
project(Gamma C)

if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, default to Release")
    set(CMAKE_BUILD_TYPE "Release")
endif ()

# Nie chcemy oglądać komunikatów o procentowym postępie kompilacji.
set_property(GLOBAL PROPERTY RULE_MESSAGES OFF)

# Chcemy zobaczyć polecenia wywołane przez make.
#set(CMAKE_VERBOSE_MAKEFILE ON)

# Ustawiamy wspólne opcje kompilowania dla wszystkich wariantów projektu.
set(CMAKE_C_FLAGS "-std=c11 -Wall -Wextra")
# Domyślne opcje dla wariantów Release i Debug są sensowne.
# Jeśli to konieczne, ustawiamy tu inne.
#set(CMAKE_C_FLAGS_RELEASE "-O3 -std=c11 -Wall -Wextra")
#set(CMAKE_C_FLAGS_DEBUG "-g")

# Wskazujemy pliki źródłowe.
set(SOURCE_FILES
        src/gamma.c
        src/gamma.h
        src/gamma_test.c
        src/player.h
        src/vector.c
        src/vector.h
        src/gamma_struct.h
        src/gamma_struct.c
        src/gamma_test2.c
        src/board.h
        src/board.c)

# Wskazujemy plik wykonywalny.
add_executable(gamma ${SOURCE_FILES})

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


if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    message("debug mode")
    set(CMAKE_C_FLAGS_DEBUG "-g -I ${CMAKE_CURRENT_SOURCE_DIR}/src/")

#    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/debug/bin )
    enable_testing()
    file(GLOB_RECURSE files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "tests/*.c")

    set(CTEST_PARALLEL_LEVEL 4)
    set(SOURCE_FILES_TESTING
            src/gamma.c
            src/gamma.h
            src/player.h
            src/gamma_struct.h
            src/vector.c
            src/vector.h
            src/gamma_struct.c
            src/board.h
            src/board.c)

    add_library(source OBJECT ${SOURCE_FILES_TESTING})
    foreach (file ${files})
        string(REGEX REPLACE "(^.*/|\\.[^.]*$)" "" file_without_ext ${file})
        string(REGEX REPLACE "tests/z2/|/[^/]*\\.c$" "" directory ${file})
        add_executable(${file_without_ext} $<TARGET_OBJECTS:source> ${file})
        add_test(${file_without_ext} ${file_without_ext})
        set_tests_properties(${file_without_ext} PROPERTIES LABELS "${directory}")

    endforeach ()
endif (CMAKE_BUILD_TYPE STREQUAL "Debug")
