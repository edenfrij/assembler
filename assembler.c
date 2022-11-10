#include "assembler.h"
/*this function recives files in a command line and sends them to a function that translate the files to a machine language according to some roles
if the function cannot open the files in the command line it alerts*/
int main (int argc, char *argv[])
{
	int i;
	char name_of_file[MAX_NAME_LEN];/*a line to save the name of the file*/
	FILE *ptr;
	if(argc>1)/*2 arguments at least at the command line*/
	{
		for(i=1;i<argc;i++)/*opens the files in the command line one by one*/
		{
			strcpy(name_of_file,argv[i]);
			ptr= fopen(strcat(name_of_file,".as"),"r");
			if(!ptr)	
				fprintf(stdout,"Not able to open the file: %s, or this file does not exist\n",name_of_file);
			else/*able to open the files*/
			{
				fprintf(stdout,"Start assembling the file: %s\n",name_of_file);
				assembler(ptr,argv[i]);
				fclose(ptr);
			}
		}
	}
	else
		fprintf(stdout,"There are not enough arguments in the command line,\n The command line should be written in this pattern: ./assembler firstFile secondFile ...\n");
	return 0;
}
/*the main function that translate the files to a mechine laguage, the fuction reads the file in two transitions line by line from the file until EOF.
The function holds three linked list, one holds the code one holde the data and the last one hold the symbol list.
in each line that the function reads the function it checks if there were any errors in it, if there were error the function return false.
If there were any errors in the file the function will no produce ext and ent files*/
void assembler(FILE *file,char *file_name)
{	
	char line[LINE_LEN];/*logest line has 80 characters saving 2 places for \n and \0*/
	/*the output files name*/
	char file_as[MAX_NAME_LEN];
	char file_ext[MAX_NAME_LEN];
	char file_ent[MAX_NAME_LEN];
	char file_ob[MAX_NAME_LEN];
	/*counter for the number of code words(IC),number of data words (DC)*/
	int IC=FIRST_ADDRESS_IC;
	int ICF=FIRST_ADDRESS_IC;/*the last adress of the IC counter after the first transition*/
	int DC=FIRST_ADDRESS_DC;
	int line_number=1;/*line counter*/
	/*initialize the heads of the lists*/
	symbol *headlist_symbol=NULL;
	data_word *headlist_data=NULL;
	code_word *headlist_code=NULL;
	/*flags for errors */
	boolean test,has_error=FALSE;
	/*pointer to the 3 output files*/
	FILE *ent;
	FILE *ext;
	FILE *ob;
	/*prepares the files name*/
	strcpy(file_ent,file_name);
	strcpy(file_ext,file_name);
	strcpy(file_as,file_name);
	strcat(file_as,".as");
	strcat(file_ent,".ent");
	strcat(file_ext,".ext");
	/*Overwrite old files if exists*/
	ent=fopen(file_ent,"w");
	if(!ent)/*if the function couldn't open the file it means that there is some problem and we exit the program*/
	{
		fprintf(stdout,"can't open file %s\n",file_ent);
		exit(0);
	}
	ext=fopen(file_ext,"w");
	if(!ext)/*if the function couldn't open the file it means that there is some problem and we exit the program*/
	{
		fprintf(stdout,"can't open file %s\n",file_ext);
		exit(0);
	}
	

	while (fgets(line,LINE_LEN,file)!=NULL)/*reads line after line from the file and send them to the first transition*/
	{
	
		if(line[strlen(line)-1]=='\n')/*the line should be end in \n*/
			{/*recives true if there are any errors*/
			test=first_transition(line,&IC,&DC,&headlist_symbol,line_number,&headlist_data,file_as);/*first transition*/
			}
		else/*if the line doesn't end in \n*/
		{
			fprintf(stdout,"<file %s,line %d> The line is longer than 80 characters which is the maximum line length\n",file_as,line_number);
			has_error=TRUE;
			while(fgetc(file)!='\n');/*running through the end of the line*/
		}
		if(test)/*if there where ant errors discovered in the first transition*/
			has_error=TRUE;
		line_number++;
	}
	ICF=IC;/*saving the funal number of code word*/
	address_update(headlist_symbol,(ICF));/*update the data address*/
	/*back to the begining of the file and initialize the line number and counter*/
	rewind(file);
	line_number=1;
	IC=FIRST_ADDRESS_IC;
	while (fgets(line,LINE_LEN,file)!=NULL)/*second run on the file*/
	{
		
		if(line[strlen(line)-1]=='\n')/*the line should be end in \n*/
			
			{/*recives true if there are any errors*/
				test=second_transition(line,&IC,&DC,&headlist_symbol,line_number,&headlist_code,ext,ent,file_as);/*second transition*/
			}
		else	
		{/*if the line doesn't end in \n*/
			has_error=TRUE;
			while(fgetc(file)!='\n');/*running through the end of the line*/
		}
		if(test)/*if there where ant errors discovered in the first transition*/
			has_error=TRUE;
		line_number++;
	}

	/*if there was errors we will erase the file and write message to the user*/
	if(has_error)
	{
		fprintf(stdout,"%s: The file has errors,no files created\n",file_as);
		fclose(ent);
		fclose(ext);
		/*removing the files*/
		remove(file_ent);
		remove(file_ext);
	}
	else
	{
		fprintf(stdout,"%s: The file assembeled successfully\n",file_as);
		if(!ftell(ent))/*if the entry file is empty, we remove it*/
		{
			fclose(ent);
			remove(file_ent);
		}
		else
			fclose(ent);
		if(!ftell(ext))/*if the extern file is empty, we remove it*/
		{
			fclose(ext);
			remove(file_ext);
		}
		else
			fclose(ext);
		/*creaating the name of the ob file*/
		strcpy(file_ob,file_name);
		strcat(file_ob,".ob");
		ob=fopen(file_ob,"w");
		if(!ob)
		{
			fprintf(stdout,"can't open file %s\n",file_ext);
			exit(0);	
		}
		fprintf(ob,"   %d  %d\n",IC-FIRST_ADDRESS_IC,DC);
		if(IC)/*if there are any words to print*/
		{
		print_code_list(&headlist_code,ob);
		}
		if(DC)/*if there are any words to print*/
		{
		print_data_list(&headlist_data,ob,ICF);
		}
		fclose(ob);
	}
	/*if the code list hold words it frees the dynamic memory*/
	if(headlist_code)
		free_code_word(headlist_code);
	/*if the data list hold words it frees the dynamic memory*/
	if(headlist_data)
		free_data_word(headlist_data);
	/*if the symbol list hold words it frees the dynamic memory*/
	if(headlist_symbol)
		free_symbol(headlist_symbol);
	return;
}	


	
	


/*this function updates the addresses of the symbol which their attributes is 'data' to the adderes+ the final ICF */
void address_update(symbol *head_symbol,int ICF)
{
	symbol *temp;

	for(temp=head_symbol;temp;temp=temp->next)
		{
			if(!strcmp(temp->attributes,"data"))
			{
			(temp->value)+=ICF;/*adds the number of the last count of the word codes to the address*/
			}
		}
	return;
}

/*this function frees the dynamic memory that has been used to create the word*/

void free_code_word(code_word * head)
{
	code_word *temp;
	for(temp=head->next;temp;head=temp,temp=temp->next)
		free(head);
	free(head);
	return;		
}
/*this function frees the dynamic memory that has been used to create the word*/
void free_data_word(data_word *head)
{
	data_word *temp;
	for(temp=head->next;temp;head=temp,temp=temp->next)
		free(head);
	free(head);
	return;		
}


/*this function frees the dynamic memory that has been used to create the symbol*/
void free_symbol(symbol *head)
{
	symbol *temp;
	for(temp=head->next;temp;head=temp,temp=temp->next)
		free(head);
	free(head);
	return;		
}
/*this function prints the whole code list to the ob file*/
void print_code_list(code_word **head,FILE *ob)
{
code_word *temp;
for(temp=(*head);temp;temp=temp->next)
    {
	fprintf(ob,"%04d %03X %c\n",temp->IC, (temp->code)&0xFFF,temp->ARE);
    }
}
/*this function prints the whole data list to the ob file*/
void print_data_list(data_word **head,FILE *ob,int icf)
{
data_word *temp;
for(temp=(*head);temp;temp=temp->next)
    {
	fprintf(ob,"%04d %03X %c\n",icf++, (temp->code)&0xFFF,temp->ARE);
    }

}
