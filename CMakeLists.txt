cmake_minimum_required(VERSION 3.16)
project(BugPlanner CXX)
set(CMAKE_CXX_STANDARD 17)

find_package(Doxygen)
find_package(PkgConfig REQUIRED)

pkg_check_modules(SDL2 REQUIRED sdl2)
pkg_check_modules(SDL2_TTF REQUIRED SDL2_ttf)

add_executable(bug_planner
    src/main.cpp
    src/mapfile.cpp
    src/grid.cpp
    src/robot.cpp
    src/app.cpp
    src/app_events.cpp
    src/app_editor.cpp
    src/app_ui.cpp
    src/app_sim.cpp
    src/app_map.cpp
    src/app_render.cpp
    src/bug0.cpp
    src/bug1.cpp
    src/bug2.cpp
)

# Copy example maps to the build directory so the executable can find them.
add_custom_command(TARGET bug_planner POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/maps
        $<TARGET_FILE_DIR:bug_planner>/maps
    COMMENT "Copying example maps to build directory"
)

target_include_directories(bug_planner PRIVATE
    include
    ${SDL2_INCLUDE_DIRS}
    ${SDL2_TTF_INCLUDE_DIRS}
)

target_link_libraries(bug_planner PRIVATE
    ${SDL2_LIBRARIES}
    ${SDL2_TTF_LIBRARIES}
)

target_compile_options(bug_planner PRIVATE
    ${SDL2_CFLAGS_OTHER}
    ${SDL2_TTF_CFLAGS_OTHER}
    -O2 -Wall -Wextra
)



# ---- Doxygen documentation ----

if(DOXYGEN_FOUND)
    set(DOXYGEN_IN ${CMAKE_SOURCE_DIR}/docs/Doxyfile)
    set(DOXYGEN_OUT ${CMAKE_BINARY_DIR}/Doxyfile)

    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

    add_custom_target(docs
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM
    )
else()
    message(STATUS "Doxygen not found, docs target will not be available")
endif()