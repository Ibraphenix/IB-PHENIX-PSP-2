#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspdebug.h>
#include <pspctrl.h>
#include <pspgu.h>
#include <pspgum.h>

PSP_MODULE_INFO("IB_PHENIX", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

static unsigned int __attribute__((aligned(16))) list[262144];

typedef struct {
    unsigned int color;
    float x;
    float y;
    float z;
} Vertex;

static Vertex __attribute__((aligned(16))) vertices[4];

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

void initGraphics(void)
{
    sceGuInit();

    sceGuStart(GU_DIRECT, list);

    sceGuDrawBuffer(
        GU_PSM_8888,
        (void *)0,
        512
    );

    sceGuDispBuffer(
        480,
        272,
        (void *)0x88000,
        512
    );

    sceGuDepthBuffer(
        (void *)0x110000,
        512
    );

    sceGuOffset(
        2048 - (480 / 2),
        2048 - (272 / 2)
    );

    sceGuViewport(
        2048,
        2048,
        480,
        272
    );

    sceGuDepthRange(65535, 0);

    sceGuScissor(
        0,
        0,
        480,
        272
    );

    sceGuEnable(GU_SCISSOR_TEST);

    sceGuDisable(GU_DEPTH_TEST);
    sceGuDisable(GU_TEXTURE_2D);

    sceGuFinish();
    sceGuSync(0, 0);

    sceDisplayWaitVblankStart();
    sceGuDisplay(GU_TRUE);
}

void startFrame(void)
{
    sceGuStart(GU_DIRECT, list);

    sceGuClearColor(0xFF202030);
    sceGuClear(GU_COLOR_BUFFER_BIT);

    sceGuDisable(GU_TEXTURE_2D);
}

void endFrame(void)
{
    sceGuFinish();
    sceGuSync(0, 0);

    sceDisplayWaitVblankStart();
    sceGuSwapBuffers();
}

void drawRectangle(
    float x,
    float y,
    float width,
    float height,
    unsigned int color
)
{
    vertices[0].color = color;
    vertices[0].x = x;
    vertices[0].y = y;
    vertices[0].z = 0;

    vertices[1].color = color;
    vertices[1].x = x + width;
    vertices[1].y = y;
    vertices[1].z = 0;

    vertices[2].color = color;
    vertices[2].x = x + width;
    vertices[2].y = y + height;
    vertices[2].z = 0;

    vertices[3].color = color;
    vertices[3].x = x;
    vertices[3].y = y + height;
    vertices[3].z = 0;

    sceGuColor(color);

    sceGuDrawArray(
        GU_SPRITES,
        GU_COLOR_8888 | GU_VERTEX_32BITF,
        2,
        NULL,
        vertices
    );
}

void drawGame(
    float playerX,
    float playerY
)
{
    startFrame();

    /* Sky */
    drawRectangle(
        0,
        0,
        480,
        200,
        0xFF243B6B
    );

    /* Ground */
    drawRectangle(
        0,
        200,
        480,
        72,
        0xFF244D2A
    );

    /* Road */
    drawRectangle(
        170,
        200,
        140,
        72,
        0xFF303030
    );

    /* Road lines */
    drawRectangle(
        235,
        205,
        10,
        20,
        0xFFFFFFFF
    );

    drawRectangle(
        235,
        240,
        10,
        20,
        0xFFFFFFFF
    );

    /* Tree left */
    drawRectangle(
        70,
        145,
        18,
        65,
        0xFF5A3218
    );

    drawRectangle(
        45,
        115,
        70,
        60,
        0xFF176B35
    );

    /* Tree right */
    drawRectangle(
        390,
        145,
        18,
        65,
        0xFF5A3218
    );

    drawRectangle(
        365,
        115,
        70,
        60,
        0xFF176B35
    );

    /* Player body */
    drawRectangle(
        playerX,
        playerY,
        20,
        28,
        0xFFE5E5E5
    );

    /* Player head */
    drawRectangle(
        playerX + 4,
        playerY - 12,
        12,
        12,
        0xFFD29A6A
    );

    /* Player phoenix mark */
    drawRectangle(
        playerX + 7,
        playerY + 7,
        6,
        10,
        0xFFCC3333
    );

    endFrame();
}

void drawMenu(int selected)
{
    pspDebugScreenClear();

    pspDebugScreenSetXY(8, 3);
    pspDebugScreenPrintf(
        "================================"
    );

    pspDebugScreenSetXY(13, 5);
    pspDebugScreenPrintf(
        "IB PHENIX"
    );

    pspDebugScreenSetXY(10, 6);
    pspDebugScreenPrintf(
        "PSP ADVENTURE"
    );

    pspDebugScreenSetXY(7, 9);

    if (selected == 0)
        pspDebugScreenPrintf(
            "> COMMENCER L'AVENTURE"
        );
    else
        pspDebugScreenPrintf(
            "  COMMENCER L'AVENTURE"
        );

    pspDebugScreenSetXY(7, 11);

    if (selected == 1)
        pspDebugScreenPrintf(
            "> OPTIONS"
        );
    else
        pspDebugScreenPrintf(
            "  OPTIONS"
        );

    pspDebugScreenSetXY(7, 13);

    if (selected == 2)
        pspDebugScreenPrintf(
            "> QUITTER"
        );
    else
        pspDebugScreenPrintf(
            "  QUITTER"
        );

    pspDebugScreenSetXY(6, 18);
    pspDebugScreenPrintf(
        "UP/DOWN : NAVIGATE"
    );

    pspDebugScreenSetXY(6, 19);
    pspDebugScreenPrintf(
        "X : SELECT     O : BACK"
    );

    pspDebugScreenSetXY(6, 20);
    pspDebugScreenPrintf(
        "START : EXIT"
    );
}

void playGame(void)
{
    SceCtrlData pad;

    unsigned int oldButtons = 0;

    float playerX = 230.0f;
    float playerY = 170.0f;

    int running = 1;

    while (running)
    {
        sceCtrlPeekBufferPositive(
            &pad,
            1
        );

        unsigned int pressed =
            pad.Buttons & ~oldButtons;

        /* Analog movement */

        if (pad.Lx < 90)
            playerX -= 2.0f;

        if (pad.Lx > 165)
            playerX += 2.0f;

        if (pad.Ly < 90)
            playerY -= 2.0f;

        if (pad.Ly > 165)
            playerY += 2.0f;

        /* D-Pad movement */

        if (pad.Buttons & PSP_CTRL_LEFT)
            playerX -= 2.0f;

        if (pad.Buttons & PSP_CTRL_RIGHT)
            playerX += 2.0f;

        if (pad.Buttons & PSP_CTRL_UP)
            playerY -= 2.0f;

        if (pad.Buttons & PSP_CTRL_DOWN)
            playerY += 2.0f;

        /* Boundaries */

        if (playerX < 10)
            playerX = 10;

        if (playerX > 450)
            playerX = 450;

        if (playerY < 80)
            playerY = 80;

        if (playerY > 220)
            playerY = 220;

        /* Back to menu */

        if (pressed & PSP_CTRL_CIRCLE)
            running = 0;

        /* Exit */

        if (pressed & PSP_CTRL_START)
            sceKernelExitGame();

        drawGame(
            playerX,
            playerY
        );

        oldButtons = pad.Buttons;

        sceDisplayWaitVblankStart();
    }
}

int main(void)
{
    SetupCallbacks();

    sceCtrlSetSamplingCycle(0);
    sceCtrlSetSamplingMode(
        PSP_CTRL_MODE_ANALOG
    );

    initGraphics();

    SceCtrlData pad;

    unsigned int oldButtons = 0;

    int selected = 0;
    int running = 1;

    drawMenu(selected);

    while (running)
    {
        sceCtrlPeekBufferPositive(
            &pad,
            1
        );

        unsigned int pressed =
            pad.Buttons & ~oldButtons;

        if (pressed & PSP_CTRL_UP)
        {
            selected--;

            if (selected < 0)
                selected = 2;

            drawMenu(selected);
        }

        if (pressed & PSP_CTRL_DOWN)
        {
            selected++;

            if (selected > 2)
                selected = 0;

            drawMenu(selected);
        }

        if (pressed & PSP_CTRL_CROSS)
        {
            if (selected == 0)
            {
                playGame();
                drawMenu(selected);
            }

            if (selected == 1)
            {
                pspDebugScreenClear();

                pspDebugScreenSetXY(10, 7);

                pspDebugScreenPrintf(
                    "OPTIONS"
                );

                pspDebugScreenSetXY(6, 10);

                pspDebugScreenPrintf(
                    "IB PHENIX PSP"
                );

                pspDebugScreenSetXY(6, 12);

                pspDebugScreenPrintf(
                    "GRAPHIC PROTOTYPE"
                );

                pspDebugScreenSetXY(6, 15);

                pspDebugScreenPrintf(
                    "PRESS O TO RETURN"
                );

                while (1)
                {
                    sceCtrlPeekBufferPositive(
                        &pad,
                        1
                    );

                    if (pad.Buttons &
                        PSP_CTRL_CIRCLE)
                    {
                        break;
                    }

                    sceDisplayWaitVblankStart();
                }

                drawMenu(selected);
            }

            if (selected == 2)
                running = 0;
        }

        if (pressed & PSP_CTRL_START)
            running = 0;

        oldButtons = pad.Buttons;

        sceDisplayWaitVblankStart();
    }

    sceKernelExitGame();

    return 0;
}
