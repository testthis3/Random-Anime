#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PLOT_WIDTH 70
#define MAX_LINE_LENGTH 256

/* Print a specific line from the file. */
char *print_line(FILE *file, int line_number)
{
    char line[MAX_LINE_LENGTH];
    int current_line = 1;

    rewind(file);

    while (fgets(line, sizeof(line), file) != NULL) {
        
        if (current_line == line_number) {
        
            line[strcspn(line, "\n")] = '\0';
            
            size_t size = strlen(line) + 1;

            char *result= malloc(size);
            
            if (result == NULL){
                perror("malloc");
                free(result);
                return NULL;
            }

            strcpy(result, line);

            return result;
        }
        
        current_line++;
    }
    
    fprintf(stderr, "Error: Line %d not found\n", line_number);
    
    return NULL;
}


void print_usage(const char *program)
{
    printf("Usage:\n");
    printf("  %s\n", program);
    printf("  %s add \"anime name\"\n", program);
    printf("  %s remove \"anime name\"\n", program);
    printf("  %s query \"anime name\"\n", program);
    printf("  %s info \"anime name\"\n", program);
}

