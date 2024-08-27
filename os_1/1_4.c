#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <dirent.h>
#include <pthread.h>
  
// A normal C function that is executed as a thread 
// when its name is specified in pthread_create()
void *second(void *vargp)
{
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
    return NULL;
}
   
int main()
{
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, second, NULL);
    pthread_join(thread_id, NULL);
    printf("thread finished\n");
    // A thread exists inside a process, using the same memory space.
    // A thread can run in parallel with other threads in the same process.

    exit(0);
}