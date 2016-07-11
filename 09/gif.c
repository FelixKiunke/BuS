/* aktiviere aktuelle (2008-ext9) POSIX extensions, z.b. getline, getaddrinfo */
#define _POSIX_C_SOURCE 200809L

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <stdbool.h>

#include "cJSON.h"

/* compilieren mit: gcc gif.c cJSON.c -pedantic -std=c11 -Wextra -Wall -lm -o gif_download */



/**
 * Verbindet einen Socket zu dem angegebenen Host auf Port 80
 *
 * @param host Der Host mit dem sich verbunden werden soll
 * @return Falls erfolgreich den verbundenen Socket, bei Fehler einen negativen Wert
 */
int connect_socket(char* host) {
    /*****   TODO:    ****/
    // Folgende Seite kann sehr hilfreich sein: http://www.beej.us/guide/bgnet/output/html/singlepage/bgnet.html#syscalls
    // Namensauflösung z.B. mit getaddrinfo
    // Socket erzeugen, wir brauchen einen TCP socket!
    // Socket verbinden, connect ist das Stichwort!
    int status;
    struct addrinfo hints;
    struct addrinfo *res;  // will point to the result

    memset(&hints, 0, sizeof hints); 
    hints.ai_family = AF_INET;       // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP 

    if ((status = getaddrinfo(host, "80", &hints, &res)) != 0) {
        fprintf(stderr, "Namensauflösung fehlgeschlagen!");
        return -1;
    }
       
    // Erstelle Socket
    int sock;
    if((sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) < 0) {
        fprintf(stderr, "Konnte den Socket nicht erstellen!");
        return sock;
    }
    
    //Verbinde Socket
    if(connect(sock, res->ai_addr, res->ai_addrlen) < 0){
        fprintf(stderr, "Konnte den Socket nicht verbinden!");
        return -1;
    }
    
    // Success
    return sock;
    
    // Fehler behandeln und entsprechende Fehlermeldungen auf stderr ausgeben
}

/**
 * Schreibt Daten in eine Datei
 *
 * @param filefd Filepointer auf mit fopen geöffnete Datei
 * @param bytes Pointer auf bytes die geschrieben werden sollen
 * @param Anzahl an bytes die zu schreiben sind
 * @return Negativ bei Fehler, sonst positiv
 */
int write_bytes_to_file(FILE* filefd, char* bytes, size_t num_bytes) {
    
    int bytes_written = 0;
    size_t total_written = 0;
    
    while(total_written != num_bytes) {
        bytes_written = fwrite(bytes+total_written, 1, num_bytes-total_written, filefd);
        if (bytes_written == 0) {
            return -1;
        }
        total_written += bytes_written;
    }
    fflush(filefd);
    
    return 1;
}


/**
 * Führt einen HTTP/1.0 GET request aus und speichert den Body in die angegebene Datei
 *
 * @param host Host bei dem Heruntergeladen werden soll
 * @param request Der GET Request
 * @param file_name Dateiname wo der HTTP Body hingeschrieben werden soll
 * @return Negativ bei Fehler, sonst positiv
 */
int download_to_file(char* host, char* request, char* file_name) {
    
    // Der Socket
    int socketfd;
    // Der Filepointer zur Datei wo wir schreiben wollen
    FILE* filefd;
    // Das muster für unseren request, die %s müssen wir noch durch die Parameter ersetzten
    char* http_request_pattern = "GET %s HTTP/1.0\r\nHost: %s\r\nConnection: close\r\n\r\n";
    // 2^10 bytes für den request sollten reichen
    char request_buffer[1 << 10];
    // 2^16 byte ist unser receive buffer, wir lesen also immer 65kB in chunks
    char receive_buffer[1 << 16];
    // Wie groß ist etwas?
    ssize_t num_bytes;
    // könnte nützlich sein um nachzuhalten ob wir schon den Header empfangen haben
    bool header_consumed = false;
    
    
    // erzeuge den Request anhand des Patterns
    num_bytes = snprintf(request_buffer, sizeof(request_buffer), http_request_pattern, request, host);
    if (num_bytes >= (ssize_t)sizeof(request_buffer)) {
        fprintf(stderr, "Der request ist zu lang\n");
        return -1;
    }
    
    // Verbinde den socket mit connect_socket und prüfe ob dies Erfolgreich war
    // Im Fehlerfall, gib einen Fehler auf stderr aus und verlass die Funktion sauber und ordentlich
    if((socketfd = connect_socket(host)) < 0) {
        fprintf(stderr, "Fehler beim Verbindungsaufbau\n");
        return -1;
    }
    
    
    /****** TODO: Sende nun (z.B. mit send) den request an den Server ****/
    // Im Fehlerfall, gib einen Fehler auf stderr aus und verlass die Funktion sauber und ordentlich
    if(send(socketfd, request_buffer, num_bytes, 0) < 0){
        fprintf(stderr, "Fehler beim Senden der Anfrage!\n");
        close(socketfd);
        return -1;
    }

    
    // Dies öffnet die Datei zum schreiben!
    filefd = fopen(file_name, "w+");
    if(filefd == NULL) {
        perror("Fehler beim Datei öffnen");
        close(socketfd);
        return -1;
    }
    
    
    /**
     * Eine HTTP response besteht aus einem Header und einem Body
     *
     * Header und Body werden durch die Zeichenfolge \r\n\r\n voneinander getrennt
     * Da wir einen HTTP/1.0 request gemacht haben, bekommen wir einen sogenannte
     * End-Of-Stream response. Wir wissen vorher nicht genau wie groß die Antwort ist.
     * aber der Server wird die Verbindung schließen sobald er alle Daten verschickt hat
     *
     * Wenn der Server die Verbindung schließt, returned recv das es 0 byte empfangen hat
     * So können wir also das End-Of-Stream feststellen.
     *
     *
     * Wir interessieren uns hier nur für den Body, den Header wollen wir ignorieren
     * Wir müssen ihn aber auf jedenfall empfangen, aber wir können ihn getrost ignorieren
     *
     * Das ganze ist etwas unsauber aber um die komplexität der Aufgabe gering zu halten machen wir das so.
     */

    while(1) {
        /**** TODO: lese Daten vom socket in den receive_buffer, z.b. mit recv ***/
        // pass auf das du nicht mehr ließt als du speicher hast!
        ssize_t read_bytes = recv(socketfd, receive_buffer, sizeof(receive_buffer), 0);
        
        /**** TODO: wieviele byte wurden empfangen? Sind wir schon am Ende? Liegt ein Fehler vor? Müssen wir die Endlosschleife verlassen? ***/
        // Im Fehlerfall, gib einen Fehler auf stderr aus und verlass die Funktion sauber und ordentlich!
        if(read_bytes < (ssize_t)0 ){
            fprintf(stderr, "Fehler beim Empfangen von Daten!\n");
            fclose(filefd);
            close(socketfd);
            return -1;
        } else if(read_bytes == (ssize_t)0 ){
            //Ende der Übertragung
            break;
        }
        
        
        /* TODO: haben wir schon den Header empfangen?
         *
         * ja? -> Dann schreib die empfangen bytes mittels write_bytes_to_file in die Datei
         *
         * nein? -> dann prüf ob wir das Ende des Headers in den aktuellen Daten haben
         *          Vergiss nicht alles nach dem Header in die Datei zu schreiben
         *
         * Achtung: Prüfe ob das Schreiben in die Datei erfolgreich war, wenn nicht räum hinter dir auf und verlass die Funktion ordentlich!
         */
        if(header_consumed) {
            int write =  write_bytes_to_file(filefd, receive_buffer, read_bytes);
            if(write < 0){
                fprintf(stderr, "Fehler beim Schreiben in die Datei!\n");
                fclose(filefd);
                close(socketfd);
                return -1;
            }
        } else {
            // Header einlesen, danach in Datei schreiben
            ssize_t remaining_bytes = read_bytes;
            int i;
            char* pos = receive_buffer;
            // Gehe Zeichen für Zeichen durch, sobald ein \r gefunden wurde: prüfe, ob darauf \n\r\n folgt
            for(i=0; i < ((int)read_bytes); i++){
                remaining_bytes--;
                if(*pos == '\r'){
                    // Wenn man sich in den letzten drei Bytes befindet, kann \n\r\n nicht mehr folgen
                    // also dies abfragen um Seg fault zu vermeiden
                    if(i<((int)read_bytes)-3 && *(pos+1) == '\n' && *(pos+2) == '\r' && *(pos+3) == '\n'){
                        header_consumed = true;
                        remaining_bytes -= 3;
                        if(remaining_bytes > 0){
                            pos += 4;
                            int write =  write_bytes_to_file(filefd, pos, remaining_bytes);
                            if(write < 0){
                                fprintf(stderr, "Fehler beim Schreiben in die Datei!");
                                fclose(filefd);
                                close(socketfd);
                                return -1;
                            }
                        }
                    }
                }
                pos++;
            }
            
        }
        
    }
    
    // schließ die Datei
    fclose(filefd);
    
    // schließ den socket!
    close(socketfd);
    
    
    return 1;
}


/**
 * Sucht die "original" URL in der 1. Zeile der Datei
 *
 * @param file_name  Datei mit dem giphy json result
 * @param url Pointer auf Pointer auf einen String, Pointer hinter Pointer sollte auf Null zeigen und muss später mit free freigeben werden (speicher wird angelegt!)
 * @return Negativ bei Fehler, sonst positiv
 */
int find_url(char* file_name, char** url) {
    FILE* file;
    
    char* buffer = NULL;
    ssize_t line_len;
    size_t linecap = 0;
    file = fopen(file_name, "r");
    
    
    // Lade 1. Zeile aus der Datei
    line_len = getline(&buffer, &linecap, file);
    
    if(line_len < 0) {
        fclose(file);
        free(buffer);
        perror("Error beim lesen einer Zeile");
        return -1;
    }
    
    // parse das Json und such die URL
    cJSON* root = cJSON_Parse(buffer);
    cJSON* data = cJSON_GetObjectItem(root, "data");
    cJSON* first = cJSON_GetArrayItem(data, 0);
    cJSON* images = cJSON_GetObjectItem(first, "images");
    cJSON* original = cJSON_GetObjectItem(images, "original");
    cJSON* jurl = cJSON_GetObjectItem(original, "url");
    
    // hat irgendwas davon nicht geklappt?
    if(jurl == NULL) {
        cJSON_Delete(root);
        free(buffer);
        fclose(file);
        return -1;
    }
    
    // wie lang ist die URL?
    int len = strlen(jurl->valuestring)+1;
    // leg speicher an
    *url = malloc(len);
    // haben wir genug speicher?
    if (*url == NULL) {
        cJSON_Delete(root);
        free(buffer);
        fclose(file);
        return -1;
    }
    // erstmal alles 0en
    memset(*url, 0, len);
    // Jetzt die URL kopieren
    memcpy(*url, jurl->valuestring, len-1);
    
    // das json objekt wieder freigeben
    cJSON_Delete(root);
    //die von getline allozierten Daten wieder freigeben
    free(buffer);
    // Datei zu machen
    fclose(file);
    
    // das hat wohl alles geklappt
    return 1;
}

/**
 * Fragt den Nutzer nach einer Eingabe und ersetzt Leerzeichen durch '+'
 *
 * @param input Pointer auf einen Pointer auf einen String, wenn Pointer hinter Pointer auf NULL zeigt wird Speicher angelegt, Achtung muss wieder freigegeben werden
 * @return Negativ bei Fehler, sonst positiv
 */
int get_search_term_from_input(char** input) {
    char* user_query = "Wonach soll ich suchen: ";
    
    size_t linecap = 0;
    ssize_t linelen;
    
    fwrite(user_query, 1, strlen(user_query), stdout);
    fflush(stdout);
    
    // ließ eine Zeile von stdin
    if((linelen = getline(input, &linecap, stdin)) < 0) {
        return -1;
    }
    
    // loesche das \n vom ENTER drücken des Users und terminiere den string
    // linelen ist laenge ohne null termination also genau das \n, achtung 0 indiziert !
    (*input)[linelen-1] = '\0';
    
    // pos zeigt auf den Anfang des Strings
    char* pos = *input;
    
    /**** TODO: ersetzte alle Leerzeichen durch ein '+' ****/
    
    while(*pos != '\0') {
        if(*pos++ == ' ') {
            *(pos-1) = '+';
        }
    }
    
    return 1;
}



int main() {
    char* JSON_URL = "/tmp/giphy_json";
    // Darauf muessen wir eine Anfrage stellen, %s wird noch durch die Suchanfrage ersetzt
    char* query_template = "/v1/gifs/search?api_key=dc6zaTOxFJmzC&limit=1&q=%s";
    char* query;
    char* url = NULL;
    
    if(get_search_term_from_input(&url) < 0){
        printf("Error bei Benutzereingabe\n");
        return -1;
    }
    
    query = malloc(strlen(query_template) + strlen(url));
    
    if(query == NULL) {
        fprintf(stderr, "Nicht genug Speicherplatz\n");
        free(url);
        return -1;
    }
    // Da ist genug Platz, Schreibe des Ergebnis des Formatstrings in query (also %s ersetzten durch die Usereingabe)
    sprintf(query, query_template, url);
    // jetzt brauchen wir URL nicht mehr
    free(url);
    // aber wir verwenden es gleich nochmal... also wieder auf NULL
    url = NULL;
    
    // download the api request t
    if(download_to_file("api.giphy.com", query, JSON_URL) < 0) {
        free(query);
        fprintf(stderr, "Error downloading response\n");
        return -1;
    }
    
    if(find_url(JSON_URL, &url) < 0) {
        free(url);
        fprintf(stderr, "No results\n");
        return -1;
    }
    
    // finde teilstrings in der URL
    // dateiname = letzer / + 1
    char* filename = strrchr(url, '/')+1;
    // host anfang = 1. / + 2 (e.g. HTTP://)
    char* host_start = strchr(url, '/')+2;
    // request ist alles nach dem host, also 1. / nach host_start
    char* request = strchr(host_start, '/');
    
    // wir brauchen den host null terminiert, sonst lesen string Funktionen bis zum Ende
    char* host = malloc(request - host_start + 1);
    memset(host, 0, request - host_start + 1);
    memcpy(host, host_start, request - host_start);
    
    
    if(download_to_file(host, request, filename) < 0) {
        fprintf(stderr, "Error downloading gif\n");
        free(host);
        free(url);
        return -1;
    };
    
    free(host);
    free(url);
    return 0;
}
