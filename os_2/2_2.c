#include <pthread.h>
#include <stdio.h>
#include <wiringPi.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define LED1 7
#define LED2 0
#define LED3 2
#define LED4 3

void *thread_function(void* arg) {
    int* iarg = (int*)arg;
    wiringPiSetup();
    pinMode(LED1, OUTPUT);
    pinMode(LED2, OUTPUT);
    pinMode(LED3, OUTPUT);
    pinMode(LED4, OUTPUT);
    int c = 0;
    for(;;) {
        int nr = *iarg;
        digitalWrite(LED1, (c&1) > 0 ? HIGH:LOW);
        digitalWrite(LED2, (c&2) > 0 ? HIGH:LOW);
        digitalWrite(LED3, (c&4) > 0 ? HIGH:LOW);
        digitalWrite(LED4, (c&8) > 0 ? HIGH:LOW);
        if(c > nr) {
            c--;
        } else if(c < nr) {
            c++;
        }
        usleep(200000);
    }
    return NULL;
}

void *calculation(void* arg) {
    clock_t start_time, end_time;
    double cpu_time_used;
    start_time = clock();
    FILE *fp;
    double num;
    double sum = 0;
    char *str = (char*)arg;
    fp = fopen(str, "r");

    if (fp == NULL) { // Check if the file was opened successfully
        printf("Error opening file.\n");
        exit(1);
    }

    while (fscanf(fp, "%lf", &num) == 1) { // Read a double from the file
        num = atan(tan(num));
        sum += num;
    }

    fclose(fp); // Close the file
    end_time = clock();
    cpu_time_used = ((double) (end_time - start_time)) / CLOCKS_PER_SEC;
    printf("%f\n", sum);
    printf("%f\n", cpu_time_used);
    return NULL;
}

int main(int argc, char *argv[]) {
    int num = 0;
    int input;
    pthread_t thread;
    pthread_create(&thread, NULL, thread_function, &num);
   
    char *str = argv[1];

    pthread_t thread2;
    pthread_create(&thread2, NULL, calculation, str);

    for(;;) {
        scanf("%d", &input);
        if(input < 0 || input > 15) {
            printf("incorrect number given\n");
            exit(1);
        }
        num = input;
    }
    return 0;
}