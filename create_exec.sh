nasm -f elf64 out.asm -o brainfuck.o
ld brainfuck.o -o a.out
rm brainfuck.o
