#include "assembler.h"

/*This function skippes the whitespaces from the begining of a given string and returns the first index of character from the left side which is not a whitespace*/
int skip_whitespaces_begining(char c[],int j)
{
	for(;c[j]==' '||c[j]=='\t';j++);/*ignoring white spaces at the begining*/
	return j;
}
/*This function skippes the whitespaces from the end of a given string and returns the index of the first char from the right side +1 (the whitespaceafter the char) */
int skip_whitespaces_end(char s[],int end)
{
	int i;
	for(i=end;i>0;i--)
		if(!isspace(s[i]))
			return i+1;
	return 1;
}

/*this function recives the details that are needed to add a symbol and adds the new symbol to the symbol list*/
void add_symbol (char symbolName[],int IC, char *attribute ,symbol **head)
{
	symbol *temp1;
	symbol *temp2;
	temp1=(symbol *)malloc(sizeof(symbol));
	if(!temp1)
	{
		fprintf(stdout,"allocation memory failed, program will shut down\n");
		exit(0);
	}
	/*initialize the information to the new symbol*/
	temp1->next=NULL;
	strcpy(temp1->name,symbolName);
	temp1->value=IC;
	strcpy(temp1->attributes,attribute);
	if((*head)==NULL)/*adding to the end of the table*/
		(*head)=temp1;
	else
	{
		for(temp2=(*head);temp2->next;temp2=(temp2->next));
		/*adding the symbol to the end of the table*/
		temp2->next=temp1;
	}

}
/*this function recives the details that are needed to add a data word and adds the new data word to the data list*/
void add_data(unsigned short code, data_word **head_data, int address, char are)
{
	data_word *temp1;
	data_word *temp2;
	temp1=(data_word *)malloc(sizeof(data_word));
	if(!temp1)
	{
		fprintf(stdout,"allocation memory failed, program will shut down\n");
		exit(0);
	}
	temp1->next=NULL;
	temp1->code=code;
	temp1->DC=address;
	temp1->ARE=are;
	if((*head_data)==NULL)/*if the list is empty*/
		{
		
		(*head_data)=temp1;
		}
	else{/*the list is not empty*/
		for(temp2=(*head_data);temp2->next;temp2=temp2->next);
		temp2->next=temp1;
	     }
	

}

/*this function checks if given string is an integer, if the string contains characters which are not numbers the function return false otherwise the function return true*/
boolean isnumber(char number[])
{
	int i;
	if(number[0]=='-'||number[0]=='+'||isdigit(number[0]))/*checks if the first character is a digit or a sign*/
	{
		for(i=1;i<strlen(number)-1;i++)
			if(!isdigit(number[i]))
				return FALSE;
	}
	else
		return FALSE;
	return TRUE;
}
/*this function checks if a given string is a register if it is returns true otherwise return false*/
boolean isregister(char reg[])
{
	char *registers[]={"r0","r1","r2","r3","r4","r5","r6","r7"};
	int i;
	int registers_size=sizeof(registers)/sizeof(*registers);
	for(i=0;i<registers_size;i++)
		if(!strcmp(registers[i],reg))
			{
			return TRUE;
			}
	return FALSE;
}
/*this function checks if a given string is one of the 16 instructions if it is it returns and int which combining the opcode and the funct in a specific way (explanation inside the function) and if a command is not found it returns -1 */
int which_command(char *command)
{

char *commands[COMMANDS_NUMBER]={"mov","cmp","add","sub","lea","clr","not","inc","dec","jmp","bne","jsr","red","prn","rts","stop"};
int opcode_funct[COMMANDS_NUMBER]={000,100,210,211,400,510,511,512,513,910,911,912,1200,1300,1400,1500};/*combining the two representative numbers of both the opcode and the funct into a single number. in each number the first two digits from the right represent the funct of the command and the digits represent the opcode*/
int i=0;

while(i<COMMANDS_NUMBER)
{
	if(!strcmp(command,commands[i]))
	{	
		return opcode_funct[i];
	}
i++;

}
return NOT_EXIST;
}


/*checks if the label is legal, returns true if the name is legal and false otherwise*/
boolean symbol_valid(char s[],int line_number,symbol **head_symbol,char *name_of_file)
{
	int i;
	boolean legal=TRUE;
	symbol *temp;
	if(!isalpha(s[0]))/*The first char have to be a letter*/
	{
		fprintf(stdout,"<file %s,line %d> The first character must be a letter (lower or upper).\n",name_of_file,line_number);
		legal=FALSE;
	}
	for(i=1;i<strlen(s)-1;i++)
		{
		
		if(!isalnum(s[i]))/*the rest of the label can include letters and numbers*/
		{
			fprintf(stdout,"<file %s,line %d> The characters in the label can include number or letter only\n",name_of_file,line_number);
			legal=FALSE;
		}
		}
	for(temp=(*head_symbol);temp!=NULL;temp=temp->next)/*checking if there is label with the same name*/
		if(!strcmp(s,temp->name))
		{
			fprintf(stdout,"<file %s,line %d> The symbol is already exist\n",name_of_file,line_number);
			legal=FALSE;
		}
	if(!(which_command(s)==NOT_EXIST)||
		!strcmp(s,"data")||
		!strcmp(s,"string")||
		!strcmp(s,"entry")||
		!strcmp(s,"extern")||
		!strcmp(s,"define")||
		isregister(s))/*checks if the name of the label is a saved word of the language*/
	{
		fprintf(stderr,"<file %s,line %d> The name of the symbol is a saved word in the language\n",name_of_file,line_number);
		legal=FALSE;
	}
	return legal;
}


/*the function returns the complement to base 2 of given negative number*/
int base_comp(char value[])
{
	int val=atoi(value+1);
	val=~val;
	return val+1;
}
/*the function copies the first n characters from src string to dest string and adding \0 in the end*/
void substring(char dest[],char src[],int n)
{
	strncpy(dest,src,n);
	dest[n]='\0';
	
	return;
}

/*this function returns the first place in a given string that contain the character we look for, if the char doesn't exist in the string the function returns -1*/
int find_char(char str[],char c,int i)
{
	while(str[i]!=c && str[i]!='\0')
		i++;
	if(str[i]==c)
		return i;
	return NOT_EXIST;
}	
/*this functions checks if a given string exists in the symbol list if it is it return a pointer to this symbol*/
symbol *symbol_exist(char label[],symbol **head_symbol)
{
	symbol *temp;
	for(temp=(*head_symbol);temp;temp=temp->next)
		if(!strcmp(label,temp->name))
			return temp;
	return NULL;
}

/*this function checks if a given operand is illegal if the operand is illegal ut return true otherwise it initializes the word with this operand*/
boolean method_type(char operand[],int *IC,symbol **head_symbol,code_word **operand_word,int *op_method,FILE *ext,char *file_name,int line_num)
{
	int jump;
	char value[LINE_LEN];/*string that saves the label after the % character*/
	symbol *temp=NULL;
	if(operand[0]=='#')/*immediately method(method 0) must start with the character '#'*/
	{
		if(isnumber(operand+1))/*checks if it's a number*/
		{
			(*op_method)=IMMEDIATE;
			(*operand_word)=initialize_word(operand[1]=='-'?base_comp(operand+1):atoi(operand+1),'A',*IC);
			(*IC)++;
				
			return FALSE;
		}
		else/*not real number*/
			return TRUE;
	}
	if(isregister(operand))/*a register operand*/
	{
		(*op_method)=REGISTER;
		(*operand_word)=initialize_word(1<<atoi(operand+1),'A',*IC);/*intializes the word where the bit of the number of the register is on*/
		(*IC)++;
		return FALSE;
	}

	if(operand[0]=='%')/*checks if it's a relative method*/
	{
		strcpy(value,operand+1);/*copies the string after the % (which supposed to be a label)*/
		for(temp=(*head_symbol);temp;temp=temp->next)
		{
			if(!strcmp(value,temp->name))/*if the string is an exist label in the symbols list*/
			{
			(*op_method)=RELATIVE;
			jump=(temp->value)-(*IC);/*no need to decreas one beacuse the IC already been updated before*/
			(*operand_word)=initialize_word(jump,'A',*IC);/*initialize the word with the number that represent the jump*/
			(*IC)++;
			return FALSE;
			}
		}
			
		fprintf(stdout, "<file %s,line %d> label does'nt exist\n", file_name, line_num);
		return TRUE;
			
	}        
	  
	   for(temp=(*head_symbol);temp;temp=temp->next)/*checks if the operand is an exist symbol in the symbol list*/
		{
		if(!strcmp(operand,temp->name))
			{
			break;
			}
		}
	  if(temp&&!strcmp(temp->name,operand))
		{    
		/*the method is direct(method 1)*/
		(*op_method)=DIRECT;
		if(!strcmp(temp->attributes,"extern"))/*if the label is extern we write it in the extern file and initialize the word, otherwise we initialize the word only*/
		{
			(*operand_word)=initialize_word(temp->value,'E',*IC);
			fprintf(ext,"%s\t%04d\n",temp->name,*IC);
			(*IC)++;
			return FALSE;
		}
		else
			{
			(*operand_word)=initialize_word(temp->value,'R',*IC);
			(*IC)++;
			return FALSE;
			}
		return TRUE;
	        }
	   
(*op_method)=NOT_EXIST;/*the operand doesn't belong to one of the addressing methods*/
fprintf(stdout,"<file %s,line %d> Unknown operand cannot handle line\n",file_name,line_num);
return TRUE;
	   
	
}
/*this function adds a code word to the end of the code list*/
void add_list (code_word **head,code_word *node)
{
	code_word *temp;
	if(!(*head))
	{
		(*head)=node;
		return;
	}
	for(temp=(*head);temp->next;temp=temp->next);
	temp->next=node;
	return;
}
/*the function initialize a code word with given values and returns the pointer to that code word, the operands words*/
code_word *initialize_word(unsigned int address,char are, int ic)
{

	code_word * temp;
	temp=(code_word *)calloc(1,sizeof(code_word));
	if(!temp)
	{
		fprintf(stdout,"allocation memory failed, program will shut down\n");
		exit(0);
	}
	temp->code=0;
	temp->code|=address;
	temp->IC=ic;
	temp->ARE=are;
	temp->next=NULL;
	return temp;
}
/*the function create the first word(opcode,funct,op1,op2) and returns the pointer to that word*/
code_word *create_word(int opcode_funct, int src_method,int dest_method, char are, int ic)
{
	int opcode=opcode_funct/100;/*as been explained in which_method this number represents the opcode*/
	int funct=opcode_funct%100;/**as been explained in which_method this number represents the funct*/
	code_word *temp;
	temp=(code_word *)calloc(1,sizeof(code_word));
	if(!temp)
	{
		fprintf(stdout,"allocation memory failed, program will shut down\n");
		exit(0);
	}
	/*initialize the first word with given values*/
	temp->code=0;/*the unused bits*/
	temp->code<<=OPCODE_LEN;
	temp->code|=opcode;
	temp->code<<=FUNCT_LEN;
	temp->code|=funct;
	temp->code<<=METHOD_LEN;
	temp->code|=src_method;
	temp->code<<=METHOD_LEN;
	temp->code|=dest_method;
	
	temp->ARE=are;
	temp->IC=ic;
	temp->next=NULL;
	return temp;
}
/*this method checks the syntax of a given line regarding immediate and relative addresing methodes */
boolean syntax_method(char operand[],int line_num,char *file_name)
{
	
	if(operand[0]=='#')/*immediate method must start with the character '#' and must have a number after it*/
	{
		
		if(strlen(operand)>1)
		{
			if(!isnumber(operand+1))
			{
			fprintf(stdout,"<file %s, line %d> The operand after '#' is illegal must be a real number)\n",file_name,line_num);
			return TRUE;
			}
		}
		else
			{
			fprintf(stdout,"<file %s, line %d> There is no char after the '#' sign must be a real number\n",file_name,line_num);
			return TRUE;
			}
	}
	if(operand[0]=='%')
	{
		if(strlen(operand)>1)
		{
			if(operand[1]==' ')/*there is space after the % mark*/
			{
			fprintf(stdout,"<file %s, line %d> The is an illegal space after the precent sign\n",file_name,line_num);
			return TRUE;
			}
		}else
			{
			fprintf(stdout,"<file %s, line %d> There is no char after the precent sign\n",file_name,line_num);
			return TRUE;
			}
	}
	
	return FALSE;
}
