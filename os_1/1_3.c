#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <dirent.h>

int second() {
    system("mkdir test");
    system("ls");
    mkdir("test2", 0777);
    struct dirent *d;
    DIR *dir = opendir(".");
    while((d = readdir(dir)) != NULL) {
        if(d->d_name[0] == '.') continue;
        printf("%s ", d->d_name);
    }
    printf("\n");
    return 0;
}

int main(int argc, char *argv[]) {
	int pid = 0;
	pid = fork();
	if (pid == 0) {
        printf("Child\n");
        second();
		exit(0);
	} else {
		printf("Parent\n");
		wait(NULL);
		printf("Done waiting.\n");
		exit(0);
	}
	return 1;
}
