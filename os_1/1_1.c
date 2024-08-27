#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
    int studentNumber = 5522897;
    char name[] = "Vincent Pikand";
    printf("Student number: %d, student name: %s\n", studentNumber, name);
    // get pid of the process
    printf("Process ID: %d\n", getpid());
    return 0;
}