#include <stdio.h>
#include <stdint.h>
char* handle_open(char const* code,char* code_runner)
{
	int hammer=0;
	code_runner++;
	while(*code_runner)
	{
		if(*code_runner ==']' && hammer==0)
			break;
		else if(*code_runner=='[')
			hammer++;
		else if(*code_runner==']')
			hammer--;
		code_runner++;
	}
	if(*code_runner)
		return (code_runner);
	return (char*)0x20;
}
char* handle_close(char const* code,char* code_runner)
{
	int hammer=0;
	code_runner--;
	while(code_runner != code)
	{
		if(*code_runner=='[' && hammer==0)
			break;
		else if(*code_runner==']')
			hammer++;
		else if(*code_runner=='[')
			hammer--;
		code_runner--;
	}
	if(*code_runner == '[')	
		return (code_runner);	
	return (char*)0x20;
}
int brainfuck(char const* code,char const* input,char* output )
{
	unsigned char tape[100];
	for(int i=0;i<100;i++)
		tape[i]=0;
	unsigned char* tape_runner=tape;
	char* code_runner=(char*)code;
	while(*code_runner)
	{
		switch(*code_runner)
		{
			case '.':*output=*tape_runner;
				 output++;
				break;
			case ',':*tape_runner=*input;
				 input++;
				break;
			case '+':*tape_runner+=1;
				break;
			case '-':*tape_runner-=1;
				break;
			case '>':tape_runner+=1;
				 if(tape_runner > (tape+100))
					 return -1;
				break;
			case '<':
				if(tape_runner==tape)
					return -1;	//will cause negative access and segfault
				tape_runner-=1;
				break;
			case '[':
				 if(!*tape_runner)
				 {
				 	code_runner=handle_open(code,code_runner);
				 	if(code_runner==(char*)0x20)
						 return -1;
				 }
				break;
			case ']':
				if(*tape_runner)
				{
					code_runner=handle_close(code,code_runner);
					if(code_runner==(char*)0x20)
						return -1;
				}
				break;
			default:
				 printf("unknown operand!");
					 return -1;

		}
		code_runner++;
	}
	*output=0x00;

	return 0;

}
int main(int argc,char* argv[])
{
	char const  code[]="+++++>+++++>+++++<<<+++++.+.+.";
	char const input[]="D";
	char output[200];
	if(!brainfuck(code,input,output))
		printf("the output is:%s",output);
	else
		printf("some error occured.\n");
	return 0;
}
