cmake_minimum_required(VERSION 3.18)

project(IB_PHENIX C)

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
    pspkernel
)

add_custom_command(TARGET IB_PHENIX POST_BUILD
    COMMAND ${PSPDEV}/bin/psp-fixup-imports
            $<TARGET_FILE:IB_PHENIX>
    COMMENT "Fix PSP imports"
)

add_custom_command(TARGET IB_PHENIX POST_BUILD
    COMMAND ${PSPDEV}/bin/mksfoex
            -d MEMSIZE=1
            "IB PHENIX"
            PARAM.SFO
    COMMAND ${PSPDEV}/bin/pack-pbp
            EBOOT.PBP
            PARAM.SFO
            NULL
            NULL
            NULL
            NULL
            NULL
            $<TARGET_FILE:IB_PHENIX>
            NULL
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Create EBOOT.PBP"
)
