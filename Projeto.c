#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int R[36] = {0};	// registradores
uint32_t *mem;		// mémória

long calcularTamanhoArquivo(FILE *arquivo) {
 
    // guarda o estado ante de chamar a função fseek
    long posicaoAtual = ftell(arquivo);
 
    // guarda tamanho do arquivo
    long tamanho;
 
    // calcula o tamanho
    fseek(arquivo, 0, SEEK_END);
    tamanho = ftell(arquivo);
 
    // recupera o estado antigo do arquivo
    fseek(arquivo, posicaoAtual, SEEK_SET);
 
    return tamanho;
}

int main (int argc, char* argv[]) {
	
	FILE* input = fopen(argv[1], "r");
	FILE* output = fopen(argv[2], "w");
	
	mem = (uint32_t *) malloc(sizeof(uint32_t) * 64);
	
	if (input == NULL) {
        printf("[SOFTWARE INTERRUPTION]\n");
        exit(1);
    }
    int i = 0;
    while (fscanf(input, "%X", &mem[i]) != EOF) {
        i++;
    }
    
	printf("[START OF SIMULATION]\n");

	int execute = 1;

	while (execute) {
		
		int desvio = 0;
		
		unsigned int opcode = (mem[R[32]] & 0xFC000000) >> 26;
		unsigned int datacode = mem[R[32]] & 0x3FFFFFF;
		
		switch (opcode) {
			case 0x00:
			//	_add();
				break;
			case 0x01:
				_addi(datacode);
				break;
			case 0x02:
			//	_sub();
				break;
			case 0x03:
			//	_subi();
				break;
			case 0x04:
				_mul(datacode);
				break;
			case 0x05:
			//	_muli();
				break;
			case 0x06:
			//	_div();
				break;
			case 0x07:
			//	_divi();
				break;
			case 0x08:
				_cmp(datacode);
				break;
			case 0x09:
			//	_cmpi(datacode);
				break;
			case 0x0A:
			//	_shl(datacode);
				break;
			case 0x0B:
			//	_shr(datacode);
				break;
			case 0x0C:
			//	_and(datacode);
				break;
			case 0x0D:
			//	_andi(datacode);
				break;
			case 0x0E:
			//	_not(datacode);
				break;
			case 0x0F:
			//	_noti(datacode);
				break;
			case 0x10:
			//	_or(datacode);
				break;
			case 0x11:
			//	_ori(datacode);
				break;
			case 0x12:
			//	_xor(datacode);
				break;
			case 0x13:
			//	_xori(datacode);
				break;
			case 0x14:
				_ldw(datacode);
				break;
			case 0x15:
			//	_ldb(datacode);
				break;
			case 0x16:
				_stw(datacode);
				break;
			case 0x17:
			//	_stb(datacode);
				break;
			case 0x1A:
				_bun(datacode);
				desvio = 1;
				break;
			case 0x1B:
				_beq(datacode);
				desvio = 1;
				break;
			case 0x1C:
				_blt(datacode);
				desvio = 1;
				break;
			case 0x1D:
				_bgt(datacode);
				desvio = 1;
				break;
			case 0x1E:
				_bne(datacode);
				desvio = 1;
				break;
			case 0x1F:
				_ble(datacode);
				desvio = 1;
				break;
			case 0x20:
				_bge();
				desvio = 1;
				break;
			case 0x3F:
				execute = _int(datacode);
				desvio = 1;
				break;
			default:
				printf("[INVALID INSTRUCTION @ 0x%08X]\n", R[32] * 4);
		}
				
		if (!desvio)
			R[32]++;
	}
	printf("[END OF SIMULATION]\n");
	
	fclose(input);
	fclose(output);
	
	return 0;
}

_add (int datacode) {
	
}

_addi (int datacode) {

	unsigned int IM16 = (datacode & 0x3FFFC00) >> 10;
	unsigned int X = (datacode & 0x3E0) >> 5;
	unsigned int Y = (datacode & 0x1F);
	
	R[X] = R[Y] + IM16;
	
	printf("addi r%d, r%d, %d\n", X, Y, IM16);
	printf("[F] FR = 0x%08X, R%d = R%d + 0x%04X = 0x%08X\n", R[35], X, Y, IM16, R[X]);	
}

_mul (int datacode) {
	
	// A SER REVISADA A QUESTÃO DA EXTENSÃO (ER)
	//
	//
	//
	unsigned int E = (datacode & 0x38000) >> 15;
	unsigned int Z = (datacode & 0x7C00) >> 10;
	unsigned int X = (datacode & 0x3E0) >> 5;
	unsigned int Y = (datacode & 0x1F);
	
	R[Z] = R[X] * R[Y];
	
	printf("mul r%d, r%d, r%d\n", Z, X, Y);
	printf("[U] FR = 0x%08X, ER = 0x%08X, R%d = R%d * R%d = 0x%08X\n", R[35], R[34], Z, X, Y, R[Z]);
}

_cmp (datacode) {
		
	unsigned int E = (datacode & 0x18000) >> 15;
	unsigned int X = (datacode & 0x3E0) >> 5;
	unsigned int Y = (datacode & 0x1F);
	
	if (R[X] == R[Y])
		R[35] = 1;	// EQ = 001
	else if (R[X] < R[Y])
		R[35] = 2;	// LT = 010
	else
		R[35] = 4;	// GT = 100
	
	printf("cmp r%d, r%d\n", X, Y);
	printf("[U] FR = 0x%08X\n", R[35]);
}

_ldw (int datacode) {
	
	unsigned int IM16 = (datacode & 0x3FFFC00) >> 10;
	unsigned int X = (datacode & 0x3E0) >> 5;
	unsigned int Y = (datacode & 0x1F);
	
	R[X] = mem[R[Y] + IM16];
	
	printf("ldw r%d, r%d, 0x%04X\n", X, Y, IM16);
	printf("[F] R%d = MEM[(R%d + 0x%04X) << 2] = 0x%08X\n", X, Y, IM16, R[X]);
}

_stw (datacode) {
	
	unsigned int IM16 = (datacode & 0x3FFFC00) >> 10;
	unsigned int X = (datacode & 0x3E0) >> 5;
	unsigned int Y = (datacode & 0x1F);
	
	mem[R[X] + IM16] = R[Y];
	
	printf("stw r%d, 0x%04X, r%d\n", X, IM16, Y);
	printf("[F] MEM[(R%d + 0x%04X) << 2] = R%d = 0x%08X\n", X, IM16, Y, mem[R[X] + IM16]);
}

_bun (int datacode) {
	
	unsigned int IM26 = datacode;
	
	R[32] = IM26;
	printf("bun 0x%08X\n", IM26);
	printf("[S] PC = 0x%08X\n", R[32] * 4);
}

_beq (int datacode) {
	
	unsigned int IM26 = datacode;
	
	if (R[35] == 1)
		R[32] = IM26;
	else
		R[32]++;
		
	printf("beq 0x%08X\n", IM26);
	printf("[S] PC = 0x%08X\n", R[32] * 4);
}

_blt (int datacode) {
	
	unsigned int IM26 = datacode;
	
	if (R[35] == 2)
		R[32] = IM26;
	else
		R[32]++;
		
	printf("blt 0x%08X\n", IM26);
	printf("[S] PC = 0x%08X\n", R[32] * 4);
}

_bgt (int datacode) {
	
	unsigned int IM26 = datacode;
	
	if (R[35] == 4)
		R[32] = IM26;
	else
		R[32]++;
		
	printf("bgt 0x%08X\n", IM26);
	printf("[S] PC = 0x%08X\n", R[32] * 4);
}

_bne (int datacode) {
	
	unsigned int IM26 = datacode;
	
	if (R[35] == 0)
		R[32] = IM26;
	else
		R[32]++;
		
	printf("bne 0x%08X\n", IM26);
	printf("[S] PC = 0x%08X\n", R[32] * 4);
}

_ble (int datacode) {
	
	unsigned int IM26 = datacode;
	
	if (R[35] == 1 || R[35] == 2)
		R[32] = IM26;
	else
		R[32]++;
		
	printf("ble 0x%08X\n", IM26);
	printf("[S] PC = 0x%08X\n", R[32] * 4);
}

_bge (int datacode) {
	
	unsigned int IM26 = datacode;
	
	if (R[35] == 1 || R[35] == 4)
		R[32] = IM26;
	else
		R[32]++;
		
	printf("bge 0x%08X\n", IM26);
	printf("[S] PC = 0x%08X\n", R[32] * 4);
}

int _int (int datacode) {
	
	unsigned int IM26 = datacode;
	
	R[32] = IM26;	// zera o PC
	
	printf("int %d\n", IM26);
	printf("[S] CR = 0x%08X, PC = 0x%08X\n", 0, R[32] * 4);
	
	if (IM26 == 0)
		return 0;
	return 1;
}
