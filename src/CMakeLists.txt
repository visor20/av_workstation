cmake_minimum_required(VERSION 3.13)

project(VIDAW C)

#external directories on local windows machine
set(EXT_DIRS "C:/libs")

#include SDL2
include_directories(${EXT_DIRS}/SDL2/include)
link_directories(${EXT_DIRS}/SDL2/lib/x64)

#include SDL image (for text support later)
#include_directories(${EXT_DIRS}/SDL2_image-2.8.2/include)
#link_directories(${EXT_DIRS}/SDL2_image-2.8.2/lib/x64)

#include portaudio(x64)
include_directories(${EXT_DIRS}/portaudio/include)
link_directories(${EXT_DIRS}/portaudio/build/Debug)


# add source
add_executable(${PROJECT_NAME} 
    main.c 
    item_list.c
    graphics.c
    audio_functions.c
)

# link the external libraries, second arg is .lib name
# sdl main is specifically required for windows systems
target_link_libraries(${PROJECT_NAME} SDL2main)
target_link_libraries(${PROJECT_NAME} SDL2)
#target_link_libraries(${PROJECT_NAME} SDL2_image)
target_link_libraries(${PROJECT_NAME} portaudio_x64)
