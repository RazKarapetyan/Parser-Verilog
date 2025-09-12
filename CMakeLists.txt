cmake_minimum_required(VERSION 3.9)

project(Parser-Verilog LANGUAGES CXX)

# --- Choose specific toolchain locations -------------------------------
# EDIT these to your installs (examples shown)
set(MY_FLEX_BIN   "/work/jenkins/rhel7/stable/tools/bin/flex")
set(MY_BISON_BIN  "/work/jenkins/rhel7/stable/tools/bin/bison")
set(MY_FLEX_INC   "/work/jenkins/rhel7/stable/tools/include")   # contains FlexLexer.h

# Make CMake’s find_package() pick these binaries
set(FLEX_EXECUTABLE  "${MY_FLEX_BIN}"  CACHE FILEPATH "flex to use"  FORCE)
set(BISON_EXECUTABLE "${MY_BISON_BIN}" CACHE FILEPATH "bison to use" FORCE)

# --- Usual compiler flags ----------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-Wall -O2)

# --- Headers (put your Flex include BEFORE system includes) ------------
# If you vendor or pin a specific FlexLexer.h:
include_directories(BEFORE "${MY_FLEX_INC}")
include_directories(${PROJECT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} parser-verilog)

# --- Flex & Bison -------------------------------------------------------
find_package(BISON REQUIRED)
find_package(FLEX  REQUIRED)

BISON_TARGET(verilog_parser
  ${PROJECT_SOURCE_DIR}/parser-verilog/verilog_parser.yy
  ${CMAKE_CURRENT_BINARY_DIR}/verilog_parser.tab.cc)

FLEX_TARGET(verilog_lexer
  ${PROJECT_SOURCE_DIR}/parser-verilog/verilog_lexer.l
  ${CMAKE_CURRENT_BINARY_DIR}/verilog_lexer.yy.cc)

ADD_FLEX_BISON_DEPENDENCY(verilog_lexer verilog_parser)

# --- Example targets ----------------------------------------------------
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/example)

add_executable(sample_parser
  ${PROJECT_SOURCE_DIR}/example/sample_parser.cpp
  ${FLEX_verilog_lexer_OUTPUTS}
  ${BISON_verilog_parser_OUTPUTS})

add_executable(ot_parser
  ${PROJECT_SOURCE_DIR}/example/ot_parser.cpp
  ${FLEX_verilog_lexer_OUTPUTS}
  ${BISON_verilog_parser_OUTPUTS})

# Ensure our custom Flex include comes first for these targets too
target_include_directories(sample_parser BEFORE PRIVATE "${MY_FLEX_INC}")
target_include_directories(ot_parser     BEFORE PRIVATE "${MY_FLEX_INC}")