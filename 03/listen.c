#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct stud_type_ {                  /* Struktur des Datensatzes:    */
    int matnum;                              /* Matrikelnummer, Vor-         */
    char vorname[20];                        /* und Nachname sind Eintraege. */
    char nachname[20];                       /* Die Datenbank ist eine       */
    struct stud_type_ *next_student;         /* einfach verkettete Liste     */
} stud_type;


/* Ist die Datenbank leer?      */
bool is_empty(stud_type** liste) {
    return *liste == NULL;
}


/* Einfuegen eines Elementes    
 * 
 * Bekommt einen Zeiger auf einen Zeiger der auf das 1. Element der Liste zeigt
 * Bekommt MatNr, Vorname und Nachname des Studenten der Eingefügt werden soll
 *
 */
void enqueue(stud_type** studenten_liste, int matnum, char vorname[20], char nachname[20])
{
    /* Deklariere benötigte Variablen */
    stud_type *insert;
    stud_type *curr;
    /* Hol Speicher auf dem Heap an für den neuen Listen Eintrag */
    insert = (stud_type*)malloc(sizeof(stud_type));
    if(insert == NULL) {
        // Speicher konnte nicht allokiert werden
        return;
    }
    /* Befüll den Speicher */
    strcpy(insert->vorname, vorname);
    strcpy(insert->nachname, nachname);
    insert->matnum = matnum;
    insert->next_student = NULL;
    /* Füg den neuen Eintrag in die Liste ein */
    /* Ist die Liste leer ? */
    /* Sortier den Studenten aufsteigend nach Matrikelnummer ein */
    if(is_empty(studenten_liste)) {
        *studenten_liste = insert;
    // Falls der neue Eintrag der zum ersten Eintrag wird
    } else if(matnum < (*studenten_liste)->matnum) {
        insert->next_student = *studenten_liste;
        *studenten_liste = insert;
    } else {
        curr = *studenten_liste;
        while((curr->next_student != NULL) && (curr->next_student)->matnum < matnum) {
            curr = curr->next_student;
        }
        // Falls der neue Eintrag die "größte" Matrikelnummer hat
        if(curr->next_student == NULL){
            curr->next_student = insert;
        // Falls die Matrikelnummer bereits in der Liste ist wird der Eintrag ersetzt
        } else if( (curr->next_student)->matnum == matnum) {
            insert->next_student = (curr->next_student)->next_student;
            free(curr->next_student);
            curr->next_student = insert;
        // Falls "mitten" in der Liste eingefügt werden muss
        } else {
            insert->next_student = curr->next_student;
            curr->next_student = insert;
        }
    }
}

/* Löschen eines Elementes. 
 * 
 * Bekommt einen Zeiger auf einen Zeiger der auf das 1. Element der Liste zeigt
 * Bekommt die MatNr des Studenten der zu löschen ist
 *
 * Gibt 0 für einen Fehler zurück
 * Gibt 1 für Erfolg zurück
 */
int dequeue(stud_type** studenten_liste, int matnum)
{
    /* Deklariere benötigte Variablen */
    stud_type *curr;
    /* Prüfe Randbedingungen */
    // Liste ist leer
    if(is_empty(studenten_liste)) {
        return 0;
    }
    /* Finde den Studenten */
    //Das erste Element muss gelöscht werden
    if((*studenten_liste)->matnum == matnum) {
        //Insbesondere kann die Liste einelementig gewesen sein, dann ist next_student null und es entsteht die leere Liste
        stud_type *temp = (*studenten_liste)->next_student;
        free(*studenten_liste);
        *studenten_liste = temp;
        return 1;
    }
    curr = *studenten_liste;
    // Falls gefunden befindet sich der gesuchte Student nach der loop in curr->next_student
    while((curr->next_student != NULL) && (curr->next_student)->matnum != matnum) {
        curr = curr->next_student;
    }
    /* Lösche den Studenten und gibt den Speicher frei */
    //Bei erfolgloser Suche wurde einmal durch die Liste iteriert wodurch curr->next NULL ist
    if(curr->next_student == NULL) {
        return 0;
    }
    // Das Element wurde nicht gefunden (curr->next ist NULL), else befindet es sich in curr->next
    else {
        stud_type *temp = (curr->next_student)->next_student;
        // Insbesondere darf (curr->next_student)->next_student auch NULL sein, also war das zu löschende Element das letzte
        free(curr->next_student);
        curr->next_student = temp;
        return 1;
    }
    /* Was muss passieren wenn das 1. Element gelöscht wird? */
    /* Was ist wenn es nicht in der Liste ist? */
    /* ... */
}

/* Auslesen eines Elementes 
 *
 * Bekommt zeiger auf den Listenstart
 * Bekommt matnr des Studenten der ausgelesen werden soll
 *
 * Schreibt Vorname und Nachname in vorname und nachname
 */
int get_student(stud_type* studenten_liste, int matnum, char vorname[20], char nachname[20])
{
    stud_type *curr;

    /* Durchmustern der DB */
    curr = studenten_liste;
    while ((curr != NULL) && (curr->matnum < matnum)) {
        curr = curr->next_student;
    }

    if ((curr == NULL) || (curr->matnum != matnum)) {
        /* Rückgabewert: Fehler */
        return 0;
    } else {
       strcpy(vorname, curr->vorname);
       strcpy(nachname, curr->nachname);
        /* Rückgabewert: OK */
       return 1;
   }
}

// Aufgabenteil b)
// Datenstruktur für eine sortier_liste
typedef struct sort_stud_type_ {
    stud_type *data;
    struct sort_stud_type_ *next_entry;
} sort_stud_type;

void enqueue_sort_list(sort_stud_type** sortier_liste, stud_type* student, int (*compare) (stud_type* a, stud_type* b));
/* Erstellen einer Sortierliste zu einer entsprechenden Studentenliste. 
 * 
 * Bekommt einen Zeiger auf einen Zeiger auf eine einelementige Sortierliste, der danach auf das 1. Element der Sortierliste zeigt
 * Bekommt einen Zeiger auf das erste Element einer Studentenliste, welche als Grundlage der Sortierliste dient
 * Bekommt eine Vergleichsfunktion, von der aus die Sortierliste aufsteigend sortiert wird
 * 
 * Gibt 0 für einen Fehler zurück, 
 * Gibt 1 für Erfolg zurück
 */
int create_sort_list (sort_stud_type** sortier_liste, stud_type* studenten_liste, int (*compare) (stud_type* a, stud_type* b))
{
    if(studenten_liste == NULL) {
        return 0;
    }
    //Mach Platz für die neue Sortierliste
    free(*sortier_liste);
    *sortier_liste = NULL;
    stud_type *curr = studenten_liste;
    while(curr != NULL) {
        enqueue_sort_list(sortier_liste, curr, compare);
        curr = curr->next_student;
    }
    return 1;
}

// Hilfsfunktion, fügt in eine Sortierliste ein Element ein, analog zur vorherigen enqueue
void enqueue_sort_list(sort_stud_type** sortier_liste, stud_type* student, int (*compare) (stud_type* a, stud_type* b)) {
    sort_stud_type *insert;
    sort_stud_type *curr;
    insert = (sort_stud_type*)malloc(sizeof(sort_stud_type));
    if(insert == NULL) {
        // Speicher konnte nicht allokiert werden
        return;
    }
    insert->data = student;
    insert->next_entry = NULL;
    if(*sortier_liste == NULL) {
        *sortier_liste = insert;
    } else if(compare(student, (*sortier_liste)->data) == -1) {
        insert->next_entry = *sortier_liste;
        *sortier_liste = insert;
    } else {
        curr = *sortier_liste;
        while((curr->next_entry != NULL) && compare((curr->next_entry)->data, student) == -1) {
            curr = curr->next_entry;
        }
        if(curr->next_entry == NULL){
            curr->next_entry = insert;
        } else if( compare((curr->next_entry)->data, student) == 0) {
            insert->next_entry = (curr->next_entry)->next_entry;
            free(curr->next_entry);
            curr->next_entry = insert;
        } else {
            insert->next_entry = curr->next_entry;
            curr->next_entry = insert;
        }
    }
}

/* Funktion zum Vergleichen nach Vornamen
 * 
 * Bekommt zwei Studentenlistenelemente
 * 
 * Gibt -1 zurück, falls der Vorname von a alphabetisch vor dem von b ist
 * Gibt 0 zurück, falls der Vorname von a gleich dem von b ist
 * Gibt 1 zurück, falls der Vorname von a alphabetisch nach dem von b ist
 * Gibt -2 zurück, falls ein Fehler auftritt
 * 
 * Funktionalität nicht gewährleistet falls die char arrays von a oder b auch nicht-Buchstaben enthalten außer '\0'
 * Groß- und Kleinschreibung werden beachtet, im Zweifelsfall ist klein geschriebenes alphabetisch ebenfalls kleiner
 */
int compare_firstname(stud_type* a, stud_type* b) {
    //Should not happen
    if(a == NULL || b == NULL) {
        return -2;
    }
    for(int i = 0; i<20; i++) {
        if(a->vorname[i] < b->vorname[i]) {
            return -1;
        } else if (a->vorname[i] > b->vorname[i]) {
            return 1;
        }
    }
    return 0;
}

/* Funktion zum Vergleichen nach Nachnamen
 * 
 * Bekommt zwei Studentenlistenelemente
 * 
 * Gibt -1 zurück, falls der Nachname von a alphabetisch vor dem von b ist
 * Gibt 0 zurück, falls der Nachname von a gleich dem von b ist
 * Gibt 1 zurück, falls der Nachname von a alphabetisch nach dem von b ist
 * Gibt -2 zurück, falls ein Fehler auftritt
 * 
 * Funktionalität nicht gewährleistet falls die char arrays von a oder b auch nicht-Buchstaben enthalten außer '\0'
 * Groß- und Kleinschreibung werden beachtet, im Zweifelsfall ist klein geschriebenes alphabetisch ebenfalls kleiner
 */
int compare_lastname(stud_type* a, stud_type* b) {
    //Should not happen
    if(a == NULL || b == NULL) {
        return -2;
    }
    for(int i = 0; i<20; i++) {
        if(a->nachname[i] < b->nachname[i]) {
            return -1;
        } else if (a->nachname[i] > b->nachname[i]) {
            return 1;
        }
    }
    return 0;
}

/* Test der Listenfunktionen  */
int main(void)                                     
{
    /* Initialisierung der Datenbank */
    stud_type *studenten_liste = NULL;
    /* platz für vorname */
    char vorname[20];
    /* platz für nachname */
    char nachname[20];
    /* zeiger für iteration */
    stud_type *curr;

    printf(">>> Fuege neuen Studenten in die Liste ein: Eddard Stark [1234] ...\n");
    enqueue(&studenten_liste, 1234, "Eddard", "Stark");
    printf(">>> Fuege neuen Studenten in die Liste ein: Jon Snow [5678] ...\n");
    enqueue(&studenten_liste, 5678, "Jon", "Snow");
    printf(">>> Fuege neuen Studenten in die Liste ein: Tyrion Lannister [9999] ...\n");
    enqueue(&studenten_liste, 9999, "Tyrion", "Lannister");
    printf(">>> Test, ob die Matrikelnummer 1289 bereits erfasst wurde ...\n");   
    
    if(get_student(studenten_liste, 815, vorname, nachname)) {
        printf("    Matrikelnummer %4i: %s %s\n", 1289, vorname, nachname);
    } else {
        printf("    Matrikelnummer %4i ist unbekannt\n", 1289);
    }

    printf(">>> Fuege neuen Studenten in die Liste ein: Daenerys Targaryen [1289] ...\n");
    enqueue(&studenten_liste, 1289, "Daenerys", "Targaryen");
    printf(">>> Loesche die Matrikelnummer 1234 ...\n");

    if(dequeue(&studenten_liste, 1234)) {
        printf("    Matrikelnummer %4i geloescht\n", 1234);
    } else {
        printf("    Matrikelnummer %4i war nicht erfasst\n", 1234);
    }

    printf(">>> Test ob die Studentenliste leer ist ...\n");

    if(is_empty(&studenten_liste)) {
        printf("    Die Studentenliste ist leer \n");
    } else {
        printf("    Die Studentenliste ist nicht leer \n");
    }

    printf(">>> Test, ob die Matrikelnummer 5678 bereits erfasst wurde ...\n");

    if(get_student(studenten_liste, 5678, vorname, nachname)) {
        printf("    Matrikelnummer %4i: %s %s\n", 5678, vorname, nachname);
    } else {
        printf("    Matrikelnummer %4i ist unbekannt\n", 5678);
    }
 
    printf(">>> Loesche die Matrikelnummer 9998 ...\n");
 
    if(dequeue(&studenten_liste, 9998)) {
        printf("    Matrikelnummer %4i geloescht\n", 9998);
    } else {
        printf("    Matrikelnummer %4i war nicht erfasst\n", 9998);
    }

    printf(">>> Loesche die Matrikelnummer 5678 ...\n");

    if(dequeue(&studenten_liste, 5678)) {
        printf("    Matrikelnummer %4i geloescht\n", 5678);
    } else {
        printf("    Matrikelnummer %4i war nicht erfasst\n", 5678);
    }

    printf(">>> Gebe alle erfassten Studenten aus ...\n");
    curr = studenten_liste;

    while(curr != NULL) {
        printf("    Matrikelnummer %4i: %s %s\n", curr->matnum, curr->vorname, curr->nachname);
        curr = curr->next_student;
    }
    
    return 0;
}
