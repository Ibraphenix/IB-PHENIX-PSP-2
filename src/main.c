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

void drawMenu(int selected, int level, int xp)
{
    pspDebugScreenClear();

    pspDebugScreenSetXY(8, 3);
    pspDebugScreenPrintf("================================");

    pspDebugScreenSetXY(13, 5);
    pspDebugScreenPrintf("IB PHENIX");

    pspDebugScreenSetXY(10, 6);
    pspDebugScreenPrintf("PSP ADVENTURE");

    pspDebugScreenSetXY(10, 7);
    pspDebugScreenPrintf("LEVEL %d   XP %d/100", level, xp);

    pspDebugScreenSetXY(7, 10);

    if (selected == 0)
        pspDebugScreenPrintf("> COMMENCER L'AVENTURE");
    else
        pspDebugScreenPrintf("  COMMENCER L'AVENTURE");

    pspDebugScreenSetXY(7, 12);

    if (selected == 1)
        pspDebugScreenPrintf("> OPTIONS");
    else
        pspDebugScreenPrintf("  OPTIONS");

    pspDebugScreenSetXY(7, 14);

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
    int i;

    pspDebugScreenSetXY(2, 2);
    pspDebugScreenPrintf("HP: ");

    for (i = 0; i < 10; i++)
    {
        if (i < health)
            pspDebugScreenPrintf("#");
        else
            pspDebugScreenPrintf("-");
    }

    pspDebugScreenPrintf(" %d/10", health);
}

void drawXP(int level, int xp)
{
    pspDebugScreenSetXY(32, 2);
    pspDebugScreenPrintf("LV %d XP %d/100", level, xp);
}

void drawGame(
    int playerX,
    int playerY,
    int enemyX,
    int enemyY,
    int health,
    int level,
    int xp,
    int defeated
)
{
    pspDebugScreenClear();

    pspDebugScreenSetXY(2, 1);
    pspDebugScreenPrintf("IB PHENIX - ZONE 01");

    drawHealth(health);
    drawXP(level, xp);

    pspDebugScreenSetXY(2, 5);
    pspDebugScreenPrintf(
        "=========================================================="
    );

    pspDebugScreenSetXY(5, 8);
    pspDebugScreenPrintf("FOREST OF THE PHOENIX");

    pspDebugScreenSetXY(5, 10);
    pspDebugScreenPrintf("MISSION 01 : LE REVEIL DU PHENIX");

    pspDebugScreenSetXY(5, 12);
    pspDebugScreenPrintf("OBJECTIF : ELIMINER L'ENNEMI");

    if (!defeated)
        drawEnemy(enemyX, enemyY);

    drawPlayer(playerX, playerY);

    pspDebugScreenSetXY(2, 22);
    pspDebugScreenPrintf("ANALOG / D-PAD : MOVE");

    pspDebugScreenSetXY(2, 24);
    pspDebugScreenPrintf("X : ATTACK     O : MENU");

    pspDebugScreenSetXY(2, 25);
    pspDebugScreenPrintf("START : EXIT");
}

void levelUpScreen(int level)
{
    SceCtrlData pad;

    pspDebugScreenClear();

    pspDebugScreenSetXY(13, 6);
    pspDebugScreenPrintf("NIVEAU SUPERIEUR !");

    pspDebugScreenSetXY(18, 9);
    pspDebugScreenPrintf("LEVEL %d", level);

    pspDebugScreenSetXY(9, 12);
    pspDebugScreenPrintf("IB PHENIX DEVIENT PLUS FORT");

    pspDebugScreenSetXY(9, 16);
    pspDebugScreenPrintf("NOUVEAU NIVEAU DEBLOQUE");

    pspDebugScreenSetXY(8, 20);
    pspDebugScreenPrintf("APPUIE SUR X POUR CONTINUER");

    while (1)
    {
        sceCtrlPeekBufferPositive(&pad, 1);

        if (pad.Buttons & PSP_CTRL_CROSS)
            break;

        if (pad.Buttons & PSP_CTRL_START)
            sceKernelExitGame();

        sceDisplayWaitVblankStart();
    }
}

void missionComplete(int *level, int *xp)
{
    SceCtrlData pad;

    *xp += 100;

    if (*xp >= 100)
    {
        *xp -= 100;
        (*level)++;

        pspDebugScreenClear();

        pspDebugScreenSetXY(12, 5);
        pspDebugScreenPrintf("MISSION ACCOMPLIE");

        pspDebugScreenSetXY(9, 8);
        pspDebugScreenPrintf("LE REVEIL DU PHENIX");

        pspDebugScreenSetXY(10, 11);
        pspDebugScreenPrintf("+100 XP");

        pspDebugScreenSetXY(10, 13);
        pspDebugScreenPrintf("NOUVEAU NIVEAU !");

        pspDebugScreenSetXY(10, 15);
        pspDebugScreenPrintf("LEVEL %d", *level);

        pspDebugScreenSetXY(7, 19);
        pspDebugScreenPrintf("APPUIE SUR X POUR CONTINUER");

        while (1)
        {
            sceCtrlPeekBufferPositive(&pad, 1);

            if (pad.Buttons & PSP_CTRL_CROSS)
                break;

            if (pad.Buttons & PSP_CTRL_START)
                sceKernelExitGame();

            sceDisplayWaitVblankStart();
        }
    }
    else
    {
        pspDebugScreenClear();

        pspDebugScreenSetXY(12, 6);
        pspDebugScreenPrintf("MISSION ACCOMPLIE");

        pspDebugScreenSetXY(10, 10);
        pspDebugScreenPrintf("+100 XP");

        pspDebugScreenSetXY(10, 13);
        pspDebugScreenPrintf("XP : %d/100", *xp);

        pspDebugScreenSetXY(7, 19);
        pspDebugScreenPrintf("APPUIE SUR X POUR CONTINUER");

        while (1)
        {
            sceCtrlPeekBufferPositive(&pad, 1);

            if (pad.Buttons & PSP_CTRL_CROSS)
                break;

            if (pad.Buttons & PSP_CTRL_START)
                sceKernelExitGame();

            sceDisplayWaitVblankStart();
        }
    }
}

void playGame(int *level, int *xp)
{
    SceCtrlData pad;

    unsigned int oldButtons = 0;

    int playerX = 10;
    int playerY = 12;

    int enemyX = 42;
    int enemyY = 12;

    int health = 10;

    int defeated = 0;
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

        if (!defeated)
        {
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
                if (enemyX >= playerX - 4 &&
                    enemyX <= playerX + 7 &&
                    enemyY >= playerY - 4 &&
                    enemyY <= playerY + 5)
                {
                    defeated = 1;
                }
            }

            if (enemyX >= playerX - 2 &&
                enemyX <= playerX + 5 &&
                enemyY >= playerY - 2 &&
                enemyY <= playerY + 4)
            {
                if (damageTimer == 0)
                {
                    health--;
                    damageTimer = 30;
                }
            }
        }

        if (damageTimer > 0)
            damageTimer--;

        if (health <= 0)
        {
            playerX = 10;
            playerY = 12;
            enemyX = 42;
            enemyY = 12;
            health = 10;
            defeated = 0;
        }

        if (defeated)
        {
            drawGame(
                playerX,
                playerY,
                enemyX,
                enemyY,
                health,
                *level,
                *xp,
                defeated
            );

            sceDisplayWaitVblankStart();

            missionComplete(level, xp);

            if (*level > 1)
                levelUpScreen(*level);

            running = 0;
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
            health,
            *level,
            *xp,
            defeated
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

    int level = 1;
    int xp = 0;

    drawMenu(selected, level, xp);

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

            drawMenu(selected, level, xp);
        }

        if (pressed & PSP_CTRL_DOWN)
        {
            selected++;

            if (selected > 2)
                selected = 0;

            drawMenu(selected, level, xp);
        }

        if (pressed & PSP_CTRL_CROSS)
        {
            if (selected == 0)
            {
                playGame(&level, &xp);
                drawMenu(selected, level, xp);
            }

            if (selected == 1)
            {
                pspDebugScreenClear();

                pspDebugScreenSetXY(10, 6);
                pspDebugScreenPrintf("OPTIONS");

                pspDebugScreenSetXY(8, 9);
                pspDebugScreenPrintf("IB PHENIX PSP");

                pspDebugScreenSetXY(8, 11);
                pspDebugScreenPrintf("LEVEL : %d", level);

                pspDebugScreenSetXY(8, 13);
                pspDebugScreenPrintf("XP : %d/100", xp);

                pspDebugScreenSetXY(7, 17);
                pspDebugScreenPrintf("PRESS O TO RETURN");

                while (1)
                {
                    sceCtrlPeekBufferPositive(&pad, 1);

                    if (pad.Buttons & PSP_CTRL_CIRCLE)
                        break;

                    if (pad.Buttons & PSP_CTRL_START)
                        sceKernelExitGame();

                    sceDisplayWaitVblankStart();
                }

                drawMenu(selected, level, xp);
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
