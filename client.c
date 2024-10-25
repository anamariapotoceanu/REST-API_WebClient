
#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "parson.h"
#include "requests.h"

// Functie pentru inregistarea unui utilizator
void function_register(char username[], char password[], int sockfd, char *message, char *response, char* address) 
{
    char *json_serialized_string = NULL;
    char error_msg[100];
    char succes_msg1[100];
    char succes_msg2[100];
    
    // Cream obiectul JSON cu caracteristicile utilizatorului
    JSON_Value *value = json_value_init_object();
    JSON_Object *object = json_value_get_object(value);
    json_object_set_string(object, "username", username);
    json_object_set_string(object, "password", password);

    json_serialized_string = json_serialize_to_string_pretty(value);
            

    message = compute_post_request("34.246.184.49", address,
                "application/json", &json_serialized_string, 1, NULL, NULL, 0);

    send_to_server(sockfd, message);
    strcpy(error_msg, "error");
    response = receive_from_server(sockfd);

    // Verificam daca primim eroare de la server
    if (strstr(response, error_msg) != NULL) {
        printf("EROARE: Username-ul este deja folosit.\n");
    }
    strcpy(succes_msg1, "201 Created");
    strcpy(succes_msg2, "200 OK");

    if (strstr(response, succes_msg1) != NULL || (strstr(response, succes_msg2) != NULL)) {
        printf("SUCCES: 200 - OK - Utilizator înregistrat cu succes!\n");
    }
    json_free_serialized_string(json_serialized_string);
}

// Functie pentru logarea unui utilizator
char* function_login(char username[], char password[], int sockfd, char *message, char *response, char *address) {

    char *json_serialized_string = NULL;
    char *cookie = NULL;
    char error_msg1[100];
    char error_msg2[100];
    JSON_Value *value = json_value_init_object();
    JSON_Object *object = json_value_get_object(value);
    // Cream obiectul JSON cu caracteristicile utilizatorului
    json_object_set_string(object, "username", username);
    json_object_set_string(object, "password", password);

    json_serialized_string = json_serialize_to_string_pretty(value);
            

    message = compute_post_request("34.246.184.49", address, "application/json",
            &json_serialized_string, 1, NULL, NULL, 0);
   

    send_to_server(sockfd, message);
    // Mesajul de eroare pentru credentialele gresite
    strcpy(error_msg1, "Credentials are not good!");
    // Mesajul de eroare pentru logarea unui utilizator care nu exista
    strcpy(error_msg2, "No account with this username!");
   
    response = receive_from_server(sockfd);
         
 
    json_free_serialized_string(json_serialized_string);

    if (strstr(response, error_msg1) != NULL) {
        printf("EROARE: Nu se potrivesc credenţialele!\n");
    } else if (strstr(response, error_msg2) != NULL) {
        printf("EROARE: Nu exista niciun utilizator cu acest nume!\n");
    } else {
        // Extragem cookie din raspunsul primit de la server 
        char *start;
        start = strstr(response, "Set-Cookie: ");
        start += 12;
        cookie = strtok(start, ";");
        printf("SUCCES: 200 - OK - Utilizatorul s-a logat cu succes!\n");
        // Returnam cookie-ul
        return cookie;
    }
    return NULL;
}

// Functie pentru accesul la biblioteca
char* function_enter(int sockfd, char *message, char *response, char *cookie, char *address)
{
    char *key, *start, *jwt_token;
    message = compute_get_request("34.246.184.49", address, NULL,  NULL, &cookie, 1);
    send_to_server(sockfd, message);

    response = receive_from_server(sockfd);
    if (strstr(response, "error")) {
        printf("EROARE: Nu se poate intra in biblioteca.\n");
        return NULL;
    }
    // Extragem token-ul din raspunsul primit de la server
    key = "\"token\":\"";
    start = strstr(response, key);
    start += 9;
    jwt_token = strtok(start, "\"");
    // Returnam token-ul
    return jwt_token;
}

// Functie pentru afisarea informatiilor despre carti
void print_books(JSON_Array *books, size_t count) {
 
    int id;
    char title[1000];
    JSON_Object *book;
    size_t i;
    int first = 1;

    printf("SUCCES:\n[\n");
    for (i = 0; i < count; i++) {
    
        book = json_array_get_object(books, i);

        id = json_object_get_number(book, "id");
        strcpy(title, json_object_get_string(book, "title"));
        if (!first) {
            printf(",\n");
        }
        first = 0;

        printf("  {\n    \"id\": %d,\n    \"title\": \"%s\"\n  }", id, title);
    }
    printf("\n]\n");


}

void function_get_books(int sockfd, char *message, char *response, char *cookie, char *jwt_token, char *address)
{
    message = compute_get_request("34.246.184.49", address, NULL, jwt_token,
                &cookie, 1);

    send_to_server(sockfd, message);

    response = receive_from_server(sockfd);
    /* Extragem cartile pe care vrem sa le afisam din raspunsul primit
    de la server. */
    JSON_Value *root_value = json_parse_string(strchr(response, '['));
    JSON_Array *total_books = json_value_get_array(root_value);
    size_t count;
    count = json_array_get_count(total_books);
    print_books(total_books, count);

}

// Functie pentru afisarea informatiilor despre o carte  
void function_get_book(int sockfd, char *message, char *response, char *cookie, char *jwt_token, char* address)
{
    char *book;
    char error_msg[100];

    message = compute_get_request("34.246.184.49", address, NULL, jwt_token,
                &cookie, 1);


    send_to_server(sockfd, message);

    // Mesajul de eroare daca cartea cu id-ul respectiv nu exista.
    strcpy(error_msg, "No book was found!");

    response = receive_from_server(sockfd);

    if (strstr(response, error_msg) != NULL) {
        printf("EROARE: Carte cu id-ul respectiv nu exista!\n");
    } else {
        // Extragem informatiile despre carte din raspunsul primit de la server
        book = strchr(response, '{');
        JSON_Value* value = json_parse_string(book);
        JSON_Object* object = json_value_get_object(value);
    
        char genre[100], publisher[100], author[100], title[100];
        int page_count, id;

        id = json_object_get_number(object, "id");
        strcpy(title, json_object_get_string(object, "title"));
        strcpy(author, json_object_get_string(object, "author"));
        strcpy(publisher, json_object_get_string(object, "publisher"));
        strcpy(genre, json_object_get_string(object, "genre"));
        page_count = json_object_get_number(object, "page_count");
        printf("SUCCES:\n{\n    \"id\": %d,\n    \"title\": \"%s\",\n    \"author\": \"%s\",\n    \"publisher\": \"%s\",\n    \"genre\": \"%s\",\n    \"page_count\": %d\n}",
                id, title, author, publisher, genre, page_count);
    }
 
}

/* Functie care verfica daca avem doar cifre intr-un sir de caractere.
    Functia aceasta se foloseste atunci cand citim numar de pagini
    pentru o carte.
*/
int is_number(char page_count[], int len)
{
    int i;
    for (i = 0;  i < len - 1; i++) {
        if (page_count[i] < '0' || page_count[i] > '9') {
            return 0;
        }
    }
    return 1;
}

int is_space(char str[], size_t len)
{
    for (size_t i = 0; i < len; i++) {
        if (str[i] == ' ') {
            return 1;
        }
    }
    return 0;
}
// Functie pentru adaugarea unei carti
void function_add_book(int sockfd, char *message, char *response, char *cookie, char *jwt_token, 
                char title[], char author[], char publisher[], char genre[], char page_count[], char *address)
{
    char *serialized_string = NULL;
    JSON_Value *value = json_value_init_object();
    JSON_Object *object = json_value_get_object(value);

    char succes_msg[100];
    char error_msg[100];
    // Setam campurile cartii

    json_object_set_string(object, "title", title);
    json_object_set_string(object, "author", author);
    json_object_set_string(object, "publisher", publisher);
    json_object_set_string(object, "genre", genre);
    json_object_set_string(object, "page_count", page_count);
 
    serialized_string = json_serialize_to_string_pretty(value);
    message = compute_post_request("34.246.184.49", address, "application/json", &serialized_string,
    1, jwt_token, &cookie, 1);

    send_to_server(sockfd, message);

    strcpy(succes_msg, "200 OK");
    strcpy(error_msg, "error");

    response = receive_from_server(sockfd);
    // Verificam daca a fost adaugata cu succes cartea
    if ((strstr(response, error_msg) != NULL) || (strstr(response, succes_msg) == NULL)) {
        printf("EROARE: Cartea nu a putut fi adaugata!\n");
    } else {
        printf("SUCCES: Cartea a fost adaugata cu succes!\n");
    }
     
}

// Functie pentru stergerea unei carti
void function_delete_book(char *message, char *response, int sockfd, char *address, char *cookie, char *jwt_token) 
{
   char error_msg[100];
    message = compute_delete_request(jwt_token, "34.246.184.49", address,
                &cookie, 1);

    send_to_server(sockfd, message);

    strcpy(error_msg, "No book was deleted!");

    response = receive_from_server(sockfd);

    if (strstr(response, error_msg) != NULL) {
        printf("EROARE: Carte cu id-ul respectiv nu exista!\n");
    } else {
        printf("SUCCES: Cartea a fost stearsa cu succes!\n");
    }

}

int main(int argc, char *argv[])
{
    char command[100];
    char *message = NULL;
    char *response = NULL;
    int sockfd;
    char  *cookie = NULL;
    char *jwt_token = NULL;
    char title[100], author[100], genre[100], publisher[100], page_count[100];
   

    while(1) {
        sockfd = open_connection("34.246.184.49", 8080, AF_INET, SOCK_STREAM, 0);
        scanf("%s", command);

           if (strcmp(command, "register") == 0) {
            char username[100];
            char password[100];
            char address[100] = "/api/v1/tema/auth/register";
               char line[256];
            fgets(line, sizeof(line), stdin);
            printf("username=");
            fgets(username, sizeof(username), stdin);
            username[strlen(username) - 1] = '\0';
            size_t len_username = strlen(username);

            printf("password=");
            fgets(password, sizeof(password), stdin);
            password[strlen(password) - 1] = '\0';
            size_t len_password = strlen(password);

            if (is_space(username, len_username) || is_space(password, len_password)) {
                printf("EROARE: Username-ul sau parola nu trebuie sa contina spatii!\n");
            } else {
                function_register(username, password, sockfd, message, response, address);
            }

        } else if (strcmp(command, "login") == 0) {
            char username[100];
            char password[100];
            char address[100] = "/api/v1/tema/auth/login";
            char line[256];
            fgets(line, sizeof(line), stdin);
            printf("username=");
            fgets(username, sizeof(username), stdin);
            username[strlen(username) - 1] = '\0';
            size_t len_username = strlen(username);

            printf("password=");
            fgets(password, sizeof(password), stdin);
            password[strlen(password) - 1] = '\0';
            size_t len_password = strlen(password);

            if (is_space(username, len_username) || is_space(password, len_password)) {
                printf("EROARE: Username-ul sau parola nu trebuie sa contina spatii!\n");
            } else {
                cookie = function_login(username, password, sockfd, message,response, address);
            }
   
        }  else if (strcmp(command, "enter_library") == 0) {
            if (cookie != NULL) {
                char address[100] = "/api/v1/tema/library/access";
                jwt_token = function_enter(sockfd, message, response, cookie, address);
                printf("SUCCES: Utilizatorul are acces la biblioteca!\n");
            } else {
                printf("EROARE: Utilizatorul nu este logat!\n");
            }

        } if (strcmp(command, "get_books") == 0) {

            if (cookie == NULL) {
                printf("EROARE: Utilizatorul nu este logat!\n");
            } else if (jwt_token == NULL) {
                printf("EROARE: Utilizatorul nu are acces la biblioteca!\n");
            } else {
                char address[1000] = "/api/v1/tema/library/books";
                function_get_books(sockfd, message, response, cookie, jwt_token, address);
            }

        } else if (strcmp(command, "get_book") == 0) {
            char address[256] = "/api/v1/tema/library/books/"; 
            int id;
            char idStr[10];
            printf("id=");
            scanf("%d", &id);
            sprintf(idStr, "%d", id);
            // Adaugam id-ul la adresa URL
            strcat(address, idStr);
              
            if (cookie == NULL) {
                printf("EROARE: Utilizatorul nu este logat!\n");
            } else if (jwt_token == NULL) {
                printf("EROARE: Utilizatorul nu are acces la biblioteca!\n");
            } else {
                function_get_book(sockfd, message, response, cookie, jwt_token, address);
            }

        } else if (strcmp(command, "add_book") == 0) {
            
            char line[256];
            fgets(line, sizeof(line), stdin);
            // Citim campurile caracteristice cartii
            printf("title=");
            fgets(title, sizeof(title), stdin);
            title[strlen(title) - 1] = '\0';

            printf("author=");
            fgets(author, sizeof(author), stdin);
            author[strlen(author) - 1] = '\0';

            printf("genre=");
            fgets(genre, sizeof(genre), stdin);
            genre[strlen(genre) - 1] = '\0';

            printf("publisher=");
            fgets(publisher, sizeof(publisher), stdin);
            publisher[strlen(publisher) - 1] = '\0';

            printf("page_count=");
            fgets(page_count, sizeof(page_count), stdin);
            page_count[strlen(page_count) - 1] = '\0';


            if (strlen(title) == 0) {
                printf("EROARE: Cartea trebuie sa aiba un titlu.\n");
            } else if (strlen(author) == 0) {
                printf("EROARE: Cartea trebuie sa aiba un autor.\n");
            } else if (strlen(genre) == 0) {
                printf("EROARE: Cartea trebuie sa aiba un gen.\n");
            } else if (strlen(publisher) == 0) {
                printf("EROARE: Cartea trebuie sa aiba un editor.\n");
            } else {
                int len = strlen(page_count);
                if (strlen(page_count) == 0) {
                    printf("EROARE: Cartea trebuie sa aiba un numar de pagini.\n");
                } else if (!is_number(page_count, len)){ 
                    printf("EROARE: Numarul de pagini nu este un numar intreg.\n");
               
                } else {
                    char address[1000] = "/api/v1/tema/library/books";
                    if (cookie == NULL) {
                        printf("EROARE: Utilizatorul nu este logat!\n");
                    } else if (jwt_token == NULL) {
                        printf("EROARE: Utilizatorul nu are acces la biblioteca!\n");
                    } else {
                        function_add_book(sockfd, message, response, cookie, jwt_token,
                                    title, author, publisher, genre, page_count, address);
                    }
                }
            }

        } else if (strcmp(command, "delete_book") == 0) {
            char address[256] = "/api/v1/tema/library/books/"; 
            int id;
            char idStr[10];
            printf("id=");
            scanf("%d", &id);
            sprintf(idStr, "%d", id);
            strcat(address, idStr);
            if (cookie == NULL) {
                printf("EROARE: Utilizatorul nu este logat!\n");
            } else if (jwt_token == NULL) {
                printf("EROARE: Utilizatorul nu are acces la biblioteca!\n");
            } else {
                function_delete_book(message,response, sockfd, address, cookie, jwt_token) ;
            }

        } else if(strcmp(command, "logout")== 0) {
            if (cookie == NULL) {
                printf("EROARE: Utilizatorul nu este logat!\n");
            } else {
                message = compute_get_request("34.246.184.49", "/api/v1/tema/auth/logout", NULL, NULL,
                            &cookie, 1);


                send_to_server(sockfd, message);
                response = receive_from_server(sockfd);
                cookie = NULL;
                jwt_token = NULL;

                printf("SUCCES: Utilizatorul a fost delogat cu succes!\n");
  
            }

        } else if (strcmp(command, "exit") == 0) {
            break;
        } 
        close_connection(sockfd);
    }

    return 0;
}
