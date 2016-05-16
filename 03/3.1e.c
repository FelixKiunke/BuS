#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

unsigned int countA = 0;
unsigned int countB = 0;
unsigned int countC = 0;

void* printCounts(void* arg){
    while(1){
        printf("Anzahl 'A': %d\nAnzahl 'B': %d\nAnzahl 'C': %d\n-----------------\n",countA, countB, countC);
        sleep(4);
    }
}

// Läuft selber als Endlosschleife, da das Programm aus a) dies ebenfalls tut
int main(){
    // Erstelle den Print Thread
    pthread_t printThread;
    if( pthread_create(&printThread, NULL, &printCounts, NULL) ){
        printf("Konnte Thread nicht starten, Programm wird beendet\n");
        exit(1);
    }
    // Einlesen der chars erfolgt auf dem main thread in Endlosschleife
    while(1){
        switch( (char)getchar() ){
            case 'A': countA++; break;
            case 'B': countB++; break;
            case 'C': countC++; break;
            default:
                printf("Etwas lief schief, beende Programm.\n");
                pthread_cancel(printThread);
                pthread_join(printThread, NULL);
                exit(1);
        }
    }
}
