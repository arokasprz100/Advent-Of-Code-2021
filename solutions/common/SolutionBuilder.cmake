include(CMakeParseArguments)

function(build_solution_for_given_day)
    cmake_parse_arguments(
            PARSED_ARGS # prefix of output variables
            "INSTALL_FILE" # list of names of the boolean arguments (only defined ones will be true)
            "DAY_NUMBER" # list of names of mono-valued arguments
            "" # list of names of multi-valued arguments (output variables are lists)
            ${ARGN} # arguments of the function to parse, here we take the all original ones
    )

    if(NOT PARSED_ARGS_DAY_NUMBER)
        message(FATAL_ERROR "Day number not given")
    endif()

    set(CMAKE_CXX_STANDARD 20)
    add_executable(Day${PARSED_ARGS_DAY_NUMBER} ${CMAKE_CURRENT_SOURCE_DIR}/day${PARSED_ARGS_DAY_NUMBER}.cpp)
    if(PARSED_ARGS_INSTALL_FILE)
        configure_file(
                ${CMAKE_CURRENT_SOURCE_DIR}/input.txt
                ${CMAKE_CURRENT_BINARY_DIR}/input.txt
                COPYONLY
        )
    endif()

endfunction()