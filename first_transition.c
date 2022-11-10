#include "assembler.h"
/*this function passes over the file for the first round. the function receives a line after a line from the assembler.c and analyzes it.
If there are errors in the file the function print the errors and return TRUE(an error exist).
The function analyzes the given line char by char.
If there aren't any mistakes that have been discovered it return FALSE (an error not exist)*/
boolean first_transition(char line[],int *IC,int *DC,symbol **head_symbol,int line_num,data_word **head_data,char *file_name)
{
	/*create strings that will parts from the line that we want to analyze*/
	char label[MAX_LABEL];
	char value[LINE_LEN];
	char command[LINE_LEN];
	/*used indexes in the function*/
	int i=0,j,k,ind;
	/*flags for label and error*/
	boolean label_exist=FALSE;
	boolean error_exist=FALSE;
	i=skip_whitespaces_begining(line,i);
	if(line[i]=='\n' || line[i]==';')/*if the line is an empty line or the line is a comment*/
		{
		return error_exist;
		}

	j=find_char(line,':',i);/*looking for a label*/
	
	if(j!=NOT_EXIST)
	{
		k=j-i;

		if(k<=MAX_LABEL-1)/*checks that the length of the label is 31 charcters maximun (not include '\0')*/
		{
			if(j>0)
			{
			     if(line[j-1]==' ')
				{
				fprintf(stdout,"<file %s,line %d> .illegal label declartion There is a whitespace between ':' and the label.\n",file_name,line_num);
				   error_exist=TRUE;
				   return error_exist;
				}
			}
			if(j<(strlen(line)-1))
			{
				if(line[j+1]!=' ')
				 {
					fprintf(stdout,"<file %s,line %d>. There must be a white space after the ':', illegal sentence.\n",file_name,line_num); 
					   error_exist=TRUE;
				 	  return error_exist;
				 }
			}
			substring(label,line+i,k);/*saving the name of the label*/
			
			if(symbol_valid(label,line_num,&(*head_symbol),file_name))/*checks if the label is legal*/
				{
				label_exist=TRUE;
				}
			else
				{
				error_exist=TRUE;
				return error_exist;
				}
		}/*(k<=MAX_LABEL-1)*/
		else/*the label name is longer than maximum length*/
		{
			
			fprintf(stdout,"<file %s,line %d> The label's name is longer than 31 characters which is the maximum length\n",file_name,line_num);
			error_exist=TRUE;
			return error_exist;
		}
	
       }/*j!=NOT_EXIST*/
	i=skip_whitespaces_begining(line,j+1);

	if(line[i]=='\n')/*in this case we are sure that the line is not empty so if the next char is '\n' it is relevant in a case that the label exist*/
	{
		fprintf(stdout,"<file %s, line %d> There must be an instruction or a guidance after the label\n",file_name,line_num);
		error_exist=TRUE;
		return error_exist;
	}
		if(line[i]=='.')/*checks if it is a guidance*/
	        {
		i++;/*skips the dot*/
		j=find_char(line,' ',i);
		if(j!=NOT_EXIST)
			{
			k=skip_whitespaces_end(line+i,j-i-1);
			if(!strncmp(line+i,"entry",k))
			{
			if(label_exist)
				{
					fprintf(stdout,"<file %s,line %d> Warning: the label declared before the '.entry' will be ignored.\n",file_name,line_num);
				}
				return error_exist;/*we will handle the rest in the second transition*/
			}/* end entry case*/
			if(!strncmp(line+i,"extern",k))/*a case of an extern*/
			{
			
				if(label_exist)
					fprintf(stdout,"<file %s,line %d> Warning: the label declared before the '.extern' will be ignored.\n",file_name,line_num);

				i=skip_whitespaces_begining(line,j+1);/*the next char after the guidance which is not a whitespace*/
				j=find_char(line,'\n',i);/*last char of the sentence*/
				k=skip_whitespaces_end(line+i,j-i-1);
				substring(value,line+i,k);
				if(symbol_valid(value,line_num,&(*head_symbol),file_name))/*checking if the label is legal and save it in the symbol table*/
					add_symbol(value,0,"extern",&(*head_symbol));
					
				else
				        error_exist=TRUE;

				return error_exist;
			}/*end extern case*/
			if(!strncmp(line+i,"data",k))/*a case of a data*/
			{
				
				if(label_exist)/*if there is label in the line we save it in the symbol list*/
					{
					add_symbol(label,*DC,"data",&(*head_symbol));
					
					}
				i=skip_whitespaces_begining(line,j+1);/*the first char at the line after the guidance which is not a whitespace*/
				j=find_char(line,',',i);
				while(j!=NOT_EXIST)/*checks all of the values if they are legal we save their values in the data list*/
				{ 
					
					if(j!=i)
					{
						k=skip_whitespaces_end(line+i,j-i-1);
						substring(value,line+i,k);/*we save the next value*/
						if(isnumber(value))
							{
							add_data(value[0]=='-'?base_comp(value):atoi(value),&(*head_data),*DC,'A');
							(*DC)++;
							}
						else/*if it's not a number*/
						{
							fprintf(stdout,"<file %s, line %d>There is an illegal parameter at this line need to be numbers 							only\n",file_name,line_num);
							error_exist=TRUE;
							return error_exist;
						}
					}
					else/*if i==j*/
					{
						fprintf(stdout,"<file %s, line %d> There is an illegal comma at this line\n",file_name,line_num);
						error_exist=TRUE;
						return error_exist;
					}
					i=skip_whitespaces_begining(line,j+1);
					j=find_char(line,',',i);
					}/*j!=NOT_EXIST-line 112*/
					j=find_char(line,'\n',i);
				  
					if(j!=i)/*checks the last data parameter*/
				
				
					{       
					
						k=skip_whitespaces_end(line+i,j-i);
						substring(value,line+i,k);/*saves the last data parameter*/
						if(isnumber(value))
						{
						add_data(value[0]=='-'?base_comp(value):atoi(value),&(*head_data),*DC,'A');
						(*DC)++;
		
						}
					else
						{/*if it's not a number*/
						fprintf(stdout,"<file %s,line %d> illegal parameter at this line need to be numbers only \n",file_name,line_num);
							error_exist=TRUE;
							return error_exist;
						}
				
					}/*end j!=i*/
					else/*j==i*/
				        {
					fprintf(stdout,"<file %s,line %d> There is an illegal comma in the end of the data.\n",file_name,line_num);
						error_exist=TRUE;
						return error_exist;
			  	        }
			return error_exist;

			}/*end of data case*/
			if(!strncmp(line+i,"string",k))/*in a case of string guidance*/
			{
				if(label_exist)/*saving label if there is any*/
					add_symbol(label,*DC,"data",&(*head_symbol));
					

				i=skip_whitespaces_begining(line,j+1);/*The first index after the guidance which is not a whitespace*/
				j=find_char(line,'\n',i);
				k=skip_whitespaces_end(line+i,j-i);/*The last char at the line which is not a whitespace or \n*/
				if(line[i]=='"'&&line[i+k-1]=='"')/*looking for the quatation marks at the begining and the end of the string*/
				{
					substring(value,line+i+1,k-2);/*saving the value inside the string*/
					for(ind=1;ind<strlen(value)-1;ind++)
					{
						if(value[ind]=='\"')/*if there are more quatations marks inside the string*/
						{
						fprintf(stdout,"<file %s,line %d>, the paramter must be one string only\n",file_name,line_num);
						error_exist=TRUE;
						return error_exist;
						}
						
					}/*end of for loop*/
					for(ind=0;ind<=strlen(value);ind++)/*adds the characters at the string to the data list */
						{
						add_data(value[ind],&(*head_data),*DC,'A');
						(*DC)++;
						}
				}/*end of if quatation marks*/
				else/*there are no quatations marks at the begining or at the end or both*/
				{
					fprintf(stdout,"<file %s,line %d> The string must begin and end in quatation marks\n",file_name,line_num);
					error_exist=TRUE;
				}
				return error_exist;
			}/*end of string case*/
			else/*unknown guidance after the point*/
			{
				fprintf(stdout,"<file %s,line %d>, Unknown guidance\n",file_name,line_num);
				error_exist=TRUE;
				return error_exist;
			}	
		}/*j!=NOT_EXIST-line 77*/
		else
		{/*nothing exist after the guidance*/
		fprintf(stdout,"<file %s,line %d> illegal guidance, nothing exist after the guidance\n",file_name,line_num);
		error_exist=TRUE;
		return error_exist;
		}
	}/*if it's not a guidance-line 73*/
	i=skip_whitespaces_begining(line,j+1);
	j=find_char(line,' ',i);
	if(j!=NOT_EXIST)
	{
	    substring(command,line+i,j-i);
	}
	else{/*in a case that it's a non-parameters command*/
		j=find_char(line,'\n',i);
		substring(command,line+i,j-i);
	     }
	if(which_command(command)!=NOT_EXIST)/*checks if it's a command*/
	{	/*two parameters commands*/
		if(!strcmp(command,"mov")||!strcmp(command,"cmp")|| !strcmp(command,"add")||!strcmp(command,"sub")||!strcmp(command,"lea"))
		  {
			
			i=skip_whitespaces_begining(line,j+1);/*the first char after the command which is not a whitespace*/
			j=find_char(line,',',i);
			if(j!=NOT_EXIST)/*if a comma found*/
			{
				if(j==i)/*if there is a comma right after the command*/
				{
					fprintf(stdout,"<file %s, line %d> illegal comma at this line.\n",file_name,line_num);
					error_exist=FALSE;
					return error_exist;
				}
				k=skip_whitespaces_end(line+i,j-i-1);/*The first char before the comma which is not a whitespace*/
				substring(value,line+i,k);
				error_exist=syntax_method(value,line_num,file_name);/*first operand syntax*/
				if(error_exist)/*if there is an syntax problem*/
					return error_exist;
				i=skip_whitespaces_begining(line,j+1);
				j=find_char(line,'\n',i);
				if(i!=j)/*if the next char is not the end of the line*/
				{
					k=find_char(line,',',i);
					if(k==NOT_EXIST)/*legal number of operators*/
					    {
						k=skip_whitespaces_end(line+i,j-i-1);
						substring(value,line+i,k);/*saving the second operand*/
						error_exist=syntax_method(value,line_num,file_name);
				                if(error_exist)/*if there id an syntax problem*/
							return error_exist;
				          	if(label_exist)/*if a label exist we add it to the symbol list*/
							add_symbol(label,*IC,"code",&(*head_symbol));
					      *IC=(*IC)+3;/*count 3 words*/
					      return error_exist;
					    }/*k==not_exist*/
					else   {/*illegal number of operands*/
						 if(i==k||j==skip_whitespaces_begining(line,k+1))
						       {
							fprintf(stdout,"<file %s,line %d> there is an illegal comma at this line\n",file_name,line_num);
							error_exist=TRUE;
							return error_exist;
						 	}
				  		else
						       {
							fprintf(stdout,"<file %s,line %d> Too many operands\n",file_name,line_num);
							error_exist=TRUE;
							return error_exist;
							}
						}/*end of else*/
				}/*end of if 1!=j line 250*/
					else{/*i==j*/
						
						fprintf(stdout,"<file %s,line %d> Missing operand at this line\n",file_name,line_num);
						error_exist=TRUE;
						return error_exist;
					    }/*end of else (i==j)*/


			}/*end of j!=not_exist- a comma was found after the command*/
			else
			{
				fprintf(stdout,"<file %s,line %d> Missing operand at this line\n",file_name,line_num);
				error_exist=TRUE;
				return error_exist;
			}
		  }/*end of first if 2 operands instructions*/
		/*one operand commands*/
		if(!strcmp(command,"clr")|| !strcmp(command,"not")|| !strcmp(command,"inc")||!strcmp(command,"dec")|| !strcmp(command,"prn")||
			   !strcmp(command,"jmp")||!strcmp(command,"bne")||!strcmp(command,"jsr")||!strcmp(command,"red"))
			{
			j=find_char(line,' ',i);
			i=skip_whitespaces_begining(line,j+1);/*skipping the command, checked in the which_group function*/
			j=find_char(line,'\n',i);
				if(i!=j)
			           {
					k=find_char(line,',',i);
					if(k==NOT_EXIST)
						{
						k=skip_whitespaces_end(line+i,j-i-1);
						substring(value,line+i,k);/*saves the operand*/
						error_exist=syntax_method(value,line_num,file_name);/*checking the operand method*/
						if(error_exist)/*if a syntax error exist*/
							return error_exist;
						if(label_exist)/*if a label exist we add it to the symbol list*/
							add_symbol(label,*IC,"code",&(*head_symbol));
						*IC=(*IC)+2;/*increase the IC count*/
						return error_exist;
	
						}/*k==not_exist*/
					else{/*if a comma exist*/
						 if(i==k||j==skip_whitespaces_begining(line,k+1))
						      {
							fprintf(stdout,"<file %s, line%d> illegal comma\n",file_name,line_num);
							error_exist=TRUE;
						      }
						 else
						      {
							fprintf(stdout,"<file %s, line%d> Too many operands\n",file_name,line_num);/*+2 to skip the comma*/
							error_exist=TRUE;
						      }
					    }/*end of else*/

				   }/*end of if i!=j*/
				else/*end of if i==j*/
					{
						fprintf(stdout,"<file %s, line%d> Missing operand\n",file_name,line_num);
						error_exist=TRUE;
					}
			return error_exist;
			}/*end second if 1 operand instructions*/
		/*non-operands commands*/
		if(!strcmp(command,"rts")||!strcmp(command,"stop"))
			{
				j=find_char(line,'\n',i);
				k=find_char(line,' ',i);
				if(k!=NOT_EXIST&&j!=skip_whitespaces_begining(line,k+1))/*if there are more operands at this line*/
		    	       {
				fprintf(stdout,"<file %s,line %d> Too many operands\n",file_name,line_num);
				error_exist=TRUE;
				return error_exist;
			       }/*end of if k!=not_exist&&j!=skip_whitespaces_begining*/
				 if(label_exist)/*if a label exist we add it to the symbol list*/
					add_symbol(label,*IC,"code",&(*head_symbol));
				(*IC)++;
				return error_exist;
			}/*end of third if non operands*/
	}/*end of which_command!=NOT_EXIST*/
	else /*which command==NOT_EXIST*/
		{
			fprintf(stdout,"<file %s,line %d> Unknown instruction\n",file_name,line_num);
			error_exist=TRUE;
			return error_exist;
		}

return error_exist;
}/*end of first trans*/

