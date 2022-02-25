#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
#include "parson.h"
#include "constants.h"

#define HOST "34.118.48.238"
#define PORT 8080

// removes new line char from a string
void remove_nl(char *string) {
    int len = strlen(string) - 1;

    if (string[len] == '\n') {
        string[len] = '\0';
    }
}

// verify if it's a valid string
void get_valid_string(char *prompt, char *entry, char *amsg) {
    memset(entry, 0, MAXINPUTLEN);
    printf("%s", prompt);
    fgets(entry, MAXINPUTLEN, stdin);
    remove_nl(entry);

    while (strlen(entry) == 0) {
        memset(entry, 0, MAXINPUTLEN);
        printf("%s\n", amsg);
        printf("%s", prompt);
        fgets(entry, MAXINPUTLEN, stdin);
        remove_nl(entry);
    }
}

// gets the error from server response (if it exists)
char *get_error(char *response) {
    char *error = strstr(response, ERRORLABEL);

    if (error) {
        error = strtok(error, DELIM);
        error = strtok(NULL, DELIM);
        error = strtok(NULL, DELIM);
    } else {
        error = strstr(response, TOOMANYREQLABEL);
    }

    return error;
}

int main() {
    int socket;
    char *action = NULL;
    char *err = NULL;
    char *username = NULL;
    char *password = NULL;
    char *regmsg, *logmsg, *add_book;
    char *to_send, *resp;
    char *ses_cookie = calloc(MAXCOOKIELEN, sizeof(char));
    char *jwt = calloc(MAXTOKLEN, sizeof(char));
    JSON_Value *rv;
    JSON_Object *ro;

    action = calloc(MAXINPUTLEN, sizeof(char));

    if (action == NULL) {
        printf("ERROR AT ACTION CALLOC\n");
        return 0;
    }

    while (TRUE) {
        memset(action, 0, MAXINPUTLEN);

        // get action
        fgets(action, MAXINPUTLEN, stdin);
        remove_nl(action);

        if (!strcmp(action, REGISTERLABEL)) {
            /* ---------- REGISTER ACTION ---------- */
            username = calloc(MAXINPUTLEN, sizeof(char));
            password = calloc(MAXINPUTLEN, sizeof(char));

            if (username == NULL || password == NULL) {
                printf("ERROR AT USERNAME/PASSWORD CALLOC\n");
                break;
            }

            // get username
            printf(USERPROMPT);
            fgets(username, MAXINPUTLEN, stdin);
            remove_nl(username);

            // get password
            printf(PASSPROMPT);
            fgets(password, MAXINPUTLEN, stdin);
            remove_nl(password);

            // construct JSON payload
            rv = json_value_init_object();
            ro = json_value_get_object(rv);
            json_object_set_string(ro, JSONUSER, username);
            json_object_set_string(ro, JSONPASS, password);
            regmsg = json_serialize_to_string_pretty(rv);

            // open connection
            socket = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

            if (socket < 0) {
                printf("ERROR AT OPEN_CONNECTION\n");
                break;
            }

            // send message and free memory
            to_send = compute_post_request(HOST, REGURL, PLTYPE, &regmsg, 1, NULL, 0, NULL);
            send_to_server(socket, to_send);
            free(username);
            free(password);
            json_free_serialized_string(regmsg);
            json_value_free(rv);
            free(to_send);

            // receive response
            resp = receive_from_server(socket);
            err = get_error(resp);

            if (err) {
                printf("%s%s\n", ERRMSG, err);
            } else {
                printf("%s\n", SUCREGMESG);
            }

            free(resp);
            close(socket);
        } else if (!strcmp(action, LOGINLABEL)) {
            /* ---------- LOGIN ACTION ---------- */
            username = calloc(MAXINPUTLEN, sizeof(char));
            password = calloc(MAXINPUTLEN, sizeof(char));

            if (username == NULL || password == NULL) {
                printf("ERROR AT USERNAME/PASSWORD CALLOC\n");
                break;
            }

            // get username
            printf(USERPROMPT);
            fgets(username, MAXINPUTLEN, stdin);
            remove_nl(username);

            // get password
            printf(PASSPROMPT);
            fgets(password, MAXINPUTLEN, stdin);
            remove_nl(password);

            // construct JSON payload
            rv = json_value_init_object();
            ro = json_value_get_object(rv);
            json_object_set_string(ro, JSONUSER, username);
            json_object_set_string(ro, JSONPASS, password);
            logmsg = json_serialize_to_string_pretty(rv);

            // open connection
            socket = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

            if (socket < 0) {
                printf("ERROR AT OPEN_CONNECTION\n");
                break;
            }

            // send message and free memory
            to_send = compute_post_request(HOST, LOGURL, PLTYPE, &logmsg, 1, NULL, 0, NULL);
            send_to_server(socket, to_send);
            free(username);
            free(password);
            json_free_serialized_string(logmsg);
            json_value_free(rv);
            free(to_send);

            // receive response
            resp = receive_from_server(socket);
            err = get_error(resp);

            if (err) {
                printf("%s%s\n", ERRMSG, err);
            } else {
                // we only need the session cookie
                memset(ses_cookie, 0, MAXCOOKIELEN);

                char *token = strstr(resp, COOKIESTART);
                token = strtok(token, COOKIEDELIM);
                memcpy(ses_cookie, token, strlen(token));

                printf("%s\n", SUCLOGMESG);
            }

            free(resp);
            close(socket);
        } else if (!strcmp(action, ENTRLABEL)) {
            /* ---------- ENTER_LIBRARY ACTION ---------- */
            // open socket
            socket = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

            if (socket < 0) {
                printf("ERROR AT OPEN_CONNECTION\n");
                break;
            }

            // send get request for jwt
            to_send = compute_get_request(HOST, ACCURL, NULL, &ses_cookie, 1, NULL);
            send_to_server(socket, to_send);
            free(to_send);

            // get response
            resp = receive_from_server(socket);
            err = get_error(resp);

            if (err) {
                printf("%s%s\n", ERRMSG, err);
            } else {
                // we need the jwt token
                memset(jwt, 0, MAXTOKLEN);

                char *token = strstr(resp, TOKENSTART);
                token = strtok(token, DELIM);
                token = strtok(NULL, DELIM);
                token = strtok(NULL, DELIM);
                memcpy(jwt, token, strlen(token));

                printf("%s\n", SUCENTLIB);
            }

            free(resp);
            close(socket);
        } else if (!strcmp(action, GETBOOKSLABEL)) {
            /* ---------- GET_BOOKS ACTION ---------- */
            // open socket
            socket = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

            if (socket < 0) {
                printf("ERROR AT OPEN_CONNECTION\n");
                break;
            }
            
            // send get request for all books
            to_send = compute_get_request(HOST, BOKURL, 0, &ses_cookie, 1, jwt);
            send_to_server(socket, to_send);
            free(to_send);

            // receive message
            resp = receive_from_server(socket);
            err = get_error(resp);

            if (err) {
                printf("%s%s %s\n", ERRMSG, err, VRFLIBENT);
            } else {
                // parse books
                printf("\nBooks available:\n");
                char *token = strstr(resp, BOOKSTART);
                rv = json_parse_string(token);
                JSON_Array *array = json_value_get_array(rv);

                if (json_array_get_count(array) == 0) {
                    printf("No books!\n\n");
                }
                
                for (int i = 0; i < json_array_get_count(array); i++) {
                    JSON_Object *ent = json_array_get_object(array, i);
                    printf("ID: %.0lf\nTitle: %s\n\n",
                            json_object_get_number(ent, "id"),
                            json_object_get_string(ent, "title"));
                }

                json_value_free(rv);
            }

            free(resp);
            close(socket);
        } else if (!strcmp(action, GETBOOKLABEL)) {
            /* ---------- GET_BOOK ACTION ---------- */
            // we first need an ID
            char *id = calloc(MAXINPUTLEN, sizeof(char));
            printf("%s", IDBKPROMPT);
            fgets(id, MAXINPUTLEN, stdin);
            remove_nl(id);

            // construct the URL
            char *gb_URL = calloc(MAXINPUTLEN, sizeof(char));
            strncpy(gb_URL, BOKURL, MAXINPUTLEN);
            strcat(gb_URL, "/");
            strcat(gb_URL, id);

            // open socket
            socket = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

            if (socket < 0) {
                printf("ERROR AT OPEN_CONNECTION\n");
                break;
            }

            // send message and free memory
            to_send = compute_get_request(HOST, gb_URL, 0, &ses_cookie, 1, jwt);
            send_to_server(socket, to_send);
            free(to_send);
            free(id);
            free(gb_URL);

            // receive response
            resp = receive_from_server(socket);
            err = get_error(resp);

            if (err) {
                printf("%s%s\n", ERRMSG, err);
            } else {
                // parse the returned book
                char *token = strstr(resp, BOOKSTART);
                rv = json_parse_string(token);
                JSON_Array *aux = json_value_get_array(rv);
                JSON_Object *book = json_array_get_object(aux, 0);

                printf("\nTitle: %s\nAuthor: %s\nPublisher: %s\nGenre: %s\nPage count: %0.lf\n\n",
                        json_object_get_string(book, JSONTITL),
                        json_object_get_string(book, JSONAUTH),
                        json_object_get_string(book, JSONPUBL),
                        json_object_get_string(book, JSONGENR),
                        json_object_get_number(book, JSONNUMB));
                
                json_value_free(rv);
            }

            free(resp);
            close(socket);
        } else if (!strcmp(action, ADDBOOKLABEL)) {
            /* ---------- ADD_BOOK ACTION ---------- */
            char *entry = calloc(MAXINPUTLEN, sizeof(char));
            char *num_entry = calloc(MAXINPUTLEN, sizeof(char));
            rv = json_value_init_object();
            ro = json_value_get_object(rv);

            // get title
            get_valid_string(TITLPROMPT, entry, ATITL);
            json_object_set_string(ro, JSONTITL, entry);

            // get author
            get_valid_string(AUTHPROMPT, entry, AAUTH);
            json_object_set_string(ro, JSONAUTH, entry);

            // get genre
            get_valid_string(GENRPROMPT, entry, AGENR);
            json_object_set_string(ro, JSONGENR, entry);

            // get publisher
            get_valid_string(PUBLPROMPT, entry, APUBL);

            // get page_count
            memset(num_entry, 0, MAXINPUTLEN);
            printf("%s", PGCTPROMPT);
            fgets(num_entry, MAXINPUTLEN, stdin);
            remove_nl(num_entry);

            while (atoi(num_entry) == 0) {
                memset(num_entry, 0, MAXINPUTLEN);
                printf("%s\n", ANUMB);
                printf("%s", PGCTPROMPT);
                fgets(num_entry, MAXINPUTLEN, stdin);
                remove_nl(num_entry);
            }

            json_object_set_number(ro, JSONNUMB, atoi(num_entry));

            json_object_set_string(ro, JSONPUBL, entry);
            add_book = json_serialize_to_string_pretty(rv);

            // open socket
            socket = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

            if (socket < 0) {
                printf("ERROR AT OPEN_CONNECTION\n");
                break;
            }

            // send message and free memory
            to_send = compute_post_request(HOST, BOKURL, PLTYPE, &add_book, 1,
                                           &ses_cookie, 1, jwt);
            send_to_server(socket, to_send);
            free(to_send);
            free(entry);
            free(num_entry);
            json_free_serialized_string(add_book);
            json_value_free(rv);

            // receive response
            resp = receive_from_server(socket);
            err = get_error(resp);

            if (err) {
                printf("%s%s %s\n", ERRMSG, err, VRFLIBENT);
            } else {
                printf("%s\n", SUCADDBOOK);
            }

            free(resp);
            close(socket);
        } else if (!strcmp(action, DELLABEL)) {
            /* ---------- DELETE_BOOK ACTION ---------- */
            // we first need an ID
            char *id = calloc(MAXINPUTLEN, sizeof(char));
            printf("%s", IDBKPROMPT);
            fgets(id, MAXINPUTLEN, stdin);
            remove_nl(id);

            // construct the URL
            char *db_URL = calloc(MAXINPUTLEN, sizeof(char));
            strncpy(db_URL, BOKURL, MAXINPUTLEN);
            strcat(db_URL, "/");
            strcat(db_URL, id);

            // open socket
            socket = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

            if (socket < 0) {
                printf("ERROR AT OPEN_CONNECTION\n");
                break;
            }

            // send message and free memory
            to_send = compute_del_request(HOST, db_URL, NULL, &ses_cookie, 1, jwt);
            send_to_server(socket, to_send);
            free(to_send);
            free(id);
            free(db_URL);

            // receive response
            resp = receive_from_server(socket);
            err = get_error(resp);

            if (err) {
                printf("%s%s\n", ERRMSG, err);
            } else {
                printf("%s\n", SUCDELBOOK);
            }

            free(resp);
            close(socket);
        } else if (!strcmp(action, LOGOUTLABEL)) {
            /* ---------- LOGOUT ACTION ---------- */
            // open socket
            socket = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

            if (socket < 0) {
                printf("ERROR AT OPEN_CONNECTION\n");
                break;
            }

            // send message and free memory
            to_send = compute_get_request(HOST, OUTURL, 0, &ses_cookie, 1, NULL);
            send_to_server(socket, to_send);
            free(to_send);

            // receive message
            resp = receive_from_server(socket);
            err = get_error(resp);

            if (err) {
                printf("%s%s\n", ERRMSG, err);
            } else {
                printf("%s\n", SUCOUTMESG);

                // set cookie and jwt to 0 for good measure
                memset(ses_cookie, 0, MAXCOOKIELEN);
                memset(jwt, 0, MAXTOKLEN);
            }

            free(resp);
            close(socket);
        } else if (!strcmp(action, EXITLABEL)) {
            /* ---------- EXIT ACTION ---------- */
            break;
        } else {
            /* ---------- UNKNOWN ACTION ---------- */
            printf(UNKNOWNCOMM);
        }
    }

    if (ses_cookie) {
        free(ses_cookie);
    }

    if (jwt) {
        free(jwt);
    }

    free(action);
    return 0;
}
