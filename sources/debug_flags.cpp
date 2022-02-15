#include "debug_flags.h"

bool DebugFlags::perspective_switching = false;
bool DebugFlags::wireframe_switching = false;

void DebugFlags::update()
{
    if (mouse->common_button_down() & Mouse::BTN_F1) { perspective_switching ? perspective_switching = false : perspective_switching = true; }
    if (mouse->common_button_down() & Mouse::BTN_F2) { wireframe_switching ? wireframe_switching = false : wireframe_switching = true; }
}