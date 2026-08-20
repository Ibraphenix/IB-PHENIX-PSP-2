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
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_DIGITAL);

    SceCtrlData pad;

    pspDebugScreenPrintf("\n\n");
    pspDebugScreenPrintf("================================\n");
    pspDebugScreenPrintf("          IB PHENIX PSP\n");
    pspDebugScreenPrintf("================================\n\n");
    pspDebugScreenPrintf("          PROTOTYPE V2\n\n");
    pspDebugScreenPrintf("          Bienvenue !\n\n");
    pspDebugScreenPrintf("     Appuie sur START pour\n");
    pspDebugScreenPrintf("          quitter.\n\n");

    while (1)
    {
        sceCtrlPeekBufferPositive(&pad, 1);

        if (pad.Buttons & PSP_CTRL_START)
        {
            break;
        }

        sceDisplayWaitVblankStart();
    }

    sceKernelExitGame();

    return 0;
}
