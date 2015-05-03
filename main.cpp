#include "MySystem.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow) {
    MySystem *system;

    system = new MySystem;
    if (!system) {
        return 0;
    }

    if (system->initalize(800,600)) {
        system->run();
    }

    system->shutdown();
    delete system;
    system = NULL;

    return 0;
}