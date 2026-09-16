#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>
#include <cjson/cJSON.h>

#define PLOT_WIDTH 70

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

void print_plot(const char *description)
{
    int column = 3;  // account for "   "

    printf("\n   Plot\n   ");

    for (size_t i = 0; description[i] != '\0'; i++) {

        /* Handle <br> */
        if (strncmp(&description[i], "<br>", 4) == 0) {
            printf("\n   ");
            column = 3;
            i += 3;
            continue;
        }

        /* Print a word without exceeding the width */
        if (description[i] == ' ') {
            putchar(' ');
            column++;
            continue;
        }

        /* Find length of the next word */
        size_t word_start = i;
        size_t word_len = 0;

        while (description[i + word_len] != '\0' &&
               description[i + word_len] != ' ' &&
               strncmp(&description[i + word_len], "<br>", 4) != 0) {
            word_len++;
        }

        /* Wrap before the word */
        if (column + word_len > PLOT_WIDTH) {
            printf("\n   ");
            column = 3;
        }

        printf("%.*s", (int)word_len, &description[word_start]);
        column += word_len;

        i += word_len - 1;
    }

    printf("\n");
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
            print_plot(description);
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

