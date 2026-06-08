#include <string.h>
#include "handshake.h"

#define PROJECT_ID "DTU_CONTROLLER"

const char* get_project_id(void)
{
    return PROJECT_ID;
}

int is_handshake_command(const char* command)
{
    return strcmp(command, "WHO_ARE_YOU?") == 0;
}