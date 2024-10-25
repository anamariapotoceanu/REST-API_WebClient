#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <stdio.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
#define BUFLEN 4096
#define LINELEN 1000

char *compute_delete_request(char *token, char *host, char *url, char **cookies, int cookies_count) {
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));
    int i;
    
    sprintf(line, "DELETE %s HTTP/1.1", url);
    compute_message(message, line);
    
  
    sprintf(line, "Host: %s", host);
    compute_message(message, line);
    
 
    if (token != NULL) {
        sprintf(line, "Authorization: Bearer %s", token);
        compute_message(message, line);
    }
    

    if (cookies != NULL && cookies_count > 0) {
        int line_cookie;
        line_cookie = sprintf(line, "Cookie: %s", cookies[0]);
        for (i = 1; i < cookies_count; i++) {
            line_cookie = line_cookie + sprintf(line + line_cookie, "; %s", cookies[i]);
        }
        compute_message(message, line);
    }

    compute_message(message, "");

    free(line);
    return message;
}

char *compute_get_request(char *host, char *url, char *query_params, char *token,
                            char **cookies, int cookies_count)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));
    int i;

    // Step 1: write the method name, URL, request params (if any) and protocol type
    if (query_params != NULL) {
        sprintf(line, "GET %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "GET %s HTTP/1.1", url);
    }

    compute_message(message, line);

    // Step 2: add the host
    sprintf(line, "Host: %s", host);
    compute_message(message, line);
    if (token != NULL) {
        sprintf(line, "Authorization: Bearer %s", token);
        compute_message(message, line);
    }
    // Step 3 (optional): add headers and/or cookies, according to the protocol format
    if (cookies != NULL && cookies_count > 0) {
        int line_cookie;
        line_cookie = sprintf(line, "Cookie: %s", cookies[0]);

        for (i = 1; i < cookies_count; i++) {
            line_cookie = line_cookie + sprintf(line + line_cookie, "; %s", cookies[i]);
        }
         compute_message(message, line);
    }
    // Step 4: add final new line
    compute_message(message, "");
    return message;
}

char *compute_post_request(char *host, char *url, char* content_type, char **body_data,
                            int body_data_fields_count, char *token, char **cookies, int cookies_count)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));
    char *body_data_buffer = calloc(LINELEN, sizeof(char));
    int i, content_length;
    int body_data_length;

    // Step 1: write the method name, URL and protocol type
    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);
    
    // Step 2: add the host
    sprintf(line, "Host: %s", host);
    compute_message(message, line);
    if (token != NULL) {
        sprintf(line, "Authorization: Bearer %s", token);
        compute_message(message, line);
    }
    /* Step 3: add necessary headers (Content-Type and Content-Length are mandatory)
            in order to write Content-Length you must first compute the message size
    */

    sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);
    if (body_data != NULL && body_data_fields_count > 0) {
        content_length = 0;
        for (i = 0; i < body_data_fields_count; i++) {
            body_data_length = 0;
            while (body_data[i][body_data_length] != '\0') {
                body_data_length++;
            }
            content_length = content_length + body_data_length;
        }
        sprintf(line, "Content-Length: %d", content_length);
        compute_message(message, line);
    }
    // Step 4 (optional): add cookies
    if (cookies != NULL && cookies_count > 0) {
        int line_cookie;
        line_cookie = sprintf(line, "Cookie: %s", cookies[0]);

        for (i = 1; i < cookies_count; i++) {
            line_cookie = line_cookie + sprintf(line + line_cookie, "; %s", cookies[i]);
        }
         compute_message(message, line);
    }
    // Step 5: add new line at end of header
    compute_message(message, "");

    // Step 6: add the actual payload data
    if (body_data != NULL && body_data_fields_count > 0 ) {
        for (i = 0; i < body_data_fields_count; i++) {
            strcat(message, body_data[i]);
        }
    }
    memset(line, 0, LINELEN);
    strcat(message, body_data_buffer);

    free(line);
    return message;
}
