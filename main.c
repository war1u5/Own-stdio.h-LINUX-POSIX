#define _CRT_SECURE_NO_WARNINGS
#include "so_stdio.h"
#include "so_tools.h"
#define _DEBUG_

int main(){
    SO_FILE* file = so_fopen("file.txt", "w");
    char buf[]="NICE!";
    so_fwrite(buf, 1, strlen(buf), file);
    so_fclose(file);

    return 0;
}