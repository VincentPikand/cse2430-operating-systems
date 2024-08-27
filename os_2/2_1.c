#include <pthread.h>
#include <stdio.h>
#include <wiringPi.h>
#include <unistd.h>
#include <stdlib.h>

#define LED1 7
#define LED2 0
#define LED3 2
#define LED4 3

void *computation(void* arg) {
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

void *listener(void* arg) {
    int* iarg = (int*)arg;
    int input;
    for(;;) {
        scanf("%d", &input);
        if(input < 0 || input > 15) {
            printf("incorrect number given\n");
            exit(1);
        }
        *iarg = input;
    }
}

int main() {
    int num = 0;
    pthread_t computation_thread;
    pthread_t listener_thread;

    pthread_create(&listener_thread, NULL, listener, &num);
    pthread_create(&computation_thread, NULL, computation, &num);

    pthread_join(computation_thread, NULL);
    pthread_join(listener_thread, NULL);
    return 0;
}