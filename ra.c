#define _GNU_SOURCE 
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdbool.h> // to use bool type 

#define MAX_LINE_LENGTH 256

/* return the number of lines of a file */
int GetLinesNumber(FILE *file) {
    int ch;
    int lines = 0;
    while ((ch = fgetc(file)) != EOF){
        if (ch == '\n'){
            lines++;
        }
    }
    rewind(file);
    return lines;
}

/* return a random number between 1 and max */
int GetRandNumber(int max){
    int rd_num = rand() % max + 1;
    return rd_num;
}

/* returns the string that exist in the given line */
void LineString(FILE *file, int line_num){
    char line[MAX_LINE_LENGTH];
    int count = 1;
    int lineFound = 0;

    while (fgets(line, sizeof line, file) != NULL) {
        if (count == line_num) {
            printf("%s", line);
            lineFound = 1;
            break;
        }
        count++;
    }
    if(!lineFound) printf("Line %d not found\n", line_num);

}

int IsThere(FILE *file, char name[]) {
    char line[MAX_LINE_LENGTH];
    
    rewind(file);

    while (fgets(line, sizeof(line), file) != NULL) {
        line[strcspn(line, "\n")] = '\0';
        
        if (strcasestr(line, name) != NULL) { 
            return 1; 
        }
    }
    return 0;
}


void query(FILE *file, char name[]) {
    char line[MAX_LINE_LENGTH];
    int count =0;
    
    rewind(file);

    while (fgets(line, sizeof(line), file) != NULL) {
        line[strcspn(line, "\n")] = '\0';
        
        if (strcasestr(line, name) != NULL) { 
            printf("%s\n", line); 
            count++;
        }
    }
    if (count == 0)
        printf("%s is Not Found\n", name);
}

int main(int argc, char *argv[]) {

    FILE *file = fopen("AnimeList.txt", "a+");

    if (file == NULL) {
        perror("fopen");
        return EXIT_FAILURE;
    }

    srand(time(NULL));


    if (argc == 1) {
        srand(time(NULL));
        int lines = GetLinesNumber(file);
        int rd_line = GetRandNumber(lines);
        LineString(file, rd_line);
    }
    else if (argc >= 3 && strcmp(argv[1], "add") == 0) {
        int is_there = IsThere(file, argv[2]);
        
        if (is_there)
            printf("%s is already added\n", argv[2]);
        else {
            fprintf(file, "%s\n", argv[2]);
            printf("%s is added succesfully to the anime list\n", argv[2]);
        }
    }
    else if (argc >= 3 && strcmp(argv[1], "remove") == 0) {
        /* remove anime */
    }
    else if (argc >= 3 && strcmp(argv[1], "query") == 0) {
        /* query anime */
       query(file, argv[2]);
    }
    else {
        printf("Usage:\n");
        printf("  %s\n", argv[0]);
        printf("  %s add \"anime name\"\n", argv[0]);
        printf("  %s remove \"anime name\"\n", argv[0]);
        printf("  %s query \"anime name\"\n", argv[0]);
    }

    fclose(file);
    return EXIT_SUCCESS;
}
