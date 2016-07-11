cmake_minimum_required(VERSION 2.8)
include(ExternalProject)
set(INSTALL_PREFIX /usr)
project(whisper-shell)

# ------------------------------------------------------------------------------
# Add whisper-core -------------------------------------------------------------
set(whisper-core_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/whisper-core")
set(whisper-core_INSTALL_DIR "/usr")
set(whisper-core_CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${whisper-protocol_INSTALL_DIR})

ExternalProject_Add(whisper-core-project
  PREFIX ${whisper-core_PREFIX}
  GIT_REPOSITORY https://github.com/AlexsJones/whisper-core.git
  INSTALL_DIR ${whisper-core_INSTALL_DIR}
  CMAKE_ARGS ${whisper-core_CMAKE_ARGS}
  BUILD_COMMAND sudo make
  INSTALL_COMMAND sudo make install
  )

# ------------------------------------------------------------------------------
add_subdirectory(src)
include_directories(${PROJECT_INC_DIR}src)
add_executable(whisper-shell ${SOURCE})
add_dependencies(whisper-shell whisper-core-project)
target_link_libraries(whisper-shell whisper-core -lncurses -lpthread -ljnxc)
install(TARGETS whisper-shell DESTINATION ${INSTALL_PREFIX}/bin)
