cmake_minimum_required(VERSION 3.1)

project("bialang"
	LANGUAGES CXX)

function(download_submodule_library LIB_NAME)
	if(NOT EXISTS "${CMAKE_SOURCE_DIR}/dependencies/${LIB_NAME}")
		if(NOT GIT_FOUND)
			find_package(Git REQUIRED)
		endif()

		message("Downloading ${LIB_NAME}...")

		execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --depth 1 -- "dependencies/${LIB_NAME}"
			WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
	endif()
endfunction()

function(create_test NAME PATH)
	if(BIA_BUILD_TESTS)
		file(GLOB_RECURSE TMP_SOURCE_FILES "${PATH}/*.cpp")

		add_executable("${NAME}_test" ${TMP_SOURCE_FILES} ${BACKWARD_ENABLE})
		target_link_libraries("${NAME}_test" PUBLIC bsl compiler)
		target_include_directories("${NAME}_test" PUBLIC ${CATCH_INCLUDE_DIRS})
		
		if(BACKWARD_ENABLE)
			add_backward("${NAME}_test")
		endif()
		
		message(STATUS "Adding test - ${NAME}")

		if(BIA_BUILD_BENCHMARKS)
			add_test(${NAME} "${NAME}_test")
		else()
			add_test(${NAME} "${NAME}_test" "~[benchmark]")
		endif()
	endif()
endfunction()

set(CMAKE_CXX_STANDARD 11)

option(BIA_BUILD_BIA_TOOL "Builds the command line tool for Bia." OFF)
option(BUILD_SHARED_LIBS "Builds the Bia library as a shared library." ON)
option(BIA_BUILD_TESTS "Builds the tests for the Bia library." ON)
option(BIA_BUILD_EXAMPLES "Builds the example programs." ON)
option(BIA_GENERATE_DOCUMENTATION "Generate the documentatio with Doxygen." OFF)
option(BIA_ENABLE_DEV_BACKTRACING "Enables C++ backtracing. Intended for development." OFF)
option(BIA_ENABLE_SANITIZER "Enables the use of sanitizers." OFF)

if(BIA_ENABLE_SANITIZER)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O1 -fno-omit-frame-pointer -fno-optimize-sibling-calls -fsanitize=address")
	set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -g -fsanitize=address")
endif()

# Enable testing
if(BIA_BUILD_TESTS)
	include(CTest)
	set(CATCH_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/Catch2")
	enable_testing()
	option(BIA_BUILD_BENCHMARKS "Build additional benchmark tests." OFF)
endif()

# Enable backtracing
if(BIA_ENABLE_DEV_BACKTRACING)
	add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/dependencies/backward-cpp")
endif()

# Add Bia library
add_subdirectory("bia")

if(BIA_BUILD_BIA_TOOL)
	add_subdirectory("tools/bia")
endif()

if(BIA_BUILD_EXAMPLES)
	add_subdirectory("examples")
endif()

# Generate documentary
if(BIA_GENERATE_DOCUMENTATION)
	find_package(Doxygen REQUIRED)

	add_custom_target("doc"
		COMMAND ${DOXYGEN_EXECUTABLE} "Doxyfile"
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		COMMENT "Building documentation")
endif()


# CPack
set(CPACK_PACKAGE_NAME "Bia")
set(CPACK_PACKAGE_DESCRIPTION "Embedded scripting language")
set(CPACK_PACKAGE_VENDOR "Bialang")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "Bia/${PROJECT_VERSION}")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})

include(CPack)
