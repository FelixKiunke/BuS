#define _GNU_SOURCE
#define USE_NAMED_SEMAPHORES

#include <sys/types.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdint.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>


void fahreAufPlattform(pid_t wagenNummer) {
    fprintf(stdout, "Wagen %d fährt auf Plattform\n", wagenNummer);
}

void oeffneTueren(pid_t wagenNummer) {
    fprintf(stdout, "Wagen %d öffnet die Türen\n", wagenNummer);
}

void schliesseTueren(pid_t wagenNummer) {
    fprintf(stdout, "Wagen %d schließt die Türen\n", wagenNummer);
}

void fahreVonPlattform(pid_t wagenNummer) {
    fprintf(stdout, "Wagen %d verlässt die Plattform\n", wagenNummer);
}

// Mit 1 initialisiert, gibt Information über Nutzung der Plattform
sem_t* plattform_lock;

// Zählsemaphor für die wartenden Besucher, mit 0 initialisiert
sem_t* verfuegbare_sitze;

// Zählsemaphor für die eingestiegenen Besucher, mit 0 initialisiert
sem_t* eingestiegene_besucher;

void AnkunftWagen() {
    pid_t wagen = getpid();
    
    fprintf(stdout, "Wagen %d kommt an\n", wagen);
    
    // aufplattform fahren wenn frei
    sem_wait(plattform_lock);
    fahreAufPlattform(wagen);
    
    oeffneTueren(wagen);
    
    // nutzer einsteigen lassen
    sem_post(verfuegbare_sitze);
    sem_post(verfuegbare_sitze);
    sem_wait(eingestiegene_besucher);
    sem_wait(eingestiegene_besucher);
    // türen schließen wenn 2 Besucher drin
    schliesseTueren(wagen);
    
    fahreVonPlattform(wagen);
    sem_post(plattform_lock);
}



void betrete_wagen(int besucherNummer) {
    fprintf(stdout, "Besucher %d steigt in Wagen\n", besucherNummer);
}

sem_t* besucher_lock;
uint32_t* besucher_nummer;

void AnkunftBesucher() {
    uint32_t besucher;
    
    
    sem_wait(besucher_lock);
    besucher = (*besucher_nummer)++;
    fprintf(stdout, "Besucher %d kommt an\n", besucher);
    sem_post(besucher_lock);
    
    
    // warte auf freien sitzplatz
    sem_wait(verfuegbare_sitze);
    betrete_wagen(besucher);
    sem_post(eingestiegene_besucher);
}




int shouldEnd = 0;

/* SIGINT signal handler */
void signal_handler(int signalNum) {
    if(signalNum == SIGINT) {
        printf("Ok, schalte ab!\n");
        shouldEnd = 1;
    }
}


int main(void) {
    
#ifdef USE_NAMED_SEMAPHORES
    
    // zahlen die mit 0 starten werden octal interpretiert :)
    
    besucher_lock = sem_open("/besucher_lock", O_CREAT|O_EXCL, 0644, 1); /* Note O_EXCL */
    sem_unlink("/besucher_lock");
    plattform_lock = sem_open("/plattform_lock", O_CREAT|O_EXCL, 0644, 1); /* Note O_EXCL */
    sem_unlink("/plattform_lock");
    verfuegbare_sitze = sem_open("/verfuegbare_sitze", O_CREAT|O_EXCL, 0644, 0); /* Note O_EXCL */
    sem_unlink("/verfuegbare_sitze");
    eingestiegene_besucher = sem_open("/eingestiegene_besucher", O_CREAT|O_EXCL, 0644, 0); /* Note O_EXCL */
    sem_unlink("/eingestiegene_besucher");
    
#else
    /* wir müssen die semaphore mit anderen prozessen teilen
     * durch ein fork erhält der andere Prozess nur eine Kopie
     * wir müssen aber auf den selben Daten arbeiten...
     *
     * Wir nutzen mmap um uns geteilten speicher zu besorgen
     * auf dem wir lesen und schreiben dürfen
     * MAP_ANONYMOUS sorgt dafür dass wir nicht assoziierten
     * speicher bekommen also keine Datei oder so mappen
     * müssen einfach HEAP memory
     */
    besucher_lock = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE,
                         MAP_ANONYMOUS | MAP_SHARED, 0, 0);
    if (!besucher_lock) {
        perror("out of memory\n");
        exit(1);
    }
    // initialisiere semaphore mit 1
    if(sem_init(besucher_lock, 1, 1) == -1) {
        perror("Semaphore init\n");
        exit(1);
    }
    
    
#endif
    
    
    // und noch geteilten speicher um besuchernummern zu vergeben
    besucher_nummer = mmap(NULL, sizeof(uint32_t), PROT_READ | PROT_WRITE,
                           MAP_ANONYMOUS | MAP_SHARED, 0, 0);
    // der 1. besucher ist natürlich nummer0
    *besucher_nummer = 0;
    
    
    
    
    /* catch interrupts */
    signal(SIGINT, signal_handler);
    
    srand(time(NULL));
    
    // main loop
    while (!shouldEnd) {
        
        // wagen und besucher erzeugen
        
        
        
        int car_or_user = rand();
        
        pid_t f_pid = fork();
        if (f_pid == 0) {
            
            /* Kinder sollen nicht auf SIG_INT hören */
            signal(SIGINT, SIG_DFL);
            
            /* wird das ein wagen oder ein besucher? */
            if(car_or_user < (RAND_MAX/2)) {
                // sagen wir mal das hier ist ein wagen
                
                AnkunftWagen();
            }else {
                // dann ist das ein besucher
                AnkunftBesucher();
            }
            // und tschüss
            exit(0);
        }else {
            // Im Vater
            sleep(1);
        }
        
    }
    
    
    
    
#ifdef USE_NAMED_SEMAPHORES
    sem_close(besucher_lock);
    sem_close(plattform_lock);
    sem_close(verfuegbare_sitze);
    sem_close(eingestiegene_besucher);
#else
    // semaphore töten
    sem_destroy(besucher_lock);
    // dran denken den speicher auch wieder abzugeben
    munmap(besucher_lock, sizeof(sem_t));
    
    munmap(besucher_nummer, sizeof(uint32_t));
    
    
#endif

    
    
}

