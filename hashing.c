#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME 255
#define M 10
#define R 3

int power(int, int);
int calculateKey(FILE *, int);
void printHash(char **);
void addToHash(char **, int, char[]);
int len_of_file(FILE *);
int compareDocs(FILE *, FILE *);
void copyToDir(FILE *doc1, FILE *doc2);

int main() {
    //samples.doc operations
    FILE *fp;
    fp = fopen("samples.doc", "r");
    if(fp == NULL) {
        printf("There is a problem opening samples.doc\n");
        return 0;
    }

    char **hash = (char **) malloc(sizeof(char *) * M);
    int i;
    for(i = 0; i < M; i++)
        hash[i] = NULL;

    char file_name[MAX_NAME];   //to keep the file names in the samples.doc
    char file_path[MAX_NAME];   //to keep the file paths

    printf("%-30s%-30s\n", "----------------------", "----------------------");
    printf("%-30s%-30s\n","File Name", "Hash Number");
    while(fscanf(fp, "%s\n", file_name) != EOF) {   //read the samples.doc file
        strcpy(file_path, "directory/");
        strcat(file_path, file_name);   //set the file_path

        printf("%-30s", file_path);

        FILE *doc = fopen(file_path, "r");

        if(doc == NULL) {
            printf("There is a problem opening %s\n", file_path);
            return 0;
        }

        int hashNum1 = calculateKey(doc, M) % M;
        int hashNum2 = calculateKey(doc, M - 1) % (M - 1);
        int hashNum = hashNum1;

        i = 0;
        while(hash[hashNum] != NULL){
            hashNum = (hashNum1 + (++i * (1 + hashNum2 ))) % M;
        }

        printf("%-30d\n", hashNum);
        addToHash(hash, hashNum, file_name);
        fclose(doc);
    }
    printf("%-30s%-30s\n", "----------------------", "----------------------");
    fclose(fp); //close sample.doc
    printf("\n");
    printHash(hash);

    int isContinue = 1;
    do {
        char newFileName[MAX_NAME];
        printf("\nEnter the name of the document you want to add (-1 to quit): ");
        scanf("%s", newFileName);

        if(!strcmp("-1", newFileName)) {
            isContinue = 0;
        }

        //if the doc name is not -1
        else {
            FILE *newDoc;   //the doc that user wants to add
            newDoc = fopen(newFileName, "r");

            if(newDoc == NULL) {
                printf("There is a problem opening %s\n", newFileName);
                return 0;
            }
            //calculate the hash number
            int hashNum1 = calculateKey(newDoc, M) % M;
            int hashNum2 = calculateKey(newDoc, M - 1) % (M - 1);
            int hashNum = hashNum1;
            int isSame = 0;
            i = 0;

            FILE *docInDir = NULL;  //to keep the files with the same hash number

            while(hash[hashNum] != NULL && !isSame){    //continue until find the same doc or free index in the hash[]

                //open the doc whose hash number is the same as newDoc
                strcpy(file_path, "directory/");
                strcat(file_path, hash[hashNum]);
                docInDir = fopen(file_path, "r");

                if(docInDir == NULL) {
                    printf("There is a problem opening %s\n", file_path);
                    return -1;
                }

                if(!compareDocs(newDoc, docInDir)) {    //if the docs are the same, change the flag
                    isSame = 1;
                }

                else {  //if the docs are not same, create new hash number
                    hashNum = (hashNum1 + (++i * (1 + hashNum2 ))) % M;
                }

                fclose(docInDir);
            }

            if(!isSame) {   //if there is no same doc

                //adding to the hash-------------
                addToHash(hash, hashNum, newFileName);    //add the doc to the hash array
                //end adding to the hash----------------

                //copying the new doc into the directory--------------------
                strcpy(file_path, "directory/");
                strcat(file_path, newFileName); //set the file path 'directory/%newDocName

                FILE *copyNewDoc;   //copy the doc to the directory/
                copyNewDoc = fopen(file_path, "w");

                copyToDir(newDoc, copyNewDoc);    //copy the doc into the directory
                fclose(copyNewDoc); //close the copy doc in the directory
                //end copying the new doc into the directory---------------------

                //adding the newDocName to the end of sample.doc-----------------------
                fp = fopen("samples.doc", "a");
                fprintf(fp, "\n%s", newFileName);
                fclose(fp);
            }

            else
                printf("%s is already in the directory.\n", newFileName);

            printHash(hash);
        }

    }while(isContinue == 1);

    fclose(fp);
}

int power(int x, int y) {
    int i, calc = 1;
    for(i = 0; i < y; i++) {
        calc = calc * x;
    }
    return calc;
}

int calculateKey(FILE *doc, int mod) {
    fseek(doc, 0, SEEK_SET);
    int strlen = 0; //keeps the length of the current string
    int index = 0, prevIndex = 0;
    int upperCaseFlag;

    int key = 0;

    char ch;
    while( (ch = fgetc(doc)) != EOF) {

        upperCaseFlag = 0;  //reset the upper case flag
        strlen = 0;
        do {
            if( (int) ch >= 'A' && (int) ch <= 'Z')
                upperCaseFlag = 1;

            strlen++;
        } while((ch = fgetc(doc)) != EOF && ch != ' ');

        prevIndex = index;
        index += strlen + 1;

        if(!upperCaseFlag) {    //if there is no uppercase letter
            fseek(doc, prevIndex, SEEK_SET);
            while( (ch = fgetc(doc)) != EOF && ch != ' ') {
                printf("%c ", ch);
                key += ((int) ch) * power(R, --strlen) % mod;
            }
        }


    }

    return key;
}

int len_of_file(FILE *doc) {
    fseek(doc, 0, SEEK_END);
    return ftell(doc);
}

void addToHash(char **hash, int hashNum, char file_name[]) {
    char *name = (char *) malloc(sizeof(char) * (strlen(file_name) + 1));

    strcpy(name, file_name);
    hash[hashNum] = name;
}

void printHash(char **hash) {
    int i;
    printf("%-30s%-30s\n", "----------------------", "----------------------");
    printf("%-30s%-30s\n", "Index", "Hash[Index]");
    for(i = 0; i < M; i++) {
        printf("%-30d%-30s\n", i, hash[i]);
    }
    printf("%-30s%-30s\n", "----------------------", "----------------------");
}

int compareDocs(FILE *doc1, FILE *doc2) {
    int lenDoc1 = len_of_file(doc1);
    int lenDoc2 = len_of_file(doc2);

    fseek(doc1, 0, SEEK_SET);    //reset the pointer of txt file
    fseek(doc2, 0, SEEK_SET);    //reset the pointer of txt file

    char *contentDoc1 = (char *) malloc(sizeof(char) * lenDoc1);
    char *contentDoc2 = (char *) malloc(sizeof(char) * lenDoc2);

    fgets(contentDoc1, lenDoc1, doc1);
    //printf("contentDoc1 %s:\n", contentDoc1);
    fgets(contentDoc2, lenDoc2, doc2);
    //printf("contentDoc2 %s:\n", contentDoc2);

    return strcmp(contentDoc1, contentDoc2);
}

void copyToDir(FILE *doc1, FILE *doc2) {
    fseek(doc1, 0, SEEK_SET);

    fseek(doc2, 0, SEEK_SET);

    char c;
    c = fgetc(doc1);
    while (c != EOF)
    {
        fputc(c, doc2);
        c = fgetc(doc1);
    }
}
