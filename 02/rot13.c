/*
 * Füge hier die Header hinzu
 * schau dir dafür die man pages an.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>

 #define BUF_SIZE 1024


/*
 * Dies ist ein mögliches Grundgerüst für eine
 * Funktion die etwas rot13 rotiertes ausgiebt.
 * Vergiss nicht, C strings müssen '\0' (Null) terminiert
 * sein, wenn du Sie mit printf ausgeben willst!
 * 
 * !Du musst diese Methode aber nicht nutzen.!
 *
 * Beachte nur Buchstaben, keine Sonderzeichen.
 * Der Modulo operator könnte von Hilfe sein ;)
 */
void print_rot13(int fd) {
    uint8_t buf[BUF_SIZE];

    ssize_t len, i;
    while ((len = read(fd, &buf, BUF_SIZE))) {
        for (i = 0; i < len; i++) {
            if (buf[i] >= 65 && buf[i] <= 90)
                buf[i] = (buf[i] - 65 + 13) % 26 + 65;
            else if (buf[i] >= 97 && buf[i] <= 122)
                buf[i] = (buf[i] - 97 + 13) % 26 + 97;
        }

        write(1, buf, len);
        fflush(stdout);
    }
}

void print_usage(char* name) {
    printf("Usage: %s [FILE]\n", name);
    printf("Print the ROT13 encoded/decoded content of FILE\n");
}

/*
 * Dies ist die main Methode
 */
int main(int argc, char** argv) {
    /*
     * Deklarier deine benötigten Variablen
     */
    int fd;

    /*
     * Vergiss nicht die command line zu prüfen
     */
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }

    fd = open(argv[1], O_RDONLY);

    if (fd == -1)
        goto fail;

    /*
     * Öffne die Datei und ließ sie ein,
     * gib den Inhalt auf dem Bildschirm aus.
     * Vergiss nicht Fehlerfälle abzufangen.
     */
    
    print_rot13(fd);

    close(fd);
    
    /*
     * Beende das Program sorgfältig.
     */
    return 0;

fail:
    printf("error %d", errno);
    return 1;
}


