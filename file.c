#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ANIME_DIR "/Projects/C/RandomAnime/"
#define ANIME_FILE "AnimeList.txt"
#define MAX_LINE_LENGTH 256

char *get_dir_path(void)
{
    const char *home = getenv("HOME");

    if (home == NULL) {
        fprintf(stderr, "Error: HOME is not set\n");
        return NULL;
    }

    size_t size = strlen(home) + strlen(ANIME_DIR) + 1;

    char *path = malloc(size);

    if (path == NULL) {
        perror("malloc");
        return NULL;
    }

    snprintf(path, size, "%s%s", home, ANIME_DIR);

    return path;
}

char *get_path(const char *filename)
{
    char *dir_path = get_dir_path();

    if (dir_path == NULL)
        return NULL;

    size_t size = strlen(dir_path) + strlen(filename) + 1;

    char *path = malloc(size);

    if (path == NULL) {
        perror("malloc");
        free(dir_path);
        return NULL;
    }

    snprintf(path, size, "%s%s", dir_path, filename);

    free(dir_path);

    return path;
}

/* Open the anime list. */
FILE *open_file(void)
{
    char *path = get_path(ANIME_FILE);

    if (path == NULL)
        return NULL;

    FILE *file = fopen(path, "a+");

    if (file == NULL)
        fprintf(stderr, "Error: Could not open %s\n", path);

    free(path);

    return file;
}

/* Return the number of lines in the file. */
int get_lines_number(FILE *file)
{
    int ch;
    int lines = 0;
    int line_has_content = 0;

    rewind(file);

    while ((ch = fgetc(file)) != EOF) {
        
        line_has_content = 1;
        
        if (ch == '\n')
            lines++;
    }

    /* if the last line doesn't end with '\n' */
    if (line_has_content && ch != '\n')
        lines++;

    rewind(file);

    return lines;
}

/* Add an anime if it doesn't already exist. */
int add_anime(FILE *file, const char *name)
{
    char line[MAX_LINE_LENGTH];

    rewind(file);

    while (fgets(line, sizeof(line), file) != NULL) {
        line[strcspn(line, "\n")] = '\0';

        if (strcmp(line, name) == 0) {
            printf("%s is already added\n", name);
            return 0;
        }
    }
    
    fseek(file, 0, SEEK_END);
   /* fprintf return the number of bytes printed on success */ 
    if (fprintf(file, "%s", name) < 0){
        perror("fprintf");
        return 1;
    }

    printf("%s was added successfully\n", name);

    return 0;
}

/* Search for an anime by name. */
int query_anime(FILE *file, const char *name)
{
    char line[MAX_LINE_LENGTH];
    int found = 0;

    rewind(file);

    while (fgets(line, sizeof(line), file) != NULL) {
        line[strcspn(line, "\n")] = '\0';

        if (strcasestr(line, name) != NULL) {
            printf("%s\n", line);
            found = 1;
        }
    }

    if (!found)
        printf("%s is not found\n", name);

    return found;
}

/* Remove an anime from the list. */
int remove_anime(FILE *file, const char *name)
{
    char line[MAX_LINE_LENGTH];
    int found = 0;

    char *temp_path = get_path("temp.txt");
    char *path = get_path(ANIME_FILE);

    if (temp_path == NULL || path == NULL) {
        free(temp_path);
        free(path);
        return 1;
    }

    FILE *temp_file = fopen(temp_path, "w");

    if (temp_file == NULL) {
        perror("fopen");
        free(temp_path);
        free(path);
        return 1;
    }

    rewind(file);

    while (fgets(line, sizeof(line), file) != NULL) {
        line[strcspn(line, "\n")] = '\0';

        if (strcmp(line, name) == 0) {
            found = 1;
            continue;
        }

        if (fprintf(temp_file, "%s\n", line) < 0) {
            perror("fprintf");
            fclose(temp_file);
            remove(temp_path);
            free(temp_path);
            free(path);
            return 1;
        }
    }

    fclose(temp_file);

    if (!found) {
        printf("%s is not present in the list\n", name);
        remove(temp_path);

        free(temp_path);
        free(path);

        return 0;
    }

    fclose(file);

    if (rename(temp_path, path) != 0) {
        perror("rename");
        free(temp_path);
        free(path);
        return 1;
    }

    printf("%s was successfully removed\n", name);

    free(temp_path);
    free(path);

    return 0;
}

