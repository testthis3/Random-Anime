#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>

#define MAX_LINE_LENGTH 256
#define ANIME_DIR "/Projects/C/RandomAnime/"
#define ANIME_FILE "AnimeList.txt"

struct MemoryStruct {
    char *memory;
    size_t size;
};

// function for WRITEFUNCTION option in curl_easy_setopt()
size_t CallBack(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    char *ptr = realloc(mem->memory, mem->size + realsize +1);
    if (ptr == NULL){
        printf("Not enough memory (realloc returned NULL)\n");
        return 0; 
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;
    
    return realsize;
}

int anime_info(const char *anime_name) 
{
    struct MemoryStruct chunk;

    chunk.memory = malloc(1);
    if (chunk.memory == NULL){
        perror("malloc");
        return 1;
    }

    chunk.size = 0;

    /* Initialize global curl environment */
    curl_global_init(CURL_GLOBAL_ALL);
    CURL *curl_handle = curl_easy_init();

    if (!curl_handle) 
    {
        fprintf(stderr, "Failed to initialize CURL\n");
        free(chunk.memory);
        return 1;
    }

    /* Define the Anilist GraphQl query */
    const char *graphql_query = 
        "query ($searchName: String) {"
        "  Media (search: $searchName, type: ANIME) {"
        "    title {"
        "      english"
        "      romaji"
        "      native"
        "    }"
        "    status"
        "    seasonYear"
        "    episodes"
        "    genres"
        "    description"
        "  }"
        "}";
    
    /* Build the JSON payload usin cJSON */
    cJSON *root = cJSON_CreateObject(); // creates {}
    cJSON_AddStringToObject(root, "query", graphql_query); // creates { "query":{ graphql }}

    cJSON *variables = cJSON_CreateObject(); 
    cJSON_AddStringToObject(variables, "searchName", anime_name); 

    cJSON_AddItemToObject(root, "variables", variables);

    char *json_payload = cJSON_Print(root); // in json format 

    /* Configure http headers */
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "Accept: application/json");
   
    /* Configure CURL options */
    curl_easy_setopt(curl_handle, CURLOPT_URL,"https://graphql.anilist.co");
    curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, json_payload);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, CallBack);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);

    /* Execute the network request */
    CURLcode result = curl_easy_perform(curl_handle);
    if (result != CURLE_OK) 
    {
        fprintf(stderr, "curl_easy_perform failed: %s\n", curl_easy_strerror(result));
    } 
    else 
    {
        /* parse chunk.memory cause it contains anime info in a json format */
        cJSON *parsed_data = cJSON_Parse(chunk.memory);
        
        if (parsed_data == NULL) 
        {
            const char *error_ptr = cJSON_GetErrorPtr();
            if (error_ptr != NULL) 
            {
                fprintf(stderr, "Error before: %s\n", error_ptr);
            }
            return 1;
        }

        cJSON *data = cJSON_GetObjectItem(parsed_data, "data");
        cJSON *media = cJSON_GetObjectItem(data, "Media");
      
        if (!data)
            fprintf(stderr, "data Anime not found or invalid API response\n");
            
        if (!data || !media) {
            fprintf(stderr, "Anime not found or invalid API response\n");
            cJSON_Delete(parsed_data);
            return 1;
        }
        cJSON *title = cJSON_GetObjectItem(media, "title");
        cJSON *genres = cJSON_GetObjectItem(media, "genres");

        /* Print available title in this order: english, romaji, native, anime_name (name entered by user)*/
        cJSON *english = cJSON_GetObjectItemCaseSensitive(title, "english");
        cJSON *romaji = cJSON_GetObjectItemCaseSensitive(title, "romaji");
        cJSON *native = cJSON_GetObjectItemCaseSensitive(title, "native");
        
        const char *display_title = anime_name;

        if (cJSON_IsString(english) && (english->valuestring != NULL)) 
            display_title = english->valuestring;
        else if (cJSON_IsString(romaji) && (romaji->valuestring != NULL)) 
            display_title = romaji->valuestring;
        else if (cJSON_IsString(native) && (native->valuestring != NULL)) 
            display_title = native->valuestring;
        
        printf("\n──────────────────────────────────────\n");
        printf("  %s\n", display_title);
        printf("──────────────────────────────────────\n\n");
       
        //printf("Title: %s\n", display_title);
         
        cJSON *status = cJSON_GetObjectItemCaseSensitive(media, "status");
        if (cJSON_IsString(status) && (status->valuestring != NULL)) {
             printf("   Status   : %s\n", status->valuestring);
        }

        cJSON *episodes = cJSON_GetObjectItemCaseSensitive(media, "episodes");
        if (cJSON_IsNumber(episodes)) {
             printf("   Episodes : %d\n", episodes->valueint);
        }
        
        cJSON *year = cJSON_GetObjectItemCaseSensitive(media, "seasonYear");
        if (cJSON_IsNumber(year)) {
             printf("   Release  : %d\n", year->valueint);
        }
       
        printf("   Genres   : ");
        // genres is an array
        cJSON *genre;
        cJSON_ArrayForEach(genre, genres)
        {
            printf("%s ", genre->valuestring);
        }

        cJSON *plot = cJSON_GetObjectItemCaseSensitive(media, "description");
        char *description = plot->valuestring;
        
        if (cJSON_IsString(plot) && (description != NULL)) {
            char *p = description;

            while ((p = strstr(p, "<br>")) != NULL) {
                *p = ' ';
                memmove(p + 1, p + 4, strlen(p + 4) +1);
                p++;
            }

            printf("\n   Plot\n   ");
            for (size_t i = 0; i < strlen(description); i++){
                char c = description[i];
                putchar(c);
                if ( c == '.')
                    printf("\n  ");
            } 
            printf("\n");

        }

    }
    
    /* Clean up allocated resources */
    free(json_payload);
    cJSON_Delete(root);
    curl_slist_free_all(headers);
    free(chunk.memory);
    curl_easy_cleanup(curl_handle);
    curl_global_cleanup();
    
    return 0;
}



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

void print_usage(const char *program)
{
    printf("Usage:\n");
    printf("  %s\n", program);
    printf("  %s add \"anime name\"\n", program);
    printf("  %s remove \"anime name\"\n", program);
    printf("  %s query \"anime name\"\n", program);
    printf("  %s info \"anime name\"\n", program);
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
            int random_line = rand() % lines + 1;
            
            char *anime_name = print_line(file, random_line);
            if (anime_name == NULL) {
                fclose(file);
                return EXIT_FAILURE;
            }           
            
            // printf("%s", anime_name);
            anime_info(anime_name);
            
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
