/*This fuction does the second transition on the file and code file to the mechine language*/
/*this function runs over the file for the second round.
the function codes the rest of the words that we did not code in the first transition (code words). 
The function analyzes the entry guidance and the insructions in the second transition. prints error if there are any.
The function analyzes the line char by char. the function returns true if there are any errors otherwise false*/

#include "assembler.h"

boolean second_transition(char line[],int *IC,int *DC,symbol **head_symbol,int line_num,code_word **head_code,FILE *ext,FILE *ent,char *file_name)
{
	int i=0,j,k;/*indexes*/
	int src_method,dest_method;/*holds the number that represents the addressing methods (one for source method and one for destination method*/
	char value[LINE_LEN];/*string that we use to copy the part that we work on*/
	char command[LINE_LEN];
	symbol *temp;
	/*pointrs to the code words, later we add them to the code list*/
	code_word *word=NULL;
	code_word *operand1=NULL;
	code_word *operand2=NULL;
	boolean error_exist=FALSE;/*flag if an error exist*/
	
	i=skip_whitespaces_begining(line,i);
	if(line[i]=='\n' || line[i]==';')/*if the line is empty ot the line is a command*/
		return error_exist;

	j=find_char(line,':',i);/*checks for label*/
	if(j!=NOT_EXIST)
		i=skip_whitespaces_begining(line,j+1);/*skips the label*/
	if(line[i]=='.')/*if it's a guidance*/
	{
		i++;/*skips the dot*/
		j=find_char(line,' ',i);
		if(j!=NOT_EXIST)/*a white space after the command*/
		{
			k=skip_whitespaces_end(line+i,j-i-1);/*end of guidance*/
			if(!strncmp(line+i,"entry",k))
			{
				i=skip_whitespaces_begining(line,j+1);
				j=find_char(line,'\n',i);/*look for the end of the line*/
				if(i==j)/*there id no label after the entry*/
				{
					fprintf(stdout,"<file %s,line %d> The label after the entry declaration is missing\n",file_name,line_num);
					return TRUE;
				}
				k=skip_whitespaces_end(line+i,j-i-1);
				substring(value,line+i,k);/*saving the operand that supossed to be a symbol*/
				if((temp=symbol_exist(value,head_symbol)))/*checks if the label in the entry parameter exist in the symbol list*/
				{
					fprintf(ent,"%s\t%04d\n",value,temp->value);
					return FALSE;
				}
				else
				{
					fprintf(stdout,"<file %s,line %d> The Label doesn't exist\n",file_name,line_num);
					return TRUE;
				}
				
			}
			else/*if we got here, it means that it is the guidance data, string or extern that we handle in the first transition*/
				return FALSE;
		}
		else/*missing parameter,message sent in the first transition*/
			return TRUE;
	}
	
	
        i=skip_whitespaces_begining(line,j+1);
	j=find_char(line,' ',i);
	if(j!=NOT_EXIST)/*if it is a case a command with operands*/
	{
	    substring(command,line+i,j-i);
	}
	else{/*if it is a non operand commands*/
		j=find_char(line,'\n',i);
		substring(command,line+i,j-i);
	     }
	if(which_command(command)!=NOT_EXIST)/*checking the command,if its one of th 16 commands we enter the case else weprint error*/
	{
		if(!strcmp(command,"mov"))/*if it's a move case*/
			{
			(*IC)++;
			j=find_char(line,' ',i);
			i=skip_whitespaces_begining(line,j+1);/*skipping the command*/
			j=find_char(line,',',i);
			if(j!=NOT_EXIST)/*found a comma*/
			{
				k=skip_whitespaces_end(line+i,j-i-1);
				substring(value,line+i,k);/*saves the first string operand*/
				error_exist=method_type(value,IC,&(*head_symbol),&operand1,&src_method,ext,file_name,line_num);/*checking operand1 method*/
				if(error_exist)/*if it is not a legal operand*/
				{	
					fprintf(stdout,"<file %s, line %d> Illegal parameter in this line\n",file_name,line_num);
					return error_exist;
				}
				if(src_method==RELATIVE)/*mov cannot recive relative as an addresing method*/
				{
					fprintf(stdout,"<file %s, line %d> Illegal method of addressing\n",file_name,line_num);
					error_exist=TRUE;
					return error_exist;
				}
				
				i=skip_whitespaces_begining(line,j+1);
				j=find_char(line,'\n',i);
				if(i!=j)
				{
					k=find_char(line,',',i);
					if(k==NOT_EXIST)
					{
						k=skip_whitespaces_end(line+i,j-i-1);
						substring(value,line+i,k);
						error_exist=method_type(value,IC,&(*head_symbol),&operand2,&dest_method,ext,file_name,line_num);/*checking operand method*/
						if(error_exist)/*if it is not a legal operand*/
						{	printf("%s",value);
							fprintf(stdout,"<file %s, line %d> Illegal parameter in this line\n",file_name,line_num);
							return error_exist;
						}

						if(dest_method==IMMEDIATE||dest_method==RELATIVE)/*mov cannot recive relative and immediate as an addresing method*/
						{
							fprintf(stdout,"<file %s, line %d> Illegal method of addressing\n",file_name,line_num);
							error_exist=TRUE;
							return error_exist;
						}
						else
						{/*creats the first word and adds them to the list*/
							word=create_word(which_command(command),src_method,dest_method,'A',(*IC)-3);
							add_list(&(*head_code),word);
							add_list(&(*head_code),operand1);
							if(operand2)
								add_list(&(*head_code),operand2);
							return FALSE;
						}
					}
					else/*too many operands or illegal comma, error mesage sent in the first transition*/
						return TRUE;
				}
				else/*missing operand, message sent in the first transition*/
					return TRUE;
			}
			else/*missing operand, error message sent in the first transition*/
				return TRUE;
		}
		if(!strcmp(command,"cmp"))/*if it's a cmp command*/
			{
			(*IC)++;/*coding the first word*/
			j=find_char(line,' ',i);
			i=skip_whitespaces_begining(line,j+1);/*skipping the command, checked in the which_command function*/
			j=find_char(line,',',i);
			if(j!=NOT_EXIST)
			{
				k=skip_whitespaces_end(line+i,j-i-1);
				substring(value,line+i,k);/*saves the first operand*/
				error_exist=method_type(value,IC,&(*head_symbol),&operand1,&src_method,ext,file_name,line_num);/*checks op1 method*/
				if(error_exist)/*if the addressing method of the operator is illegal*/
				{
					fprintf(stdout,"<file %s, line %d> Illegal parameter in this line\n",file_name,line_num);
					return error_exist;
				}
				if(src_method==RELATIVE)/*add can't have relative method in destination operand*/
						{
							fprintf(stdout,"<file %s, line%d> Illegal method of addressing\n",file_name,line_num);
							error_exist=TRUE;
							return error_exist;
						}
				i=skip_whitespaces_begining(line,j+1);
				j=find_char(line,'\n',i);/*searching for the end of the line*/
				if(i!=j)
				{
					k=find_char(line,',',i);
					if(k==NOT_EXIST)
					{
						k=skip_whitespaces_end(line+i,j-i-1);
						substring(value,line+i,k);/*saves the operand string*/
						error_exist=method_type(value,IC,&(*head_symbol),&operand2,&dest_method,ext,file_name,line_num);
						/*if the addressing method of the operator is illegal*/
						if(error_exist)
						{
							fprintf(stdout,"<file %s, line %d> Illegal parameter in this line\n",file_name,line_num);
							return error_exist;
						}
					if(dest_method==RELATIVE)/*add can't have relative method in destination operand*/
						{
							fprintf(stdout,"<file %s, line%d> Illegal method of addressing\n",file_name,line_num);
							error_exist=TRUE;
							return error_exist;
						}
						/*creat the first word and adds them to the list*/
						word=create_word(which_command(command),src_method,dest_method,'A',*IC-3);
						add_list(&(*head_code),word);
						add_list(&(*head_code),operand1);
						if(operand2)
							add_list(&(*head_code),operand2);
						return FALSE;
					}
					else/*too many operands or illegal comma, error mesage sent in the first transition*/
						return TRUE;
				}
				else/*missing operand, message sent in the first transition*/
					return TRUE;
			}
			else/*missing operand, error message sent in the first transition*/
				return TRUE;
			}
		if(!strcmp(command,"add"))
			{
			(*IC)++;/*coding the first word*/
			j=find_char(line,' ',i);
			i=skip_whitespaces_begining(line,j+1);/*skipping the command, checked in the which_command function*/
			j=find_char(line,',',i);
			if(j!=NOT_EXIST)
			{
				k=skip_whitespaces_end(line+i,j-i-1);
				substring(value,line+i,k);/*saves the operand*/
				error_exist=method_type(value,IC,&(*head_symbol),&operand1,&src_method,ext,file_name,line_num);/*checking op1 method*/
				if(error_exist)
				{/*if the addressing method of the operator is illegal*/
					fprintf(stdout,"<file %s, line %d> Illegal parameter in this line\n",file_name,line_num);
					return error_exist;
				}
				if(src_method==RELATIVE)/*add can't have relative method in source operand*/
						{
							fprintf(stdout,"<file %s, line%d> Illegal method of addressing\n",file_name,line_num);
							error_exist=TRUE;
							return error_exist;
						}
				i=skip_whitespaces_begining(line,j+1);
				j=find_char(line,'\n',i);
				if(i!=j)
				{
					k=find_char(line,',',i);
					if(k==NOT_EXIST)
					{
						k=skip_whitespaces_end(line+i,j-i-1);
						substring(value,line+i,k);
						error_exist=method_type(value,IC,&(*head_symbol),&operand2,&dest_method,ext,file_name,line_num);
						if(error_exist)
						{
							fprintf(stdout,"<file %s, line %d> Illegal parameter in this line\n",file_name,line_num);
							return error_exist;
						}
						if(dest_method==IMMEDIATE||dest_method==RELATIVE)/*add can't have immediate or relative method in destination operand*/
						{
							fprintf(stdout,"<file %s, line%d> Illegal method of addressing\n",file_name,line_num);
							error_exist=TRUE;
							return error_exist;
						}
						else
						{/*creats the first word and adds them to the list*/
							word=create_word(which_command(command),src_method,dest_method,'A',*IC-3);
							add_list(&(*head_code),word);
							add_list(&(*head_code),operand1);
							if(operand2)
								add_list(&(*head_code),operand2);
							return FALSE;
						}
					}
					else/*too many operands or illegal comma, error mesage sent in the first transition*/
						return TRUE;
				}
				else/*missing operand, message sent in the first transition*/
					return TRUE;
			}
			else/*missing operand, error message sent in the first transition*/
				return TRUE;
			}
	if(!strcmp(command,"sub"))
			{
			(*IC)++;/*coding the first word*/
			j=find_char(line,' ',i);
			i=skip_whitespaces_begining(line,j+1);/*skipping the command*/
			j=find_char(line,',',i);
			if(j!=NOT_EXIST)
			{
				k=skip_whitespaces_end(line+i,j-i-1);
				substring(value,line+i,k);
				error_exist=method_type(value,IC,&(*head_symbol),&operand1,&src_method,ext,file_name,line_num);/*checking op1 method*/
				if(error_exist)
				{/*if the operand is illegal*/
					fprintf(stdout,"<file %s, line %d> Illegal parameter in this line\n",file_name,line_num);
					return error_exist;
				}
				if(src_method==RELATIVE)/*sub can't have relative method in source operand*/
						{
							fprintf(stdout,"<file %s,line %d> Illegal method of addressing\n",file_name,line_num);
							error_exist=TRUE;
						}
				
				i=skip_whitespaces_begining(line,j+1);
				j=find_char(line,'\n',i);
				if(i!=j)
				{
					k=find_char(line,',',i);
					if(k==NOT_EXIST)
					{
						k=skip_whitespaces_end(line+i,j-i-1);
						substring(value,line+i,k);
						error_exist=method_type(value,IC,&(*head_symbol),&operand2,&dest_method,ext,file_name,line_num);
						if(error_exist)
						{/*if the operand is illegal*/
							fprintf(stdout,"<file %s, line %d> Illegal parameter in this line\n",file_name,line_num);
							return error_exist;
						}
						if(dest_method==IMMEDIATE||dest_method==RELATIVE)/*sub can't have immediate or relative method in destination operand*/
						{
							fprintf(stdout,"<file %s,line %d> Illegal method of addressing\n",file_name,line_num);
							error_exist=TRUE;
							return error_exist;
						}
						else
						{/*creates the first word and adds them to the list*/
							word=create_word(which_command(command),src_method,dest_method,'A',*IC-3);
							add_list(&(*head_code),word);
							add_list(&(*head_code),operand1);
							if(operand2)
								add_list(&(*head_code),operand2);
							return FALSE;
						}
					}
					else/*too many operands or illegal comma, error mesage sent in the first transition*/
						return TRUE;
				}
				else/*missing operand, message sent in the first transition*/
					return TRUE;
			}
			else/*missing operand, error message sent in the first transition*/
				return TRUE;
			}
		if(!strcmp(command,"not"))
			{
			(*IC)++;/*coding the first word*/
			j=find_char(line,' ',i);
			i=skip_whitespaces_begining(line,j+1);/*skipping the command, checked in the which_command function*/
			j=find_char(line,'\n',i);
			if(i!=j)
			{
				k=find_char(line,',',i);
				if(k==NOT_EXIST)
				{
					k=skip_whitespaces_end(line+i,j-i-1);
					substring(value,line+i,k);
					error_exist=method_type(value,IC,&(*head_symbol),&operand2,&dest_method,ext,file_name,line_num);/*checking op2 method*/
					if(error_exist)
					{/*if the addersing method of the operand is illegal*/
						fprintf(stdout,"<file %s, line %d> Illegal parameter in this line\n",file_name,line_num);
						return error_exist;
					}
					if(dest_method==IMMEDIATE||dest_method==RELATIVE)/*not can't have immediate or relative method in source operand*/
					{
						fprintf(stdout,"<file %s, line %d> Illegal method of addressing\n",file_name,line_num);
						error_exist=TRUE;
						return error_exist;
					}
					else
					{/*creates the first word and adds them to the list*/
						word=create_word(which_command(command),0,dest_method,'A',*IC-2);
						add_list(&(*head_code),word);
						add_list(&(*head_code),operand2);
						return FALSE;
					}
				}
				else/*too many operands or illegal comma, error mesage sent in the first transition*/
					return TRUE;
			}
			else/*missing operand, message sent in the first transition*/
				return TRUE;
			}
		if(!strcmp(command,"clr"))
			{
			(*IC)++;/*coding the first word*/
			j=find_char(line,' ',i);
			i=skip_whitespaces_begining(line,j+1);/*skipping the command, checked in the which_command function*/
			j=find_char(line,'\n',i);
			if(i!=j)
			{
				k=find_char(line,',',i);
				if(k==NOT_EXIST)
				{
					k=skip_whitespaces_end(line+i,j-i-1);
					substring(value,line+i,k);
					error_exist=method_type(value,IC,&(*head_symbol),&operand2,&dest_method,ext,file_name,line_num);/*checking second operand*/
					if(error_exist)
						{/*if the operand is illegal*/
							fprintf(stdout,"<file %s, line %d> Illegal parameter in this line\n",file_name,line_num);
							return error_exist;
						}
					if(dest_method==IMMEDIATE||dest_method==RELATIVE)/*clr can't have immediate or relative method in destination operand*/
					{
						fprintf(stdout, "<file %s, line %d> Illegal method of addressing\n",file_name,line_num);
						error_exist=TRUE;
						return error_exist;
					}
					else
					{/*creates the last word and add them to the list*/
						word=create_word(which_command(command),0,dest_method,'A',*IC-2);
						add_list(&(*head_code),word);
						add_list(&(*head_code),operand2);
						return FALSE;
					}
				}
				else/*too many operands or illegal comma, error mesage sent in the first transition*/
					return TRUE;
			}
			else/*missing operand, message sent in the first transition*/
				return TRUE;
			}
		if(!strcmp(command,"lea"))
			{
			(*IC)++;/*coding the first word*/
			j=find_char(line,' ',i);
			i=skip_whitespaces_begining(line,j+1);/*skipping the command*/
			j=find_char(line,',',i);
			if(j!=NOT_EXIST)
			{
				k=skip_whitespaces_end(line+i,j-i-1);
				substring(value,line+i,k);/*saves the first operand*/
				error_exist=method_type(value,IC,&(*head_symbol),&operand1,&src_method,ext,file_name,line_num);/*checking first operand*/
				if(error_exist)
				{/*if the operand id illegal*/
					fprintf(stdout,"<file %s, line %d> Illegal parameter in this line\n",file_name,line_num);
					return error_exist;
				}
				if(src_method==IMMEDIATE||src_method==REGISTER||src_method==RELATIVE)/*lea can't have immediate or register or relative method in source operand*/
				{
					fprintf(stdout,"<file %s, line %d> Illegal method of addressing\n",file_name,line_num);
					error_exist=TRUE;
					return error_exist;
				}
				i=skip_whitespaces_begining(line,j+1);
				j=find_char(line,'\n',i);
				if(i!=j)
				{
					k=find_char(line,',',i);
					if(k==NOT_EXIST)
					{
						k=skip_whitespaces_end(line+i,j-i-1);
						substring(value,line+i,k);/*saves the second operand*/
						error_exist=method_type(value,IC,&(*head_symbol),&operand2,&dest_method,ext,file_name,line_num);
						if(error_exist)
						{
							fprintf(stdout,"<file %s, line %d> Illegal parameter in this line\n",file_name,line_num);
							return error_exist;
						}
						if(dest_method==IMMEDIATE||dest_method==RELATIVE)/*lea can't have immediate or relative method in destination operand*/
						{
							fprintf(stdout,"<file %s, line %d> Illegal method of addressing\n",file_name,line_num);
							error_exist=TRUE;
							return error_exist;
						}
						else
						{/*creates the first word and adds them to the list*/
							word=create_word(which_command(command),src_method,dest_method,'A',*IC-3);
							add_list(&(*head_code),word);
							add_list(&(*head_code),operand1);
							if(operand2)
								add_list(&(*head_code),operand2);
							return FALSE;
						}
					}
					else/*too many operands or illegal comma, error mesage sent in the first transition*/
						return TRUE;
				}
				else/*missing operand, message sent in the first transition*/
					return TRUE;
			}
			else/*missing operand, error message sent in the first transition*/
				return TRUE;
			}
		if(!strcmp(command,"inc"))
			{
			(*IC)++;/*coding the first word*/
			j=find_char(line,' ',i);
			i=skip_whitespaces_begining(line,j+1);/*skipping the command*/
			j=find_char(line,'\n',i);
			if(i!=j)
			{
				k=find_char(line,',',i);
				if(k==NOT_EXIST)
				{
					k=skip_whitespaces_end(line+i,j-i-1);
					substring(value,line+i,k);/*saves the first operand*/
					error_exist=method_type(value,IC,&(*head_symbol),&operand2,&dest_method,ext,file_name,line_num);/*checking for the first
 operand*/				if(error_exist)/*if the operand is illegal*/
					{	fprintf(stdout,"<file %s, line %d> Illegal parameter in this line\n",file_name,line_num);
						return error_exist;
					}
					if(dest_method==IMMEDIATE||dest_method==RELATIVE)/*inc can't have immediate or relative method in destination operand*/
					{
						fprintf(stdout,"<file %s, line %d> Illegal method of addressing\n",file_name,line_num);
						error_exist=TRUE;
						return error_exist;
					}
					else
					{/*create the first word and adds them to the list*/
						word=create_word(which_command(command),0,dest_method,'A',*IC-2);
						add_list(&(*head_code),word);
						add_list(&(*head_code),operand2);
						return FALSE;
					}
				}
				else/*too many operands or illegal comma, error mesage sent in the first transition*/
					return TRUE;
			}
			else/*missing operand, message sent in the first transition*/
				return TRUE;
			}
		if(!strcmp(command,"dec"))
			{
			(*IC)++;/*coding the first word*/
			j=find_char(line,' ',i);
			i=skip_whitespaces_begining(line,j+1);/*skipping the command, checked in the which_command function*/
			j=find_char(line,'\n',i);
			if(i!=j)
			{
				k=find_char(line,',',i);
				if(k==NOT_EXIST)
				{
					k=skip_whitespaces_end(line+i,j-i-1);
					substring(value,line+i,k);/*saves the second operand*/
					error_exist=method_type(value,IC,&(*head_symbol),&operand2,&dest_method,ext,file_name,line_num);/*checks second operand*/				if(error_exist)
						{/*if the operand is illegal*/
							fprintf(stdout,"<file %s, line %d> Illegal parameter in this line\n",file_name,line_num);
							return error_exist;
						}

					if(dest_method==IMMEDIATE||dest_method==RELATIVE)/*dec can't have immediate or relative method in destination operand*/
					{
						fprintf(stdout,"<file %s, line %d> Illegal method of addressing\n",file_name,line_num);
						error_exist=TRUE;
						return error_exist;
					}
					else
					{/*create the first word and add them to the list*/
						word=create_word(which_command(command),0,dest_method,'A',*IC-2);
						add_list(&(*head_code),word);
						add_list(&(*head_code),operand2);
						return FALSE;
					}
				}
				else/*too many operands or illegal comma, error mesage sent in the first transition*/
					return TRUE;
			}
			else/*missing operand, message sent in the first transition*/
				return TRUE;
		}
		if(!strcmp(command,"jmp"))
			{
			(*IC)++;/*coding the first word*/
			j=find_char(line,' ',i);
			i=skip_whitespaces_begining(line,j+1);/*skipping the command, checked in the which_command function*/
			j=find_char(line,'\n',i);
			if(i!=j)
			{
				k=find_char(line,',',i);
				if(k==NOT_EXIST)
				{
					k=skip_whitespaces_end(line+i,j-i-1);
					substring(value,line+i,k);/*saves the second operand*/
					error_exist=method_type(value,IC,&(*head_symbol),&operand2,&dest_method,ext,file_name,line_num);/*checks second operand*/
					if(error_exist)
						{/*if the operand is illegal*/
						fprintf(stdout,"<file %s, line %d> Illegal parameter in this line\n",file_name,line_num);
						return error_exist;
						}
					
					if(dest_method==IMMEDIATE||dest_method==REGISTER)/*jmp can't have immediate or register method in destination operand*/
					{
						fprintf(stdout,"<file %s, line %d> Illegal method of addressing\n",file_name,line_num);
						error_exist=TRUE;
						return error_exist;
					}
					else
					{/*creates the first word and adds them to the list*/
						word=create_word(which_command(command),0,dest_method,'A',*IC-2);
						add_list(&(*head_code),word);
						add_list(&(*head_code),operand2);
						return FALSE;
					}
				}
				else/*too many operands or illegal comma, error mesage sent in the first transition*/
					return TRUE;
			}
			else/*missing operand, message sent in the first transition*/
				return TRUE;
			}
		if(!strcmp(command,"bne"))
			{
			(*IC)++;/*coding the first word*/
			j=find_char(line,' ',i);
			i=skip_whitespaces_begining(line,j+1);/*skipping the command*/
			j=find_char(line,'\n',i);
			if(i!=j)
			{
				k=find_char(line,',',i);
				if(k==NOT_EXIST)
				{
					k=skip_whitespaces_end(line+i,j-i-1);
					substring(value,line+i,k);
					error_exist=method_type(value,IC,&(*head_symbol),&operand2,&dest_method,ext,file_name,line_num);/*checks second operand*/
					if(error_exist)
					{/*if the operand is illegal*/
						fprintf(stdout,"<file %s, line %d> Illegal parameter in this line\n",file_name,line_num);
						return error_exist;
					}
					if(dest_method==IMMEDIATE||dest_method==REGISTER)/*bne can't have immediate or register method in destination operand*/
					{
						fprintf(stdout,"<file %s, line %d> Illegal method of addressing\n",file_name,line_num);
						error_exist=TRUE;
						return error_exist;
					}
					else
					{/*creates the first word and adds them to the list*/
						word=create_word(which_command(command),0,dest_method,'A',*IC-2);
						add_list(&(*head_code),word);
						add_list(&(*head_code),operand2);
						return FALSE;
					}
				}
				else/*too many operands or illegal comma, error mesage sent in the first transition*/
					return TRUE;
			}
			else/*missing operand, message sent in the first transition*/
				return TRUE;
			}
		if(!strcmp(command,"red"))
			{
			(*IC)++;/*coding the first word*/
			j=find_char(line,' ',i);
			i=skip_whitespaces_begining(line,j+1);/*skipping the command*/
			j=find_char(line,'\n',i);
			if(i!=j)
			{
				k=find_char(line,',',i);
				if(k==NOT_EXIST)
				{
					k=skip_whitespaces_end(line+i,j-i-1);
					substring(value,line+i,k);
					error_exist=method_type(value,IC,&(*head_symbol),&operand2,&dest_method,ext,file_name,line_num);/*checking second operand*/
					if(error_exist)/*if the parameter is illegal*/
						{
							fprintf(stdout,"<file %s, line %d> Illegal parameter in this line\n",file_name,line_num);
							return error_exist;
						}

					if(dest_method==IMMEDIATE||dest_method==RELATIVE)/*red can't have immediate or relative method in destination operand*/
					{
						fprintf(stdout,"<file%s, line%d> Illegal method of addressing\n",file_name,line_num);
						error_exist=TRUE;
						return error_exist;
					}
					else
					{/*creates the first word and adds them to the list*/
						word=create_word(which_command(command),0,dest_method,'A',*IC-2);
						add_list(&(*head_code),word);
						add_list(&(*head_code),operand2);
						return FALSE;
					}
				}
				else/*too many operands or illegal comma, error mesage sent in the first transition*/
					return TRUE;
			}
			else/*missing operand, message sent in the first transition*/
				return TRUE;
			}
		if(!strcmp(command,"prn"))
			{
			(*IC)++;/*coding the first word*/
			j=find_char(line,' ',i);
			i=skip_whitespaces_begining(line,j+1);/*skipping the command*/
			j=find_char(line,'\n',i);
			if(i!=j)
			{
				k=find_char(line,',',i);
				if(k==NOT_EXIST)
				{
					k=skip_whitespaces_end(line+i,j-i-1);
					substring(value,line+i,k);/*saves the second operand*/
					error_exist=method_type(value,IC,&(*head_symbol),&operand2,&dest_method,ext,file_name,line_num);/*checks second operand*/
					if(error_exist)
					{/*if the parameter is illegal*/
						fprintf(stdout,"<file %s, line %d> Illegal parameter in this line\n",file_name,line_num);
						return error_exist;
					}
					if(dest_method==RELATIVE)/*prn can't have relative as method addresing*/
					{
						fprintf(stdout, "<file %s, line%d> Illegal method of addressing\n",file_name,line_num);
						error_exist=TRUE;
						return error_exist;
					}/*creates the first word and adds them to the list*/
					word=create_word(which_command(command),0,dest_method,'A',*IC-2);
					add_list(&(*head_code),word);
					add_list(&(*head_code),operand2);
					return FALSE;
				}
				else/*too many operands or illegal comma, error mesage sent in the first transition*/
					return TRUE;
			}
			else/*missing operand, message sent in the first transition*/
				return TRUE;
			}
		if(!strcmp(command,"jsr"))
			{
			(*IC)++;/*coding the first word*/
			j=find_char(line,' ',i);
			i=skip_whitespaces_begining(line,j+1);/*skipping the command*/
			j=find_char(line,'\n',i);
			if(i!=j)
			{
				k=find_char(line,',',i);
				if(k==NOT_EXIST)
				{
					k=skip_whitespaces_end(line+i,j-i-1);
					substring(value,line+i,k);
					error_exist=method_type(value,IC,&(*head_symbol),&operand2,&dest_method,ext,file_name,line_num);/*checking second operand*/
					if(error_exist)
						{/*if the operand is illegal*/
							fprintf(stdout,"<file %s, line %d> Illegal parameter in this line\n",file_name,line_num);
							return error_exist;
						}

					if(dest_method==IMMEDIATE||dest_method==REGISTER)/*jsr can't have immediate or a register method in destination operand*/
					{
						fprintf(stdout, "<file %s, line%d> Illegal method of addressing\n",file_name,line_num);
						error_exist=TRUE;
						return error_exist;
					}
					else
					{/*create the word and adds them to the list*/
						word=create_word(which_command(command),0,dest_method,'A',*IC-2);
						add_list(&(*head_code),word);/*adding the word to the list*/
						add_list(&(*head_code),operand2);/*adding the word to the list*/
						return FALSE;
					}
				}
				else/*too many operands or illegal comma, error mesage sent in the first transition*/
					return TRUE;
			}
			else/*missing operand, message sent in the first transition*/
				return TRUE;
			}
		if(!strcmp(command,"rts"))
			{
			(*IC)++;/*coding the first word*/
			j=find_char(line,'\n',i);
			k=find_char(line,' ',i);
			if(k==NOT_EXIST||j==skip_whitespaces_begining(line,k+1))/*if there are no operands create the word and add it to the list*/
			{
				word=create_word(which_command(command),0,0,'A',*IC-1);
				add_list(&(*head_code),word);/*adding the word to the list*/
				return FALSE;
			}
			else/*too many operands or illegal comma, error mesage sent in the first transition*/
				return TRUE;
			}
		if(!strcmp(command,"stop"))
			{
			(*IC)++;/*coding the first word*/
			j=find_char(line,'\n',i);
			k=find_char(line,' ',i);
			if(k==NOT_EXIST||j==skip_whitespaces_begining(line,k+1))
			{
				word=create_word(which_command(command),0,0,'A',*IC-1);/*if there are no operands create the word and add it to the list*/
				add_list(&(*head_code),word);/*adding the word to the list*/
				return FALSE;
			}
			else/*too many operands or illegal comma, error mesage sent in the first transition*/
				return TRUE;
			}
	}
	return error_exist;

}


