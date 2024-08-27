#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

int main() {
    system("mkdir test");
    system("ls");
    mkdir("test2", 0777);
    struct dirent *d;
    DIR *dir = opendir(".");
    while((d = readdir(dir)) != NULL) {
        if(d->d_name[0] == '.') continue;
        printf("%s ", d->d_name);
    }
    closedir(dir);
    printf("\n");
    return 0;
}