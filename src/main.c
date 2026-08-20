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

void drawMenu(int selected)
{
    pspDebugScreenClear();

    pspDebugScreenSetXY(8, 3);
    pspDebugScreenPrintf("================================");

    pspDebugScreenSetXY(13, 5);
    pspDebugScreenPrintf("IB PHENIX");

    pspDebugScreenSetXY(10, 6);
    pspDebugScreenPrintf("PSP ADVENTURE");

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

void drawPlayer(int x, int y)
{
    pspDebugScreenSetXY(x, y);
    pspDebugScreenPrintf("  O  ");

    pspDebugScreenSetXY(x, y + 1);
    pspDebugScreenPrintf(" /|\\ ");

    pspDebugScreenSetXY(x, y + 2);
    pspDebugScreenPrintf(" / \\ ");

    pspDebugScreenSetXY(x, y + 3);
    pspDebugScreenPrintf("/___\\");
}

void drawEnemy(int x, int y)
{
    pspDebugScreenSetXY(x, y);
    pspDebugScreenPrintf(" /\\ ");

    pspDebugScreenSetXY(x, y + 1);
    pspDebugScreenPrintf("<XX>");

    pspDebugScreenSetXY(x, y + 2);
    pspDebugScreenPrintf(" \\/ ");

    pspDebugScreenSetXY(x, y + 3);
    pspDebugScreenPrintf("/  \\");
}

void drawHealth(int health)
{
    pspDebugScreenSetXY(2, 2);
    pspDebugScreenPrintf("HP: ");

    int i;

    for (i = 0; i < 10; i++)
    {
        if (i < health)
            pspDebugScreenPrintf("#");
        else
            pspDebugScreenPrintf("-");
    }

    pspDebugScreenPrintf(" %d/10", health);
}

void drawGame(
    int playerX,
    int playerY,
    int enemyX,
    int enemyY,
    int health
)
{
    pspDebugScreenClear();

    pspDebugScreenSetXY(2, 1);
    pspDebugScreenPrintf("IB PHENIX - ZONE 01");

    drawHealth(health);

    pspDebugScreenSetXY(2, 5);
    pspDebugScreenPrintf(
        "=========================================================="
    );

    pspDebugScreenSetXY(5, 8);
    pspDebugScreenPrintf("FOREST OF THE PHOENIX");

    pspDebugScreenSetXY(5, 11);
    pspDebugScreenPrintf("      ^       ^       ^");

    pspDebugScreenSetXY(5, 12);
    pspDebugScreenPrintf("     /|\\     /|\\     /|\\");

    pspDebugScreenSetXY(5, 13);
    pspDebugScreenPrintf("    /###\\   /###\\   /###\\");

    pspDebugScreenSetXY(5, 17);
    pspDebugScreenPrintf("              PATH");

    drawPlayer(playerX, playerY);
    drawEnemy(enemyX, enemyY);

    pspDebugScreenSetXY(2, 22);
    pspDebugScreenPrintf("ANALOG / D-PAD : MOVE");

    pspDebugScreenSetXY(2, 24);
    pspDebugScreenPrintf("X : ATTACK     O : MENU");

    pspDebugScreenSetXY(2, 25);
    pspDebugScreenPrintf("START : EXIT");
}

void playGame(void)
{
    SceCtrlData pad;

    unsigned int oldButtons = 0;

    int playerX = 10;
    int playerY = 12;

    int enemyX = 42;
    int enemyY = 12;

    int health = 10;

    int attackTimer = 0;
    int damageTimer = 0;

    int running = 1;

    while (running)
    {
        sceCtrlPeekBufferPositive(&pad, 1);

        unsigned int pressed =
            pad.Buttons & ~oldButtons;

        if (pad.Lx < 90)
            playerX--;

        if (pad.Lx > 165)
            playerX++;

        if (pad.Ly < 90)
            playerY--;

        if (pad.Ly > 165)
            playerY++;

        if (pad.Buttons & PSP_CTRL_LEFT)
            playerX--;

        if (pad.Buttons & PSP_CTRL_RIGHT)
            playerX++;

        if (pad.Buttons & PSP_CTRL_UP)
            playerY--;

        if (pad.Buttons & PSP_CTRL_DOWN)
            playerY++;

        if (playerX < 3)
            playerX = 3;

        if (playerX > 48)
            playerX = 48;

        if (playerY < 8)
            playerY = 8;

        if (playerY > 15)
            playerY = 15;

        if (enemyX < playerX)
            enemyX++;

        if (enemyX > playerX)
            enemyX--;

        if (enemyY < playerY)
            enemyY++;

        if (enemyY > playerY)
            enemyY--;

        if (pressed & PSP_CTRL_CROSS)
        {
            attackTimer = 10;

            if (enemyX >= playerX - 3 &&
                enemyX <= playerX + 7 &&
                enemyY >= playerY - 3 &&
                enemyY <= playerY + 5)
            {
                enemyX = 42;
                enemyY = 12;
            }
        }

        if (attackTimer > 0)
            attackTimer--;

        if (damageTimer > 0)
            damageTimer--;

        if (enemyX >= playerX - 2 &&
            enemyX <= playerX + 5 &&
            enemyY >= playerY - 2 &&
            enemyY <= playerY + 4)
        {
            if (damageTimer == 0)
            {
                health--;

                damageTimer = 30;

                if (health <= 0)
                {
                    health = 10;
                    playerX = 10;
                    playerY = 12;
                    enemyX = 42;
                    enemyY = 12;
                }
            }
        }

        if (pressed & PSP_CTRL_CIRCLE)
            running = 0;

        if (pressed & PSP_CTRL_START)
            sceKernelExitGame();

        drawGame(
            playerX,
            playerY,
            enemyX,
            enemyY,
            health
        );

        oldButtons = pad.Buttons;

        sceDisplayWaitVblankStart();
    }
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

    drawMenu(selected);

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
                pspDebugScreenPrintf("OPTIONS");

                pspDebugScreenSetXY(6, 10);
                pspDebugScreenPrintf("IB PHENIX PSP");

                pspDebugScreenSetXY(6, 12);
                pspDebugScreenPrintf("PROTOTYPE");

                pspDebugScreenSetXY(6, 15);
                pspDebugScreenPrintf("PRESS O TO RETURN");

                while (1)
                {
                    sceCtrlPeekBufferPositive(&pad, 1);

                    if (pad.Buttons & PSP_CTRL_CIRCLE)
                        break;

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
