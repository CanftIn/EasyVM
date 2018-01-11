#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

typedef enum
{
	PUSH,		/* 0  PUSH data                    :: pushes data to the stack */
	ADD,		/* 1  ADD Register_A, Register_B   :: pops two values on top of the stack, adds them pushes to stack */
	POP,		/* 2  POP                          :: pops the value on the stack, will also print it for debugging */
	SET,		/* 3  SET Register, data           :: sets register */
	RET,		/* 4  RET                          :: stop the program */
	MUL,		/* 5  MUL Register_A, Register_B   :: multiplies top two vals on stack */
	DIV,		/* 6  DIV Register_A, Register_B   :: divides top two vals on stack */
	SUB,		/* 7  SUB Register_A, Register_B   :: substracts top two vals on stack */
	MOV,		/* 8  MOV Register_A, Register_B   :: movs the value in Register_A to Register_B */
	NOP			/* 9  NOP                          :: nothing*/
}Instructions;

const int program[] = {
	PUSH, 5,
	PUSH, 6,
	ADD,
	POP,
	RET
};

/*
	Registers:
		six registers: A, B, C, D, E, F
		IP and SP registers: PC, SP
		numbers of registers: NUM_OF_REGISTERS : 8
*/
typedef enum
{
	A, B, C, D, E, F, PC, SP,
	NUM_OF_REGISTERS
}Registers;

/* state machine running label*/
bool running = true; 

/* instruction stack */
int stack[256]; 

/* how many instructions to do */
int instruction_count = 0;

// how much space is allocated for the instructions
// 4 instructions by default
int instruction_space = 4;

static int registers[NUM_OF_REGISTERS];

int *instructions;

/* stack point */
#define SP (registers[SP])

/* instruction point*/
#define IP (registers[PC])

#define FETCH (instructions[IP])



/*
	state machine
*/
void eval(int instr)
{
	switch (instr)
	{
		case RET:
		{
			running = false;
			printf("Finished Execution. \n");
			break;
		}
		case PUSH:
		{
			SP++;
			IP++;
			stack[SP] = program[IP];
			break;
		}
		case POP:
		{
			SP--;
			int val_popped = stack[SP];
			printf("popped %d \n", val_popped);
			break;
		}
		case ADD:
		{
			registers[A] = stack[SP];
			SP--;
			registers[B] = stack[SP];
			SP--;
			registers[C] = registers[A] + registers[B];
			SP++;
			stack[SP] = registers[C];
			printf("%d + %d = %d\n", registers[A], registers[B], registers[C]);
			break;
		}
		case MUL: 
		{
			registers[A] = stack[SP];
			SP--;
			registers[B] = stack[SP];
			SP--;
			registers[C] = registers[B] * registers[A];
			SP++;
			stack[SP] = registers[C];
			printf("%d * %d = %d\n", registers[B], registers[A], registers[C]);
			break;
		}
		case DIV:
		{
			registers[A] = stack[SP];
			SP--;
			registers[B] = stack[SP];
			SP--;
			registers[C] = registers[B] / registers[A];
			SP++;
			stack[SP] = registers[C];
			printf("%d / %d = %d\n", registers[B], registers[A], registers[C]);
			break;
		}
		case SUB:
		{
			registers[A] = stack[SP];
			SP--;
			registers[B] = stack[SP];
			SP--;
			registers[C] = registers[B] - registers[A];
			SP++;
			stack[SP] = registers[C];
			printf("%d - %d = %d\n", registers[B], registers[A], registers[C]);
			break;
		}
		case MOV: 
		{
			registers[instructions[IP + 2]] = registers[instructions[IP + 1]];
			IP = IP + 2;
			break;
		}
		case NOP: 
		{
			printf("Do Nothing\n");
			break;
		}
		default: 
		{
			printf("Unknown Instruction %d\n", instr);
			break;
		}
	}
}


int main(int argc, char **argv)
{
	if (argc <= 1)
	{
		printf("error: no input files.\n");
		return -1;
	}

	//filename
	char *filename = argv[1];

	FILE *file = fopen(filename, "r");
	if (!file)
	{
		printf("error: could not read file: '%s' \n", filename);
		return -1;
	}

	//allocate the memory for instrutions
	instructions = malloc(sizeof(*instructions) * instruction_space);

	//read file
	int num;
	int i = 0;
	while (fscanf(file, "%d", &num) > 0)
	{
		instructions[i] = num;
		printf("%d\n", instructions[i]);
		i++;
		if (i >= instruction_space) {
			instruction_space *= 2;
			instructions = realloc(instructions, sizeof(*instructions) * instruction_space); // double size
		}
	}

	// set 'instruction_count' to number of instructions read
	instruction_count = i;

	// close the file
	fclose(file);

	// initialize stack pointer
	SP = -1;

	while (running)
	{
		eval(FETCH);
		IP++;
	}

	free(instructions);

	return 0;
}