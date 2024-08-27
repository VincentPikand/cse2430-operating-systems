#include <pthread.h>
#include <softPwm.h>
#include <unistd.h>
#include <wiringPi.h>

#define LED1 7
#define LED2 0
#define LED3 2
#define LED4 3

pthread_barrier_t barrier; 

void *fadein() {
    for(int i = 0; i <= 100; i++) {
        softPwmWrite(LED1, i);
        softPwmWrite(LED2, i);
        softPwmWrite(LED3, i);
        softPwmWrite(LED4, i);
        usleep(10000);
    }
    pthread_barrier_wait(&barrier);
    return NULL;
}
void *fadeout() {
    pthread_barrier_wait(&barrier);
    for(int i = 100; i >= 0; i--) {
        softPwmWrite(LED1, i);
        softPwmWrite(LED2, i);
        softPwmWrite(LED3, i);
        softPwmWrite(LED4, i);
        usleep(10000);
    }
    return NULL;
}

int main() {
    wiringPiSetup();
    softPwmCreate(LED1, 0, 100);
    softPwmCreate(LED2, 0, 100);
    softPwmCreate(LED3, 0, 100);
    softPwmCreate(LED4, 0, 100);

    pthread_t thread1;
    pthread_t thread2;
    pthread_barrier_init(&barrier, NULL, 2);

    pthread_create(&thread1, NULL, fadein, NULL);
    pthread_create(&thread2, NULL, fadeout, NULL);
    
    // not sure why this is needed
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    return 0;
}