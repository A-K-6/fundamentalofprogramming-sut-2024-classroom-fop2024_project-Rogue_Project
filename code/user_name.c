#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int get_user_info(const char *username, char *email, char *password);
int split(const char *str, char delimiter, char **tokens, int max_tokens) ;
int is_unique_username(const char *username) ;

char *trim_whitespace(char *str) ;

int main(){
    if (is_unique_username("ak")){
        printf("hi");
    }
    else{
        printf("is not uniqe");
        char *email = malloc(30 * sizeof(char));
        char *password = malloc(30 * sizeof(char));

        get_user_info("ak", email, password);
        printf("\n password is %s", password);
    }



    return 0; 
}


int split(const char *str, char delimiter, char **tokens, int max_tokens) {
    int token_count = 0;
    const char *start = str;
    const char *ptr = str;

    while (*ptr != '\0' && token_count < max_tokens - 1) { // Leave space for the last token
        if (*ptr == delimiter) {
            int length = ptr - start;
            tokens[token_count] = (char*)malloc(length + 1);
            strncpy(tokens[token_count], start, length);
            tokens[token_count][length] = '\0';

            // Move to the next token
            token_count++;
            start = ptr + 1;
        }
        ptr++;
    }

    // Add the last token
    if (token_count < max_tokens) {
        int length = ptr - start;
        tokens[token_count] = (char*)malloc(length + 1);
        strncpy(tokens[token_count], start, length);
        tokens[token_count][length] = '\0';
        token_count++;
    }

    return token_count;
}


int is_unique_username(const char *username) {
    FILE *file = fopen("users.txt", "r");
    // if (!file) return 1; // If file doesn't exist, username is unique
    char line[256]; // Buffer to hold each line
    char input_trimmed[256];
    strncpy(input_trimmed, username, sizeof(input_trimmed));
    trim_whitespace(input_trimmed);

    while (fgets(line, sizeof(line), file)) {
        // Remove trailing newline character
        line[strcspn(line, "\n")] = '\0';
        // Prepare to store tokens
        char *tokens[3]; // username, email, password
        int num_tokens = split(line, '|', tokens, 3);

        if (num_tokens >= 1) {
            // Trim whitespace from the username
            char *stored_username = trim_whitespace(tokens[0]);

            // Compare with the provided username
            if (strcmp(input_trimmed, stored_username) == 0) {
                // Free allocated memory
                
                for (int i = 0; i < num_tokens; i++) {
                    free(tokens[i]);
                }
                fclose(file);
                return 0; // Username exists
            }
        }

        // Free allocated memory for tokens
        for (int i = 0; i < num_tokens; i++) {
            free(tokens[i]);
        }
    }

    fclose(file);
    return 1; // Username is unique
}


int get_user_info(const char *username, char *email, char *password) {
    FILE *file = fopen("users.txt", "r");
    if (!file) {
        return 0; // Failure
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        // Remove any trailing newline character
        line[strcspn(line, "\n")] = '\0';

        // Split the line into tokens
        char *tokens[3]; // Assuming username|email|password
        int num_tokens = split(line, '|', tokens, 3);

        if (num_tokens == 3) {
            char *stored_username = tokens[0];
            char *stored_email = tokens[1];
            char *stored_password = tokens[2];

            // Compare usernames
            if (strcmp(username, stored_username) == 0) {
                // Copy email and password to output variables
                strcpy(email, stored_email);
                strcpy(password, stored_password);

                // Free allocated memory
                for (int i = 0; i < num_tokens; i++) {
                    free(tokens[i]);
                }
                fclose(file);
                return 1; // Success
            }
        }

        // Free allocated memory
        for (int i = 0; i < num_tokens; i++) {
            free(tokens[i]);
        }
    }

    fclose(file);
    return 0; // User not found
}


char *trim_whitespace(char *str) {
    char *end;

    while (*str == ' ' || *str == '\t') str++;

    if (*str == '\0') {
        return str;
    }

    end = str + strlen(str) - 1;
    while (end > str && (*end == ' ' || *end == '\t')) end--;

    *(end + 1) = '\0';

    return str;
}
