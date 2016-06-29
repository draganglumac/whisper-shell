cmake_minimum_required(VERSION 2.8)
set(INSTALL_PREFIX /usr)
project(whisper-shell)

execute_process(COMMAND git submodule init)
execute_process(COMMAND git submodule update)
execute_process(COMMAND cmake . WORKING_DIRECTORY whisper-core)
execute_process(COMMAND make . WORKING_DIRECTORY whisper-core)
execute_process(COMMAND sudo make install . WORKING_DIRECTORY whisper-core)

add_subdirectory(src)
include_directories(${PROJECT_INC_DIR}src)
add_executable(whisper-shell ${SOURCE})
target_link_libraries(whisper-shell -lwhisper-core -lncurses -lpthread)