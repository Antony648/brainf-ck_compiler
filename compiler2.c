#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#define NO_INPUT	1
#define SYNTAX_ERROR	2
#define WRITE_TO_FILE_ERROR 3
#define SOME_ERROR 4

enum ops:unsigned char
{
	PLUS='+',MINUS='-',DOT='.',COMMA=',',OPEN_BRACKET='[',CLOSE_BRACKET=']',LEFT='<',RIGHT='>',END='\0'
};
struct jmp_table_entry
{
	enum ops symbol;
	int self;
	int referal;
};
int get_ops_len(char* inp_str)
{

	int count=0;
	int balance=0;
	for(int i=0;inp_str[i];i++)
	{
		switch(inp_str[i])
		{
			case '+':count++;break;
			case '-':count++;break;
			case '>':count++;break;
			case '<':count++;break;
			case ',':count++;break;
			case '.':count++;break;
			case '[':count++;balance++;break;
			case ']':count++;balance--;break;
			default:break;
		}
		if(balance <0)
			return 0;
	}
	if(balance)
		return 0;
	return ++count;
}
int set_inp_str(enum ops* inp_str,char* string)
{
	//enum ops* inp_str=*inp_str_addr;
	int j=0;
	int ret=0;
	for(int i=0;string[i];i++)
	{
		switch(string[i])
		{
			case '+':inp_str[j]=PLUS;j++;break;
			case '-':inp_str[j]=MINUS;j++;break;
			case '>':inp_str[j]=RIGHT;j++;break;
			case '<':inp_str[j]=LEFT;j++;break;
			case ',':inp_str[j]=COMMA;j++;break;
			case '.':inp_str[j]=DOT;j++;break;
			case '[':inp_str[j]=OPEN_BRACKET;j++;ret++;break;
			case ']':inp_str[j]=CLOSE_BRACKET;j++;ret++;break;
			default:break;
		}
	}
	inp_str[j]=END;	
	return ret;
}
void set_jmp_table(struct jmp_table_entry* jmp_table, enum ops* inp_str,int loop_count)
{
	int stack[loop_count/2 +1];
	int top=-1;
	int i=0;
	int index=0;
	
	while(inp_str[i]!= END)
	{
		if(inp_str[i]==OPEN_BRACKET)
		{ 
			top++;
			stack[top]=index;
			jmp_table[index].symbol=OPEN_BRACKET;
			jmp_table[index].self=index;
			index++;
			//jmp_table[count].referal
			i++;
			continue;
		}
		if(inp_str[i]==CLOSE_BRACKET)
		{
			jmp_table[stack[top]].referal=index;
			jmp_table[index].self=index;
			jmp_table[index].referal=stack[top];
			jmp_table[index].symbol=CLOSE_BRACKET;
			top--;
			index++;
		}
		i++;
	}
	
}
int write_code(int asm_file,enum ops* inp_str,struct jmp_table_entry* jmp_table)
{
	ssize_t  bytes_written;
	char* basic[]={"global _start\n","section .data\n","section .bss\n","\ttape resb 30000\n","section .text\n","_start:\n","\tlea rbx,[tape]\n"};
	for(int i=0;i<7;i++)
	{
		bytes_written=write(asm_file,basic[i],strlen(basic[i]));
		if(bytes_written ==-1)
		{
			printf("some error occured in writing to file");
			return -WRITE_TO_FILE_ERROR;
		}
	}
	int op_count=0,jmp_table_index=0; char* str;
	char* add_str="\tinc byte [rbx]\n";
	char* sub_str="\tdec byte [rbx]\n";
	char* dot_str[]={"\tmov rax,1\n","\tmov rdi,1\n","\tmov rsi,rbx\n","\tmov rdx,1\n","\tsyscall\n"};
	char* comma_str[]={"\tmov rax,0\n","\tmov rdi,1\n","\tmov rsi,rbx\n","\tmov rdx,1\n","\tsyscall\n"};
	char* open_loop[]={"loop_start",":\n","\tcmp byte [rbx],0\n","\tje loop_end","\n"};
	char* close_loop[]={"\tcmp byte [rbx],0\n","\tjne loop_start","\n","loop_end",":\n"};
	char* left_str="\tdec rbx\n";
	char* right_str="\tinc rbx\n";
	while(inp_str[op_count]!=END)
	{
		switch(inp_str[op_count])
		{
			case PLUS:
				bytes_written=write(asm_file,add_str,strlen(add_str));
				break;
			case MINUS:
				bytes_written=write(asm_file,sub_str,strlen(sub_str));
				break;
			case DOT:
				for(int i=0;i<5;i++)
				{
					bytes_written=write(asm_file,dot_str[i],strlen(dot_str[i]));
					if(bytes_written==-1)
					{
						printf("failed to write");
						return -WRITE_TO_FILE_ERROR;
					}
				}
				break;
			case COMMA:
				for(int i=0;i<5;i++)
				{
					bytes_written=write(asm_file,comma_str[i],strlen(comma_str[i]));
					if(bytes_written==-1)
					{
						printf("failed to write");
						return -WRITE_TO_FILE_ERROR;
					}
				}
				break;
			case OPEN_BRACKET:
				if(jmp_table[jmp_table_index].symbol!=OPEN_BRACKET)
				{
					printf("some error occured");
					return -SOME_ERROR;
				}
				for(int i=0;i<5;i++)
				{
					if(i==1)
					{
						char a[12];
						sprintf(a,"%d",jmp_table[jmp_table_index].self);
						write(asm_file,a,strlen(a));
					}
					if(i==4)
					{
						char a[12];
						sprintf(a,"%d",jmp_table[jmp_table_index].referal);
						write(asm_file,a,strlen(a));
					}
					bytes_written=write(asm_file,open_loop[i],strlen(open_loop[i]));
				}
				jmp_table_index++;
				break;
			case CLOSE_BRACKET:
				if(jmp_table[jmp_table_index].symbol!=CLOSE_BRACKET)
				{
					printf("some error occured");
					return -SOME_ERROR;
				}
				for(int i=0;i<5;i++)
				{
					if(i==2)
					{
						char a[12];
						sprintf(a,"%d",jmp_table[jmp_table_index].referal);
						write(asm_file,a,1);
					}
					if(i==4)
					{
						char a[12];
						sprintf(a,"%d",jmp_table[jmp_table_index].self);
						write(asm_file,a,1);
					}
					bytes_written=write(asm_file,close_loop[i],strlen(close_loop[i]));
				}
				jmp_table_index++;
				break;
			case LEFT:
				bytes_written=write(asm_file,left_str,strlen(left_str));
				break;
			case RIGHT:
				bytes_written=write(asm_file,right_str,strlen(right_str));
				break;
		}
		op_count++;
	}
	char* end[]={"\tmov rax, 60\n","\txor rdi,rdi\n","\tsyscall"};
	for(int i=0;i<3;i++)
	{
		
		bytes_written=write(asm_file,end[i],strlen(end[i]));
		if(bytes_written ==-1)
		{
			printf("some error occured in writing to file");
			return -WRITE_TO_FILE_ERROR;
		}
	}
}
int main(int argc,char* argv[])
{
	if(!argv[1] || argv[1]=="")
	{
		printf("usage bfc <program string>");
		return -NO_INPUT;
	}
	int len_inp_str=get_ops_len(argv[1]);
	if(!len_inp_str)
	{
		printf("syntax error");
		return -SYNTAX_ERROR;
	}
	enum ops inp_str[len_inp_str];
	int loop_count=set_inp_str(inp_str,argv[1]);
	struct jmp_table_entry jmp_table[loop_count];
	set_jmp_table(jmp_table,inp_str,loop_count);
	//make .asm file
	int asm_file=creat("./out.asm",0644);
	if(asm_file ==-1)
	{
		printf("failed in creating file out.asm");
		return 0;
	}
	write_code(asm_file,inp_str,jmp_table);
	close(asm_file);
	system("nasm -f elf64 out.asm");
	system("ld out.o -o a.out");
	system("rm out.asm out.o");
	return 0;

}
