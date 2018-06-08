cmake_minimum_required(VERSION 3.5)
project(cond_var)

find_package(Threads REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(EXISTS "${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json")
	execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different
		${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json
		${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json
	)
endif()

set(SOURCES
	src/main.cc
)

file(GLOB_RECURSE ALL_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cc ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
add_custom_target(
	fmt
	COMMAND /usr/bin/clang-format
	-style=LLVM
	-i
	${ALL_SOURCE_FILES}
)

add_executable(cv ${SOURCES})
target_link_libraries(cv ${CMAKE_THREAD_LIBS_INIT})
