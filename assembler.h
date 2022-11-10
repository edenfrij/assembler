
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
/*lengths*/
#define WORD_LEN 12
#define LINE_LEN 82
#define FIRST_ADDRESS_IC 100
#define FIRST_ADDRESS_DC 0
#define MAX_LABEL 31
#define NOT_EXIST -1
#define MAX_NAME_LEN 1000
#define MAX_GUIDANCE 8
#define MAX_ATTRIBUTES 9
#define COMMANDS_NUMBER 16
/*define the 4 methods of addressing*/
#define IMMEDIATE 0
#define DIRECT 1
#define RELATIVE 2
#define REGISTER 3
/*lengths for the initialization*/
#define FUNCT_LEN 4
#define OPCODE_LEN 4
#define METHOD_LEN 2

/*boolean types FALSE=0, TRUE=1*/
typedef enum {FALSE,TRUE}boolean;

/*a struct for a symbol*/
typedef struct symbols{
		char name[MAX_LABEL];
		int value;
		char attributes[MAX_ATTRIBUTES];
		struct symbols *next;
	        }symbol;

/*a struct for a data word*/
typedef struct data_words{
			unsigned short code;
			char ARE;
			int DC;
			struct data_words *next;
		       }data_word;

/*a struct for a code word*/
typedef struct code_words {
			  unsigned int code;
			  char ARE;
			  int IC;
			  struct code_words *next;
			 }code_word;

/*the main function that translate the files to a mechine laguage, the fuction reads the file in two transitions line by line from the file until EOF.
The function holds three linked list, one holds the code one holde the data and the last one hold the symbol list.
in each line that the function reads the function it checks if there were any errors in it, if there were error the function return false.
If there were any errors in the file the function will no produce ext and ent files*/
void assembler(FILE *file,char *file_name);
/*this function frees the dynamic memory that has been used to create the word*/
void free_code_word(code_word *head);
/*this function frees the dynamic memory that has been used to create the word*/
void free_data_word(data_word *head);
/*this function frees the dynamic memory that has been used to create the symbol*/
void free_symbol(symbol *head);
/*this function prints the whole code list to the ob file*/
void print_code_list(code_word **,FILE *);
/*this function prints the whole data list to the ob file*/
void print_data_list(data_word **,FILE *,int);
/*this function passes over the file for the first round. the function receives a line after a line from the assembler.c and analyzes it.
If there are errors in the file the function print the errors and return TRUE(an error exist).
The function analyzes the given line char by char.
If there aren't any mistakes that have been discovered it return FALSE (an error not exist)*/
boolean first_transition(char line[],int *IC,int *DC,symbol **head_symbol,int line_num,data_word **head_data,char *file_name);
/*this function passes over the file for the first round. the function receives a line after a line from the assembler.c and analyzes it.
If there are errors in the file the function print the errors and return TRUE(an error exist).
The function analyzes the given line char by char.
If there aren't any mistakes that have been discovered it return FALSE (an error not exist)*/
boolean second_transition(char line[],int *IC,int *DC,symbol **head_symbol,int line_num,code_word **head_code,FILE *ext,FILE *ent,char *file_name);			
/*This functions skippes the whitespace from the begining of a given string and returns the first index from the left side which is not a whitespace*/
int skip_whitespaces_begining(char c[],int j);
/*this function updates the addresses of the symbol which their attributes is 'data' to the adderes+ the final ICF */
void address_update(symbol *head_symbol,int IC);
/*skippes the whitespaces from the end of a given string and returns the index of the first char from the right side+1(the whitespaceafter the char) */
int skip_whitespaces_end(char s[],int end);
/*this function adds new symbol to the symbol table*/
void add_symbol (char symbolName[],int IC, char *attribute ,symbol **head);
/*this function recives the details that are needed to add a symbol and adds the new symbol to the symbol list*/
void add_data(unsigned short code, data_word **head_data, int address, char ARE);
/*checks if given string is an integer, if the string contains characters which are not numbers the function return false otherwise the function returns true*/
boolean isnumber(char number[]);
/*checks if a given string is a register if it is returns true otherwise return false*/
boolean isregister(char reg[]);
/*this function checks if a given string is one of the 16 instructions if it is it returns and int which combining the opcode and the funct in a specific way (explanation inside the function) and if a command is not found it returns -1 */
int which_command(char *command);
/*checks if the label is legal, returns true if the name is legal and false otherwise*/
boolean symbol_valid(char s[],int line_number,symbol **head_symbol,char *name_of_file);
/*the function returns the complement to base 2 of given negative number*/
int base_comp(char value[]);
/*the function copies the first n characters from src string to dest string and adding \0 in the end*/
void substring(char dest[],char src[],int n);/*adding null character in the end of the copied string*/
/*this function returns the first place in a given string that contain the character we look for, if the char doesn't exist in the string the function returns -1*/
int find_char(char str[],char c,int i);
/*this functions checks if a given string exists in the symbol list if it is it return a pointer to this symbol*/
symbol *symbol_exist(char label[],symbol **head_symbol);
/*this function checks if a given operand is illegal if the operand is illegal ut return true otherwise it initializes the word with this operand*/
boolean method_type(char operand[],int *IC,symbol **head_symbol,code_word **operand_word,int *op_method,FILE *ext,char *file_name,int line_num);
/*this function adds a code word to the end of the code list*/
void add_list (code_word **head,code_word *node);
/*the function initialize a code word with given values and returns the pointer to that code word, the operands words*/
code_word *initialize_word(unsigned int address,char are, int ic);
/*the function create the first word(opcode,funct,op1,op2) and returns the pointer to that word*/
code_word *create_word(int opcode_funct, int src_method,int dest_method, char are, int ic);
/*this method checks the syntax of a given line regarding immediate and relative addresing methodes */
boolean syntax_method(char operand[],int line_num,char *file_name);






		

			
			

		


