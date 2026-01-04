#include <stdio.h>
#define NO_INPUT	1
#define SYNTAX_ERROR	2

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

	int count;
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
	return 0;

}
