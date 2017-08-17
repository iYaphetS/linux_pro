#include "../header.h"
int main()
{
    char tmp_path[256];

    strcpy(tmp_path, "/tmp/save_the_world_XXXXXX");

    mkstemp(tmp_path);

    printf("%s\n", tmp_path);
}
