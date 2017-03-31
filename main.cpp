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

#define BIBLIOGRAPHY_BUILD "v0.0.1.1" //NOTE:Rember to update this please

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
#define CMD_SAVE 210704454611
#define CMD_LIST 6385440353
#define CMD_EDIT 6385183019
#define CMD_RELOAD 6953974359388
#define CMD_UPDATE 6954104508584
//******************

struct book
{
   uint16 ID;
   int16 Pages[16]; /* <-- I may be able to decrease this */
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
//TODO: I need to better secure this process against random symbols and also decide if I will allow them
   while(*cmd_bfr != '\0') {
      while(isspace(*cmd_bfr)) //skip spaces
	 ++cmd_bfr;
      if(*cmd_bfr != '\0') {
	 word_list[word_count++] = cmd_bfr; //tag start of word
	 while(isalnum(*++cmd_bfr))
	    ;
	 if(*cmd_bfr != '\0')
	    *cmd_bfr++ = '\0';
      }
   }
   return(word_count);
}      

//TODO: Look at this again later to see if I can simplify it a bit
internal void
SaveToFile(book **book_list, uint32 book_index)
{
   uint32 page_buf_size = SizeOfArray(book_list[0]->Pages);
   FILE *save_file = fopen("bibliography_list.txt","w");
   for(uint32 i = 0; i < book_index; ++i) {
      book *Book = book_list[i];
      fprintf(save_file ,"{\n\tID :%d,\n\tISBN :%llu,\n\tName :%s,\n\tAuthor :%s,\n\tDesc :%s,\n\tPages :\n",
	      Book->ID, Book->ISBN, Book->Name, Book->Author, Book->Desc);
      for(uint32 j = 0; Book->Pages[j] != -1 && j < page_buf_size; j+=2)
	 fprintf(save_file, "\t( %d - %d )\n",Book->Pages[j], Book->Pages[j+1]);
      fprintf(save_file, "}\n");
   }
   fclose(save_file);
}


internal uint32
ReadFromFile(book **book_list)
{
   FILE *rf = fopen("bibliography_list.txt","r");
   uint32 index = 0;
   char *buffer;
   if(rf) {
      char c;
      while((c = fgetc(rf)) != EOF)
      {
	 if(isspace(c))
	    continue;

	 if(c == '{') {
	    book *entry = (book *)malloc(sizeof(book));
	    
	    while((c = fgetc(rf)) != ':')
	       ;
	    fscanf(rf,"%hd",&entry->ID);
	    while((c = fgetc(rf)) != ':')
	       ;
	    fscanf(rf,"%llu",&entry->ISBN);
	    while((c = fgetc(rf)) != ':')
	       ;
	    fscanf(rf,"%[^,]",&entry->Name);
	    while((c = fgetc(rf)) != ':')
	       ;
	    fscanf(rf,"%[^,]",&entry->Author);
	    while((c = fgetc(rf)) != ':')
	       ;
	    fscanf(rf,"%[^,]",&entry->Desc);
	    while((c = fgetc(rf)) != ':')
	       ;
	    for(uint32 i = 0; i < 16; i+=2) {
	       do
		  c = fgetc(rf);
	       while(isspace(c));
	       if(c == '}') {
		  while(i < 16)
		     entry->Pages[i++] = -1;
		  continue;
	       }
	       ungetc(c,rf);
	       fscanf(rf,"( %hd - %hd )",&entry->Pages[i] , &entry->Pages[i+1]);
	    }
	    book_list[index++] = entry;
	 } else {
	    continue;
	 }
      }
   }
   fclose(rf);
   return index;
}


int main(int argc, char *argv[])
{
   printf("Welcome to bibliography %s\n",BIBLIOGRAPHY_BUILD);
//-----
   book *book_list[256];
   int32 book_index = 0;
   char cmd_buffer[256];
   char *word_list[64];
   uint32 word_count;
//-----
   book_index = ReadFromFile(book_list);
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
		  printf("Please enter a valid ISBN-10 number!\n");
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
	    for(uint32 i = 0; i < SizeOfArray(entry->Pages);++i)
	       entry->Pages[i] = -1;
	    book_list[book_index++] = entry;
	    SaveToFile(book_list, book_index);
	    printf("-----*Book successfully registered*-----\n");
	 }break;
	    
	 case CMD_HASH: {
	    if(word_count > 1)
	       printf("%s : %llu\n",word_list[1], Hash(word_list[1]));
	    else
	       printf("# No Hash-able word found! #\n");
	 } break;

	 case CMD_LIST: {
	    if(book_index > 0) {
	       uint32 pages_bfr_len = SizeOfArray(book_list[0]->Pages);
	       for(uint32 i = 0; i < book_index; ++i) {
		  book *Book = book_list[i];
		  printf("\n\t-----|ID:%3d|-----\n  Name:\t%s\nAuthor:\t%s\n  ISBN:\t%llu\n  Desc:\t%s\n Pages:\n",
			 Book->ID, Book->Name, Book->Author ,Book->ISBN ,Book->Desc);
		  for(uint32 j = 0; Book->Pages[j] != -1 && j < pages_bfr_len; j+=2) //-1 == Not set
		     printf("\t(%3d - %3d)\n",Book->Pages[j], Book->Pages[j+1]);
	       }
	    } else {
	       printf("# No books were found! #\n");
	    }
	 }break;

	 case CMD_UPDATE: {
	    if(word_count == 4) {
	       //TODO:Add checks!
	       int32 book_id = strtol(word_list[1], NULL, 10);
	       int16 page_from = strtol(word_list[2], NULL, 10);
	       int16 page_to = strtol(word_list[3], NULL, 10);
	       {
		  if(book_id < book_index) {
		     int16 *Pages = book_list[book_id]->Pages;
		     int16 page_lowest;
		     int16 page_highest;
		     if(Pages[0] != -1) //NOTE: Non-used page pointers hold the -1 value
			for(uint32 i = 0; Pages[i] != -1 && i < SizeOfArray(Pages); ++i) {
			   //TODO: Make sure that the first page will always be a smaller number than the right one.
			   page_lowest = Pages[i];
			   page_highest = Pages[++i];
			   //TODO: THIS NEEDS TO HANDLE MORE CASES!!
			   if(page_from <= page_highest && page_to > page_highest) {
			      Pages[i] = page_to;
			      break;
			   }
			   if(page_from >= page_lowest && page_to <= page_highest) {
			      break;
			   }
			}
		     else {
			Pages[0] = page_from;
			Pages[1] = page_to;
		     }
		     SaveToFile(book_list, book_index);
		     printf("-----*Book successfully updated*-----\n");
		  } else {
		     printf("# Book with ID(%d) was not found! #\n",book_id);  
		  }
	       }
	    } else {
	       printf("Wrong format. Use \"update ID FROM TO\"");
	    }
	 }break;

	 case CMD_EDIT: {
	    //TODO: Implement
	    printf("~ Not yet implemented.. ~\n");
	 }break;
	    
	 case CMD_SAVE: {
	    SaveToFile(book_list, book_index);
	 }break;

	 case CMD_RELOAD: {
	    book_index = ReadFromFile(book_list);
	 }break;
	    
	 default: {
	    printf("# Unknown Command #\n");
	 }break;
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

