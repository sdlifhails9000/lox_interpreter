cmake_minimum_required(VERSION 3.5.0)

project("lox"
	DESCRIPTION "An interpreter for the lox programming langugage."
	LANGUAGES "C")

set(CMAKE_C_STANDARD "11")
set(CMAKE_C_STANDARD_REQUIRED True)

add_executable("lox" "main.c")

target_include_directories("lox" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

target_sources("lox"
	PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/token.c"
	PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/tokenizer.c"
	PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/parser.c"
	PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/logging.c"
	PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/map.c"
	PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/expr.c"
	PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/ast_printer.c"
)
