#include <pthread.h>
#include <wiringPi.h>
#include "queue.h"
#include <stdio.h>
#include <unistd.h>
#include <softPwm.h>


#define LED1 7
#define LED2 0
#define LED3 2
#define LED4 3

pthread_barrier_t barrier; 

// This struct is used to pass the LED number and the queue to the thread
typedef struct LED {
    int ledNumber;
    Queue* q;
} LED;

void* led(void *arg) {
    pthread_barrier_wait(&barrier);

    LED* led = (LED*) arg;
    Queue* q = led->q;

    int brightness, duration;
    int qsize = queueSize(q);
    for(int i = 0; i < qsize; i++) {
        removeFromQueue(q, &brightness, &duration);
        // printf("led: %d, brightness: %d, duration: %d\n", led->ledNumber, brightness, duration);
        softPwmWrite(led->ledNumber, brightness);
        usleep(duration * 1000);
    }
    return NULL;
}

void* userinput(void* arg) {
    // Queue** means that the argument is a pointer to an array of pointers to queues
    Queue** arr = (Queue**) arg;
    int led, brightness, duration;
    
    while(scanf("%d %d %d", &led, &brightness, &duration) == 3) {
        addToQueue(arr[led], brightness, duration);
    }
    pthread_barrier_wait(&barrier);
    return NULL;
}


int main(){
    wiringPiSetup();

    // first deal with reading the command line arguments
    Queue q, q2, q3, q4;
    Queue* qArray[] = {&q, &q2, &q3, &q4};
    for(int i = 0; i < 4; i++) {
        initQueue(qArray[i]);
    }
    pthread_barrier_init(&barrier, NULL, 5);
    pthread_t input;
    pthread_create(&input, NULL, userinput, (void*)qArray); // is the cast to (void*) necessary?

    // now deal with the LEDs
    pthread_t led1, led2, led3, led4;
    softPwmCreate(LED1, 0, 100);
    softPwmCreate(LED2, 0, 100);
    softPwmCreate(LED3, 0, 100);
    softPwmCreate(LED4, 0, 100);

    LED led1Struct = {LED1, &q};
    LED led2Struct = {LED2, &q2};
    LED led4Struct = {LED4, &q4};
    LED led3Struct = {LED3, &q3};

    pthread_create(&led1, NULL, led, (void*) &led1Struct);
    pthread_create(&led2, NULL, led, (void*) &led2Struct);
    pthread_create(&led3, NULL, led, (void*) &led3Struct);
    pthread_create(&led4, NULL, led, (void*) &led4Struct);

    pthread_join(input, NULL);
    pthread_join(led1, NULL);
    pthread_join(led2, NULL);
    pthread_join(led3, NULL);
    pthread_join(led4, NULL);

    return 0;
}