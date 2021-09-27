# doxygen.cmake

find_package(Doxygen)

if (DOXYGEN_FOUND)
    set(DOXYGEN_DOXYFILE_ENCODING "UTF-8")
    set(DOXYGEN_PROJECT_NAME "\"${PROJECT_NAME}\"")
    set(DOXYGEN_PROJECT_NUMBER "${PROJECT_VERSION}")
    set(DOXYGEN_PROJECT_BRIEF "Programm for designing and deploying large distributed programm-hardware systems")
    set(DOXYGEN_PROJECT_LOGO "${CMAKE_CURRENT_SOURCE_DIR}/resources/images/splash.png")
    set(DOXYGEN_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/documentation")
    set(DOXYGEN_QT_AUTOBRIEF "YES")
    set(DOXYGEN_FILE_PATTERNS *.c *.cc *.cxx *.cpp *.c++ *.h *.hh *.hpp)
    set(DOXYGEN_RECURSIVE "YES")
    set(DOXYGEN_ABBREVIATE_BRIEF
        "The $name class"
        "The $name widget"
        "The $name file"
        "is"
        "provides"
        "specifies"
        "contains"
        "represents"
        "a"
        "an"
        "the"
        )
    set(DOXYGEN_OPTIMIZE_OUTPUT_FOR_C "YES")
    set(DOXYGEN_CPP_CLI_SUPPORT "YES")
    set(DOXYGEN_CASE_SENSE_NAMES "NO")
    set(DOXYGEN_EXCLUDE_PATTERNS "*/*_autogen/*" "*/build-*/*" "*/Catch2/*")
    set(DOXYGEN_GENERATE_LATEX "NO")

    doxygen_add_docs(doxygen ${CMAKE_CURRENT_SOURCE_DIR} COMMENT "generate doxygen documentation by cmake")

    add_custom_target(documentation
                      COMMAND doxygen "Doxyfile.doxygen"
                      DEPENDS ${PROJECT_NAME}
                      WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
                      COMMENT "Generate doxygen documentation"
                      )
endif ()
