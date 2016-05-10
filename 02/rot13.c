/*
 * Füge hier die Header hinzu
 * schau dir dafür die man pages an.
 */
#include <stdio.h>
#include <string.h>
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

// Upon successful completion, returns 0, otherwise errno
int print_rot13(int fd) {
    ssize_t i, len;
    uint8_t buf[BUF_SIZE];

    // Liest häppchenweise die Inhalte der übergebenen Dateien, rotiert die
    // Buchstaben, und schreibt das ganze dann in stdout. Wir verwenden hier
    // nicht printf, da es nicht mit Null-Bytes umgehen kann.
    while ((len = read(fd, &buf, BUF_SIZE))) {
        if (len < 0)
            return errno;

        for (i = 0; i < len; i++) {
            if (buf[i] >= 65 && buf[i] <= 90)
                buf[i] = (buf[i] - 65 + 13) % 26 + 65;
            else if (buf[i] >= 97 && buf[i] <= 122)
                buf[i] = (buf[i] - 97 + 13) % 26 + 97;
        }

        if (write(1, buf, len) == -1)
            return errno;
    }

    return 0;
}

void print_usage(char* name) {
    printf("Usage: %s [FILE]\n", name);
    printf("Print the ROT13 encoded/decoded content of FILE\n");
}

void print_error(char* name, char* msg, int errnum) {
    char *errormsg = strerror(errnum);
    fprintf(stderr, "%s: %s: %s\n", name, msg, errormsg);
}

int main(int argc, char* argv[]) {
    int fd, ret;

    if (argc < 2) {
        print_usage(argv[0]);
        // Normalerweise würde ich return 1 verwenden, aber die Aufgabenstellung
        // erfordert ja exit()
        exit(1);
    }

    fd = open(argv[1], O_RDONLY);

    if (fd == -1) {
        int errsv = errno;
        print_error(argv[0], "cannot open() file", errsv);
        exit(errsv);
    }

    ret = print_rot13(fd);
    
    if (ret != 0) {
        // Wir ignorieren close()-Fehler hier, da wir hier ohnehin abbrechen.
        close(fd);
        print_error(argv[0], "cannot print_rot13() file", ret);
        exit(ret);
    }

    if (close(fd) == -1) {
        int errsv = errno;
        print_error(argv[0], "cannot close() file", errsv);
        exit(errsv);
    }

    return 0;
}


