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

# Génération de l'EBOOT.PBP
"
)
add_custom_command(TARGET IB_PHENIX POST_BUILD
    COMMAND mksfo "IB PHENIX" PARAM.SFO
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
