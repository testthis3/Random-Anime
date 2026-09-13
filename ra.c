#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_LINE_LENGTH 256
#define ANIME_DIR "/Projects/C/RandomAnime/"
#define ANIME_FILE "AnimeList.txt"

/*
 * Return:
 *     $HOME/Projects/C/RandomAnime/
 *
 * The returned string must be freed by the caller.
 */
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

/*
 * Return:
 *     $HOME/Projects/C/RandomAnime/<filename>
 *
 * The returned string must be freed by the caller.
 */
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

    rewind(file);

    while ((ch = fgetc(file)) != EOF) {
        if (ch == '\n')
            lines++;
    }

    rewind(file);

    return lines;
}

/* Return a random number between 1 and max. */
int get_random_number(int max)
{
    return rand() % max + 1;
}

/* Print a specific line from the file. */
void print_line(FILE *file, int line_number)
{
    char line[MAX_LINE_LENGTH];
    int current_line = 1;

    rewind(file);

    while (fgets(line, sizeof(line), file) != NULL) {
        if (current_line == line_number) {
            printf("%s", line);
            return;
        }

        current_line++;
    }

    fprintf(stderr, "Error: Line %d not found\n", line_number);
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

    /*
     * We have finished reading the file.
     * Reposition the stream before writing.
     */
    fseek(file, 0, SEEK_END);

    if (fprintf(file, "%s\n", name) < 0) {
        perror("fprintf");
        return 1;
    }

    printf("%s was added successfully to the anime list\n", name);

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

    /*
     * Close the original file before replacing it.
     */
    fclose(file);

    /*
     * On Linux, rename() can replace the existing file.
     * This is better than remove() followed by rename().
     */
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

void print_usage(const char *program)
{
    printf("Usage:\n");
    printf("  %s\n", program);
    printf("  %s add \"anime name\"\n", program);
    printf("  %s remove \"anime name\"\n", program);
    printf("  %s query \"anime name\"\n", program);
}

int main(int argc, char *argv[])
{
    srand(time(NULL));

    FILE *file = open_file();

    if (file == NULL)
        return EXIT_FAILURE;

    if (argc == 1) {
        int lines = get_lines_number(file);

        if (lines == 0) {
            printf("The anime list is empty.\n");
        } else {
            int random_line = get_random_number(lines);
            print_line(file, random_line);
        }

        fclose(file);
        return EXIT_SUCCESS;
    }

    if (argc != 3) {
        print_usage(argv[0]);
        fclose(file);
        return EXIT_FAILURE;
    }

    int result = 0;

    if (strcmp(argv[1], "add") == 0) {
        result = add_anime(file, argv[2]);
    }
    else if (strcmp(argv[1], "remove") == 0) {
        result = remove_anime(file, argv[2]);

        /*
         * remove_anime() closes the file itself
         * when it successfully replaces it.
         */
        if (result == 0)
            return EXIT_SUCCESS;
    }
    else if (strcmp(argv[1], "query") == 0) {
        query_anime(file, argv[2]);
    }
    else {
        print_usage(argv[0]);
        result = 1;
    }

    fclose(file);

    return result == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
