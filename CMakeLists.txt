cmake_minimum_required(VERSION 3.16)
project(dgtkProject LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_CXX_STANDARD 17)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x)
FetchContent_MakeAvailable(SFML)

FetchContent_Declare(
        nlohmann
        GIT_REPOSITORY  "https://github.com/onavratil-monetplus/json"
        GIT_TAG         "master"
)

FetchContent_MakeAvailable( nlohmann )

add_executable(dgtkProject src/main.cpp include/decoration.hpp include/ui.hpp include/resource.hpp include/id.hpp src/resource.cpp include/display.hpp src/display.cpp include/position.hpp include/input.hpp include/system.hpp src/input.cpp include/log.hpp src/log.cpp include/game.hpp src/game.cpp include/map.hpp include/scene.hpp src/scene.cpp include/player.hpp include/creature.hpp src/player.cpp include/location.hpp include/view.hpp src/view.cpp include/math.hpp src/math.cpp src/map.cpp src/ui.cpp src/decoration.cpp include/layer.hpp include/action.hpp src/action.cpp src/creature.cpp include/bsp.hpp src/bsp.cpp)
target_link_libraries(dgtkProject PRIVATE sfml-audio sfml-graphics nlohmann_json::nlohmann_json)
target_compile_features(dgtkProject PRIVATE cxx_std_17)

if(WIN32)
    add_custom_command(
        TARGET dgtkProject
        COMMENT "Copy OpenAL DLL"
        PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:dgtkProject>
        VERBATIM)
endif()

install(TARGETS dgtkProject)
