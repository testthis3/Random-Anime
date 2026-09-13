#define _GNU_SOURCE 

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define MAX_LINE_LENGTH 256

/* return $HOME/path/to/ur/AnimeList.txt */
char *getdirPath(void)
{
    const char *home = getenv("HOME");
    if (home == NULL) {
        fprintf(stderr, "HOME is not set\n");
        return NULL;
    }

    const char *suffix = "/Projects/C/RandomAnime/";
    size_t size = strlen(home) + strlen(suffix) + 1;
    char *path = malloc(size);
    
    if (path == NULL) {
        perror("malloc");
        return NULL;
    }
    
    snprintf(path, size, "%s%s", home, suffix);
    return path;
}

char *getpath(char * filename)
{
    char *dir_path = getdirPath(); // directory path

    if (dir_path == NULL){
        free(dir_path);
        return NULL;
    }
    
    size_t size = strlen(dir_path) + strlen(filename) + 1;
    
    char *path = malloc(size);
    
    if(path == NULL){
        perror("malloc");
        return NULL;
    }

    snprintf(path, size, "%s%s", dir_path, filename);
    
    free(dir_path);
    
    return path;
}

/* open file */
FILE *open_file(char *filename)
{
    char *path = getpath(filename);

    if (path == NULL){
        return NULL;
    }

    FILE *file = fopen(path, "a+");

    if (!file) {
        fprintf(stderr, "Could not open %s\n", path);
    }
    
    free(path);
    
    return file;
}

/* return the number of lines of a file */
int GetLinesNumber(FILE *file) 
{
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
int GetRandNumber(int max)
{
    int rd_num = rand() % max + 1;
    return rd_num;
}

/* returns the string that exist in the given line */
void LineString(FILE *file, int line_num)
{
    char line[MAX_LINE_LENGTH];
    int current_line = 1;
    int lineFound = 0;

    while (fgets(line, sizeof line, file) != NULL) {
        if (current_line == line_num) {
            printf("%s", line);
            lineFound = 1;
            break;
        }
        current_line++;
    }
    if(!lineFound) printf("Line %d not found\n", line_num);
}

void add(FILE *file, char name[]) {
    char line[MAX_LINE_LENGTH];
    int count = 0;
   
    rewind(file); // the stream is positionned at the beginning of the file

    while (fgets(line, sizeof(line), file) != NULL) {
        line[strcspn(line, "\n")] = '\0';
        
        if (strcmp(line, name) == 0) { 
            count++; 
        }
    }
    if (count != 0)
        printf("%s is already added\n", name);
    else {
        fprintf(file, "%s\n", name);
        printf("%s is added succesfully to the anime list\n", name);
    }

}


void query(FILE *file, char name[]) 
{
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

void delete(FILE *file, char name[])
{
    char line[MAX_LINE_LENGTH];
    int count = 0;
    
    char *temp_path = getpath("temp.txt");
    char *path = getpath("AnimeList.txt");

    if (temp_path == NULL || path == NULL){
        free(temp_path);
        free(path);
        return;
    }
    
    FILE *temp_file = fopen(temp_path, "w");
    
    if (temp_file == NULL){
        perror("fopen");
        return;
    }

    rewind(file);

    while (fgets(line, sizeof(line), file) != NULL)
    {
        line[strcspn(line, "\n")] = '\0';
        
        if (strcmp(line, name) != 0){
            fprintf(temp_file, "%s\n", line);
        }
        else 
            count++;
    }
   
    // if anime does not exist
    if (!count){
        printf("%s is not present in the list\n", name);

        fclose(temp_file);
        remove(temp_path);
        
        free(temp_path);
        free(path);
        return;
    }
        
    if (remove(path) != 0){
        printf("Error: Unable to delete the orginal file.\n");
    }
    
    if (rename(temp_path, path) != 0){
        printf("Error: Renaming the temporary file failed.\n");
    }
    
    printf("%s successfully removed.\n", name);
    
    fclose(temp_file);
    
    free(temp_path);
    free(path);
}

int main(int argc, char *argv[]) {

    FILE *file = open_file("AnimeList.txt");

    if (!file){     
        return EXIT_FAILURE;
    }
    
    if (argc == 1) {
        /* random anime */
        
        srand(time(NULL));
        
        int lines = GetLinesNumber(file);
        
        if (lines == 0){
            printf("The anime list is empty.\n");
        }
        else{
            int rd_line = GetRandNumber(lines);
            LineString(file, rd_line);
        }
        
    }
    else if (argc == 3 && strcmp(argv[1], "add") == 0) {
        /* add anime */
        add(file, argv[2]);
    }
    else if (argc == 3 && strcmp(argv[1], "remove") == 0) {
        /* remove anime */
        delete(file, argv[2]);
    }
    else if (argc == 3 && strcmp(argv[1], "query") == 0) {
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
