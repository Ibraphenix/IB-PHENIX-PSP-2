cmake_minimum_required(VERSION 3.18)

project(IB_PHENIX C)

set(CMAKE_C_STANDARD 99)

add_executable(IB_PHENIX
    src/main.c
)

target_link_libraries(IB_PHENIX
    pspdebug
    pspdisplay
    pspctrl
    pspkernel
)

add_custom_command(TARGET IB_PHENIX POST_BUILD
    COMMAND mksfoex -s TITLE="IB PHENIX" -s DISC_ID=IBPHNX01 "IB PHENIX" PARAM.SFO
    COMMAND pack-pbp
        EBOOT.PBP
        PARAM.SFO
        NULL
        NULL
        NULL
        NULL
        NULL
        IB_PHENIX
        NULL
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Création de EBOOT.PBP"
)
