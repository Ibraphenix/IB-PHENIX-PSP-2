cmake_minimum_required(VERSION 3.18)

project(IB_PHENIX)

set(CMAKE_C_STANDARD 99)

add_executable(IB_PHENIX
    src/main.c
)

target_link_libraries(IB_PHENIX
    pspdebug
    pspge
    pspdisplay
    pspctrl
    pspgu
    pspgum
    pspaudio
    pspkernel
    psputility
)
