// file.h
#ifndef FILE_H
#define FILE_H

char *get_dir_path(void);
char *get_path(const char *filename);
FILE *open_file(void);
int get_lines_number(FILE *file);
int add_anime(FILE *file, const char *name);
int query_anime(FILE *file, const char *name);
int remove_anime(FILE *file, const char *name);

#endif
