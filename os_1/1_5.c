#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#define LED1 7
#define LED2 0
#define LED3 2
#define LED4 3

int main(int argc, char *argv[]) {
    if(argc != 2) {
        printf("incorrect amount of arguments given");
        exit(1);
    }

    int nr = atoi(argv[1]);
    if(nr < 0 || nr > 15) {
        printf("incorrect number given");
        exit(1);
    }
	wiringPiSetup();
    pinMode(LED1, OUTPUT);
    pinMode(LED2, OUTPUT);
    pinMode(LED3, OUTPUT);
    pinMode(LED4, OUTPUT);

    digitalWrite(LED1, (nr&1) > 0 ? HIGH:LOW);
    digitalWrite(LED2, (nr&2) > 0 ? HIGH:LOW);
    digitalWrite(LED3, (nr&4) > 0 ? HIGH:LOW);
    digitalWrite(LED4, (nr&8) > 0 ? HIGH:LOW);
    // sleep(2);
    // digitalWrite(LED1, LOW);
    // digitalWrite(LED2, LOW);
    // digitalWrite(LED3, LOW);
    // digitalWrite(LED4, LOW);
    return 0;
}