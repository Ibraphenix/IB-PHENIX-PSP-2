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

int main(void)
{
    SetupCallbacks();

    pspDebugScreenInit();

    sceCtrlSetSamplingCycle(0);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);

    SceCtrlData pad;

    while (1)
    {
        sceCtrlPeekBufferPositive(&pad, 1);

        pspDebugScreenSetXY(0, 5);

        pspDebugScreenPrintf("IB PHENIX CONTROLLER TEST\n\n");
        pspDebugScreenPrintf("BUTTONS : %08X\n", pad.Buttons);
        pspDebugScreenPrintf("ANALOG X: %3d\n", pad.Lx);
        pspDebugScreenPrintf("ANALOG Y: %3d\n", pad.Ly);

        if (pad.Buttons & PSP_CTRL_START)
            sceKernelExitGame();

        sceDisplayWaitVblankStart();
    }

    return 0;
}
