cmake_minimum_required(VERSION 3.10)
project(MyVisageProject CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add the visage library directory with a binary directory
add_subdirectory(
    /Users/skyler/Desktop/Crank/visage
    ${CMAKE_BINARY_DIR}/visage_build  # This creates a subdirectory for the build output of visage
)

# Create your executable
add_executable(my_visage_app src/main.cpp)

# Ensure the correct include directories for visage
# Corrected line: target_include_directories should be applied to a target
target_include_directories(my_visage_app PRIVATE ${CMAKE_SOURCE_DIR}/build/visage_build/include)

# Link the visage static library to your executable
target_link_libraries(my_visage_app PRIVATE visage)

add_custom_target(run
    COMMAND my_visage_app
    DEPENDS my_visage_app
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)