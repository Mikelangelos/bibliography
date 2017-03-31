/*
* This is a personal project from Michael Angelos Gakis which aims to track the reading and learning
* process of a person so that later you may know what has been read, what has been left half read ,
* and maybe go back and re-read some of that stuff.
* ~This projects will likely never be publicly available since this is just an expirement and the code
* will most likely not be good production code
* ~Good luck:)
*/
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

#define SizeOfArray(array) (sizeof(array) / sizeof(array[0]))

#define BIBLIOGRAPHY_BUILD "v0.0.0.1" //NOTE:Rember to update this please

#define global_variable static
#define local_persist static
#define internal static

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef float real32;
typedef double real64;
typedef int32 bool32;

// IMPORTANT: Don't touch those AND don't change the hash function
// These values are the result of the hashing the respective command
//******************
#define CMD_CREATE 6953402479289
#define CMD_HASH 210704059208
//******************

struct book
{
   uint16 ID;
   uint16 *Pages[16]; /* <-- I may be able to decrease this */
   char Name[64];
   char Author[64];
   char Desc[256];
   uint64 ISBN;
};

internal uint64
Hash(char *str)
{
   uint64 hash = 5381;
   uint32 c;
   while (c = *str++)
      hash = ((hash << 5) + hash) + c;
   return(hash);
}

internal uint32
RequestArguments(char *cmd_bfr, uint32 bfr_size,
		 char **word_list, uint32 word_list_size)
{
   gets_s(cmd_bfr, bfr_size);
   uint32 word_count = 0;

   while(*cmd_bfr != '\0') {
      while(isspace(*cmd_bfr)) //skip spaces
	 ++cmd_bfr;
      if(*cmd_bfr != '\0') {
	 word_list[word_count++] = cmd_bfr; //tag start of word
	 while(isalnum(*++cmd_bfr))
	    ;
	 if(isspace(*cmd_bfr))
	    *cmd_bfr++ = '\0';
      }
   }
   return(word_count);
}      

int main(int argc, char *argv[])
{
   printf("Welcome to bibliography %s\n",BIBLIOGRAPHY_BUILD);

   book *book_list[256];
   int32 book_index = 0;
   char cmd_buffer[256];
   char *word_list[64];
   uint32 word_count;
   
   while(word_count = RequestArguments(cmd_buffer, 256, word_list, 64)) {
      uint64 Command = Hash(word_list[0]);
      switch(Command) {
	 case CMD_CREATE: {
	    //TODO: Search if the book already exists
	    book *entry = (book *)malloc(sizeof(book));
	    printf("ISBN:");
	    uint32 check;
	    char tmp[1024];
	    while(gets_s(tmp,sizeof(tmp))) {
	       uint32 i;
	       char *scan = tmp;
	       for(i = 0; isdigit(*scan++); ++i)
		  ;
	       if(i == 10) {
		  entry->ISBN = strtol(tmp,NULL,10);
		  break;
	       } else {
		  printf("Please enter a valid ISBN-10 number!\n>");
	       }
	    }
	    printf("Name:");
	    //TODO:ADD CHECKS
	    gets_s(entry->Name, sizeof(entry->Name));
	    printf("Author:");
	    gets_s(entry->Author, sizeof(entry->Author));
	    printf("Desc:");
	    gets_s(entry->Desc, sizeof(entry->Desc));
	    entry->ID = book_index;
	    book_list[book_index++] = entry;

	    printf("Book successfully registered:\nID:%d\nName:%s\nAuthor:%s\nDesc:%s\nISBN:%llu\n",
		   entry->ID, entry->Name, entry->Author, entry->Desc, entry->ISBN);
	 } break;
	    
	 case CMD_HASH: {
	    if(word_count > 1)
	       printf("%s : %llu\n",word_list[1], Hash(word_list[1]));
	    else
	       printf("# No Hash-able word found! #\n");
	 } break;

      }
      
#if 0
      printf("Words: %d\n",word_index);
      for(int32 i = 0; i < word_index; ++i) {
	 printf("%d: %s\n",i, word_list[i]);
      }
#endif
      
   }
   
   return(0);
}

