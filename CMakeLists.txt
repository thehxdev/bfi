cmake_minimum_required(VERSION 3.1...3.28)

project(
  bfi
  VERSION 1.0.0
  LANGUAGES C)


set(CMAKE_C_STANDARD 99)
add_compile_options(-Wall -Wextra -Wno-unused-result)

if (OPTIMIZE)
    add_compile_options(-O2)
else()
    add_compile_options(-Og -ggdb)
endif()


set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")
if(CMAKE_EXPORT_COMPILE_COMMANDS)
  set(CMAKE_C_STANDARD_INCLUDE_DIRECTORIES 
      ${CMAKE_C_IMPLICIT_INCLUDE_DIRECTORIES})
endif()


# BFI
set(SRC_DIR "./src")
set(SRC_FILES
    ${SRC_DIR}/main.c
    ${SRC_DIR}/bfi.c
    ${SRC_DIR}/rand.c
    ${SRC_DIR}/scanner.c
)
add_executable(bfi ${SRC_FILES})
if (NON_STD_CMDS)
    target_compile_definitions(bfi PUBLIC NON_STD_CMDS)
endif()
