#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspdebug.h>
#include <pspctrl.h>

PSP_MODULE_INFO("IB_PHENIX", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

int exit_callback(int arg1, int arg2, void *common)
{
    sceKernelExitGame();
    return 0;
}

int CallbackThread(SceSize args, void *argp)
{
    int cbid = sceKernelCreateCallback(
        "Exit Callback",
        exit_callback,
        NULL
    );

    if (cbid >= 0)
        sceKernelRegisterExitCallback(cbid);

    sceKernelSleepThreadCB();

    return 0;
}

int SetupCallbacks(void)
{
    int thid = sceKernelCreateThread(
        "update_thread",
        CallbackThread,
        0x11,
        0xFA0,
        0,
        NULL
    );

    if (thid >= 0)
        sceKernelStartThread(thid, 0, NULL);

    return thid;
}

void draw_menu(int selected)
{
    pspDebugScreenClear();

    pspDebugScreenSetXY(8, 3);
    pspDebugScreenPrintf("================================");

    pspDebugScreenSetXY(10, 5);
    pspDebugScreenPrintf("IB PHENIX");

    pspDebugScreenSetXY(9, 6);
    pspDebugScreenPrintf("PSP PROTOTYPE");

    pspDebugScreenSetXY(7, 9);

    if (selected == 0)
        pspDebugScreenPrintf("> COMMENCER L'AVENTURE");
    else
        pspDebugScreenPrintf("  COMMENCER L'AVENTURE");

    pspDebugScreenSetXY(7, 11);

    if (selected == 1)
        pspDebugScreenPrintf("> OPTIONS");
    else
        pspDebugScreenPrintf("  OPTIONS");

    pspDebugScreenSetXY(7, 13);

    if (selected == 2)
        pspDebugScreenPrintf("> QUITTER");
    else
        pspDebugScreenPrintf("  QUITTER");

    pspDebugScreenSetXY(6, 18);
    pspDebugScreenPrintf("UP/DOWN : NAVIGATE");

    pspDebugScreenSetXY(6, 19);
    pspDebugScreenPrintf("X : SELECT     O : BACK");

    pspDebugScreenSetXY(6, 20);
    pspDebugScreenPrintf("START : EXIT");
}

int main(void)
{
    SetupCallbacks();

    pspDebugScreenInit();

    sceCtrlSetSamplingCycle(0);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);

    SceCtrlData pad;
    unsigned int oldButtons = 0;

    int selected = 0;
    int running = 1;

    draw_menu(selected);

    while (running)
    {
        sceCtrlPeekBufferPositive(&pad, 1);

        unsigned int pressed =
            pad.Buttons & ~oldButtons;

        if (pressed & PSP_CTRL_UP)
        {
            selected--;

            if (selected < 0)
                selected = 2;

            draw_menu(selected);
        }

        if (pressed & PSP_CTRL_DOWN)
        {
            selected++;

            if (selected > 2)
                selected = 0;

            draw_menu(selected);
        }

        if (pressed & PSP_CTRL_CROSS)
        {
            if (selected == 0)
            {
                pspDebugScreenClear();

                pspDebugScreenSetXY(8, 7);
                pspDebugScreenPrintf("IB PHENIX");

                pspDebugScreenSetXY(6, 9);
                pspDebugScreenPrintf("AVENTURE EN PREPARATION...");

                pspDebugScreenSetXY(6, 12);
                pspDebugScreenPrintf("Appuie sur O pour revenir.");
            }

            if (selected == 1)
            {
                pspDebugScreenClear();

                pspDebugScreenSetXY(8, 7);
                pspDebugScreenPrintf("OPTIONS");

                pspDebugScreenSetXY(5, 10);
                pspDebugScreenPrintf("Prototype IB PHENIX");

                pspDebugScreenSetXY(6, 14);
                pspDebugScreenPrintf("Appuie sur O pour revenir.");
            }

            if (selected == 2)
            {
                running = 0;
            }
        }

        if (pressed & PSP_CTRL_CIRCLE)
        {
            draw_menu(selected);
        }

        if (pressed & PSP_CTRL_START)
        {
            running = 0;
        }

        oldButtons = pad.Buttons;

        sceDisplayWaitVblankStart();
    }

    sceKernelExitGame();

    return 0;
}
