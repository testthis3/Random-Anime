#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "file.h"
#include "display.h"
#include "api.h"


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
            int random_line = rand() % lines + 1;
            
            char *anime_name = print_line(file, random_line);
            if (anime_name == NULL) {
                fclose(file);
                return EXIT_FAILURE;
            }           
            
            // printf("%s", anime_name);
            if (anime_info(anime_name))
                printf("\n%s\n", anime_name);

            
            free(anime_name);
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

        if (result == 0)
            return EXIT_SUCCESS;
    }
    else if (strcmp(argv[1], "query") == 0) {
        query_anime(file, argv[2]);
    }
    else if (strcmp(argv[1], "info") == 0){
        anime_info(argv[2]);
    }
    else {
        print_usage(argv[0]);
        result = 1;
    }

    fclose(file);

    return result == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
