cmake_minimum_required(VERSION 2.8.4)
project(cog)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/cog.app/Contents/MacOS")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/cog.app/Contents/MacOS")

add_library(game SHARED
    src/Assets.cpp
    src/Entities.cpp
    src/Game.cpp
    src/Images.cpp
    src/Materials.cpp
    src/Meshes.cpp
    src/Shaders.cpp
    src/Textures.cpp
    src/TextureCubes.cpp
    src/assetParsers/imageParser.cpp
    src/components/Cameras.cpp
    src/components/MeshRenderers.cpp
    src/components/Transforms.cpp
    src/meshBuilders/Quad.cpp
    src/meshBuilders/UVSphere.cpp)

target_compile_options(game PRIVATE
    -std=c++11 -Werror -pedantic -fvisibility=hidden -fvisibility-inlines-hidden
    $<$<NOT:$<CONFIG:DEBUG>>:-flto>
    $<$<CXX_COMPILER_ID:Clang>:-stdlib=libc++ -Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-padded -Wno-float-equal>
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wno-missing-field-initializers>)

add_library(stb_image
    libs/stb_image.c)

target_link_libraries(game
    stb_image
    "-framework OpenGL")

add_library(gameRunner
    runner/AssetLoader.cpp
    runner/CircularBuffer.cpp
    runner/GameRunner.cpp)

target_compile_options(gameRunner PRIVATE
    -std=c++11 -Werror -pedantic -fvisibility=hidden -fvisibility-inlines-hidden
    $<$<NOT:$<CONFIG:DEBUG>>:-flto>
    $<$<CXX_COMPILER_ID:Clang>:-stdlib=libc++ -Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-padded -Wno-float-equal>
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wno-missing-field-initializers>)

target_link_libraries(gameRunner "-lpthread")

# TODO: We should use a png crusher on the resized files
add_custom_command(
    OUTPUT "${CMAKE_SOURCE_DIR}/build/cog.app/Contents/Resources/icon.icns"
    COMMAND rm -rf "${CMAKE_BINARY_DIR}/icon.iconset"
    COMMAND mkdir -p "${CMAKE_BINARY_DIR}/icon.iconset"
    COMMAND sips -z 16 16 "${CMAKE_SOURCE_DIR}/data/icon.png" --out "${CMAKE_BINARY_DIR}/icon.iconset/icon_16x16.png"
    COMMAND sips -z 32 32 "${CMAKE_SOURCE_DIR}/data/icon.png" --out "${CMAKE_BINARY_DIR}/icon.iconset/icon_16x16@2x.png"
    COMMAND sips -z 32 32 "${CMAKE_SOURCE_DIR}/data/icon.png" --out "${CMAKE_BINARY_DIR}/icon.iconset/icon_32x32.png"
    COMMAND sips -z 64 64 "${CMAKE_SOURCE_DIR}/data/icon.png" --out "${CMAKE_BINARY_DIR}/icon.iconset/icon_32x32@2x.png"
    COMMAND sips -z 128 128 "${CMAKE_SOURCE_DIR}/data/icon.png" --out "${CMAKE_BINARY_DIR}/icon.iconset/icon_128x128.png"
    COMMAND sips -z 256 256 "${CMAKE_SOURCE_DIR}/data/icon.png" --out "${CMAKE_BINARY_DIR}/icon.iconset/icon_128x128@2x.png"
    COMMAND sips -z 256 256 "${CMAKE_SOURCE_DIR}/data/icon.png" --out "${CMAKE_BINARY_DIR}/icon.iconset/icon_256x256.png"
    COMMAND sips -z 512 512 "${CMAKE_SOURCE_DIR}/data/icon.png" --out "${CMAKE_BINARY_DIR}/icon.iconset/icon_256x256@2x.png"
    COMMAND sips -z 512 512 "${CMAKE_SOURCE_DIR}/data/icon.png" --out "${CMAKE_BINARY_DIR}/icon.iconset/icon_512x512.png"
    COMMAND sips -z 1024 1024 "${CMAKE_SOURCE_DIR}/data/icon.png" --out "${CMAKE_BINARY_DIR}/icon.iconset/icon_512x512@2x.png"
    COMMAND mkdir -p "${CMAKE_SOURCE_DIR}/build/cog.app/Contents/Resources"
    COMMAND iconutil -c icns -o "${CMAKE_SOURCE_DIR}/build/cog.app/Contents/Resources/icon.icns" "${CMAKE_BINARY_DIR}/icon.iconset"
    DEPENDS "${CMAKE_SOURCE_DIR}/data/icon.png")

add_custom_command(
    OUTPUT "${CMAKE_SOURCE_DIR}/build/cog.app/Contents/Info.plist"
    COMMAND mkdir -p "${CMAKE_SOURCE_DIR}/build/cog.app/Contents/"
    COMMAND cp "${CMAKE_SOURCE_DIR}/data/Info.plist" "${CMAKE_SOURCE_DIR}/build/cog.app/Contents/Info.plist"
    DEPENDS "${CMAKE_SOURCE_DIR}/data/Info.plist")

add_custom_target(bundle
    COMMAND mkdir -p "${CMAKE_SOURCE_DIR}/build/cog.app/Contents/MacOS"
    DEPENDS
    "${CMAKE_SOURCE_DIR}/build/cog.app/Contents/Resources/icon.icns"
    "${CMAKE_SOURCE_DIR}/build/cog.app/Contents/Info.plist")

add_custom_command(TARGET bundle
    POST_BUILD
    COMMAND touch "${CMAKE_SOURCE_DIR}/build/cog.app")

add_executable(cog
    osx/GameView.mm
    osx/GameWindowDelegate.mm
    osx/main.mm)

target_compile_options(cog PRIVATE
    -x objective-c++ -std=c++11 -Werror -pedantic -fvisibility=hidden -fvisibility-inlines-hidden
    $<$<NOT:$<CONFIG:DEBUG>>:-flto>
    $<$<CXX_COMPILER_ID:Clang>:-stdlib=libc++ -Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-padded -Wno-float-equal>
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wno-missing-field-initializers>)

target_link_libraries(cog gameRunner "-framework Cocoa" "-framework CoreVideo" "-framework CoreServices" "-framework OpenGL")

add_dependencies(cog
    game)

add_dependencies(game
    bundle)
