#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <sqlite3.h>

#include "stringbuilder.h"

#define DB_NAME         "database.db"
#define CREATE_TABLE    "CREATE TABLE"

static int callback(void *NotUsed, int argc, char **argv, char **azColName){
    int i;
    for(i=0; i<argc; i++){
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

static int executeSQL(sqlite3 *db, char* query){
    char *zErrMsg = 0;
    if(sqlite3_exec(db, query, callback, 0, &zErrMsg) != SQLITE_OK){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return -1;
    }
    return 1;
}

static int readFile(char *filename, sqlite3 *db){
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    
    fp = fopen(filename, "r");
    if (fp == NULL){
        return -1;
    }
    
    // Used for create request only
    StringBuilder *sb = NULL;
    char *str = NULL;
    
    while ((read = getline(&line, &len, fp)) != -1) {
        
        if (read > 0 && strstr(line, CREATE_TABLE) != NULL) {
            // begin of create
            sb = sb_create();
            sb_appendf(sb, line);
        }else if (sb!=NULL && line[read-3] == ')' && line[read-2] == ';'){
            // end of create
            sb_appendf(sb, line);
            str = sb_concat(sb);
            
            if (executeSQL(db, str) == -1){
                return -1;
            }
            
            sb_free(sb);
            sb = NULL;
            free(str);
            
        }else if (sb!=NULL && read > 0){
            // within a create
            sb_appendf(sb, line);
        }else if (sb==NULL && read > 0 && line[read-2] == ';'){
            // simple line
            if (executeSQL(db, line) == -1){
                return -1;
            }
        }
    }
    
    fclose(fp);
    if (line)
        free(line);
    
    return 0;
}

int main(int argc, const char * argv[]) {
    
    int i;
    unsigned short arguments[4] = {0}, v = 0;
    const char *commands[]={"-c", "-i", "-s", "-d", "-v", NULL};
    enum {SKIP_CREATE, SKIP_INSERT , SKIP_SELECT, SKIP_DELETE, VERBOSE};
    if (argc==2 && (strcmp(argv[1], "--help") ==0 || strcmp(argv[1], "-h") ==0)) {
    usage:   printf ("Usage: %s [commands]\n"
                     "-c  Skip creation of the database (default = no).\n"
                     "-i  Skip INSERT queries (default = no).\n"
                     "-s  Skip SELECT queries (default = no).\n"
                     "-d  Skip DELETE queries (default = no).\n"
                     "-v  verbose mode (default = no).\n",argv[0]);
        return 1;
    }else if(argc > 1){
        for (i = 0; commands[i] && strcmp(argv[1], commands[i]); i++);
        switch (i) {
            case SKIP_CREATE:
                arguments[SKIP_CREATE] = 1;
                break;
            case SKIP_INSERT:
                arguments[SKIP_INSERT] = 1;
                break;
            case SKIP_SELECT:
                arguments[SKIP_SELECT] = 1;
                break;
            case SKIP_DELETE:
                arguments[SKIP_DELETE] = 1;
                break;
            case VERBOSE:
                v = 1;
                break;
            default:
                printf("Unknown command...");
                goto usage;
        }
    }
    
    
    sqlite3 *db;
    
    if(sqlite3_open(DB_NAME, &db) < 0){
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(EXIT_FAILURE);
    }
    
    if (!arguments[SKIP_CREATE]){
        if (v) puts("Creating SQL tables...");
        if (readFile("queries/tables.sql", db) < 0){
            fprintf(stderr, "Error while parsing file\n");
            sqlite3_close(db);
            exit(EXIT_FAILURE);
        }
        if (v) puts("-> Creating SQL tables [DONE]");
    }
    
    if (!arguments[SKIP_INSERT]){
        if (v) puts("Inserting Data...");
        if (readFile("queries/data.sql", db) < 0){
            fprintf(stderr, "Error while parsing file\n");
            sqlite3_close(db);
            exit(EXIT_FAILURE);
        }
        if (v) puts("-> Inserting Data [DONE]");
    }
    
    if (!arguments[SKIP_SELECT]){
        if (v) puts("Executing SELECT queries...");
        if (readFile("queries/select.sql", db) < 0){
            fprintf(stderr, "Error while parsing file\n");
            sqlite3_close(db);
            exit(EXIT_FAILURE);
        }
        if (v) puts("-> Executing SELECT queries... [DONE]");
    }
    
    if (!arguments[SKIP_DELETE]){
        if (v) puts("Executing DELETE queries...");
        if (readFile("queries/delete.sql", db) < 0){
            fprintf(stderr, "Error while parsing file\n");
            sqlite3_close(db);
            exit(EXIT_FAILURE);
        }
        if (v) puts("-> Executing DELETE queries... [DONE]");
    }
    
    sqlite3_close(db);
    return 0;
}
