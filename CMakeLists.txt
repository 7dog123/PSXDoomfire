cmake_minimum_required(VERSION 3.10)

# Define the project name and version
project(DoomFire CXX)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 11)

# Include PS2 SDK and SDL2 directories
include_directories(${PS2SDK}/include)
include_directories(${PS2SDK}/ports/include/SDL2)

# Set the path for the SDL2 library
link_directories(${PS2SDK}/ports/lib)

# Add your source files
set(SOURCES
    doomfire.cpp
)

# Define the executable target
add_executable(doomfire ${SOURCES})

# Set the required compiler flags for the PS2 (e.g., optimization and linking)
target_compile_options(doomfire PRIVATE
    -O2
    -G0
    -Wall
    -fno-exceptions
    -fno-rtti
    -Dmain=SDL_main
)

# Link the SDL2 library and other required libraries
target_link_libraries(doomfire
    SDL2
    SDL2main
    c
    m
	gskit
    dmakit
    dma
	ps2_drivers
    patches
)

# Set the linker flags
set_target_properties(doomfire PROPERTIES LINK_FLAGS "-L${PS2SDK}/ports/sdl2/lib")

# Set the final output file name (EE binary)
set_target_properties(doomfire PROPERTIES
    OUTPUT_NAME doomfire.elf
)
