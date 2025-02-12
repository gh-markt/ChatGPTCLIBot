cmake_minimum_required(VERSION 3.24)
project(GPT3Bot)

set(CMAKE_CXX_STANDARD 20)

# Disable clip examples and tests
set(CLIP_EXAMPLES OFF CACHE BOOL "Compile clip examples")
set(CLIP_TESTS OFF CACHE BOOL "Compile clip tests")

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED)
if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIRS})
endif ()

find_package(CURL CONFIG REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(ftxui REQUIRED)
if (WIN32)
    find_package(libproxy REQUIRED)
endif ()

add_executable(${PROJECT_NAME} main/main.cpp main/GPTMain.cpp main/GPTMain.h main/interface/util/PromptUtils.cpp main/interface/util/PromptUtils.h main/interface/util/SystemUtils.cpp main/interface/util/SystemUtils.h main/interface/model/Embedding.cpp main/interface/model/Embedding.h main/interface/chat/Exchange.cpp main/interface/chat/Exchange.h main/interface/util/CURLUtils.cpp main/interface/util/CURLUtils.h main/interface/network/Network.cpp main/interface/network/Network.h main/interface/cpp-terminal/platforms/conversion.hpp main/interface/cpp-terminal/platforms/platform.hpp main/interface/cpp-terminal/platforms/platform.cpp main/interface/cpp-terminal/platforms/input.cpp main/interface/cpp-terminal/platforms/macros.hpp main/interface/cpp-terminal/platforms/tty.cpp main/interface/cpp-terminal/platforms/terminfo.cpp main/interface/cpp-terminal/platforms/terminal.cpp main/interface/cpp-terminal/base.cpp main/interface/cpp-terminal/base.hpp main/interface/cpp-terminal/color.cpp main/interface/cpp-terminal/color.hpp main/interface/cpp-terminal/exception.hpp main/interface/cpp-terminal/input.cpp main/interface/cpp-terminal/input.hpp main/interface/cpp-terminal/prompt.cpp main/interface/cpp-terminal/prompt.hpp main/interface/cpp-terminal/terminal.cpp main/interface/cpp-terminal/terminal.hpp main/interface/cpp-terminal/terminfo.hpp main/interface/cpp-terminal/tty.hpp main/interface/cpp-terminal/window.cpp main/interface/cpp-terminal/window.hpp main/resources.rc main/interface/util/TermUtils.cpp main/interface/util/TermUtils.h main/interface/network/APIKey.cpp main/interface/network/APIKey.h main/interface/util/TokenUtils.cpp main/interface/util/TokenUtils.h)
add_subdirectory(main/interface/clip)
add_subdirectory(main/interface/cpp-tiktoken)

target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
target_link_libraries(${PROJECT_NAME} PRIVATE CURL::libcurl nlohmann_json::nlohmann_json ftxui::component clip tiktoken)
if (WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBPROXY_LIBRARIES})
endif ()
if (NOT APPLE)
    if (UNIX)
        set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -lpthread")
    else ()
        set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -Wl,-Bstatic,--whole-archive -lpthread -Wl,-Bdynamic,--no-whole-archive")
    endif ()
endif ()