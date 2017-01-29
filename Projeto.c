#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

uint32_t R[36] = {0};	// registradores
uint32_t *mem;			// memória

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
		
		unsigned int OP = (mem[R[32]] & 0xFC000000) >> 26;
		unsigned int IM26 = (mem[R[32]] & 0x3FFFFFF);
		unsigned int IM16 = (mem[R[32]] & 0x3FFFC00) >> 10;
		unsigned int Y = ((mem[R[32]] & 0x8000) >> 10) + (mem[R[32]] & 0x1F);
		unsigned int X = ((mem[R[32]] & 0x10000) >> 11) + ((mem[R[32]] & 0x3E0) >> 5);
		unsigned int Z = ((mem[R[32]] & 0x20000) >> 12) + ((mem[R[32]] & 0x7C00) >> 10);
		
		switch (OP) {
			case 0x00:
			//	_add();
				break;
			case 0x01:
				_addi(IM16, X, Y);
				break;
			case 0x02:
			//	_sub();
				break;
			case 0x03:
			//	_subi();
				break;
			case 0x04:
				_mul(Z, X, Y);
				break;
			case 0x05:
			//	_muli();
				break;
			case 0x06:
			//	_div();
				break;
			case 0x07:
				_divi(IM16, X, Y);
				break;
			case 0x08:
				_cmp(X, Y);
				break;
			case 0x09:
				_cmpi(X, IM16);
				break;
			case 0x0A:
				_shl(Z, X, Y);
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
				_ldw(IM16, X, Y);
				break;
			case 0x15:
			//	_ldb(datacode);
				break;
			case 0x16:
				_stw(IM16, X, Y);
				break;
			case 0x17:
			//	_stb(datacode);
				break;
			case 0x1A:
				_bun(IM26);
				desvio = 1;
				break;
			case 0x1B:
				_beq(IM26);
				desvio = 1;
				break;
			case 0x1C:
				_blt(IM26);
				desvio = 1;
				break;
			case 0x1D:
				_bgt(IM26);
				desvio = 1;
				break;
			case 0x1E:
				_bne(IM26);
				desvio = 1;
				break;
			case 0x1F:
				_ble(IM26);
				desvio = 1;
				break;
			case 0x20:
				_bge(IM26);
				desvio = 1;
				break;
			case 0x3F:
				execute = _int(IM26);
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

_addi (unsigned int IM16, unsigned int X, unsigned int Y) {
	
	// OV
	uint64_t Ry = R[Y];
	uint64_t Im = IM16;
	
	if (Ry + Im > UINT32_MAX) {
		R[35] = R[35] | 0x10;
	}
	
	R[X] = R[Y] + IM16;
	
	printf("addi r%d, r%d, %d\n", X, Y, IM16);
	printf("[F] FR = 0x%08X, R%d = R%d + 0x%04X = 0x%08X\n", R[35], X, Y, IM16, R[X]);	
}

_mul (unsigned int Z, unsigned int X, unsigned int Y) {
	
	// A SER REVISADA A QUESTÃO DA EXTENSÃO (ER)
	//
	// 0000 0000 0000 0000 0000 0000 0000 0000
	// 1111 1111 1111 1111 1111 1111 1111 1111 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000

	R[Z] = R[X] * R[Y];
	
	printf("mul r%d, r%d, r%d\n", Z, X, Y);
	printf("[U] FR = 0x%08X, ER = 0x%08X, R%d = R%d * R%d = 0x%08X\n", R[35], R[34], Z, X, Y, R[Z]);
}

_divi (unsigned int IM16, unsigned int X, unsigned int Y) {
	
	// R[34] = ER
	
	if (IM16 == 0) {
		R[35] = R[35] | 0x08;
	} else {
		R[34] = R[Y] % IM16;
		R[X] = R[Y] / IM16;
	}
	
	printf("divi r%d, r%d, %d\n", X, Y, IM16);
	printf("[F] FR = 0x%08X, ER = 0x%08X, R%d = R%d / 0x%04X = 0x%08X\n", R[35], R[34], X, Y, IM16, R[X]);
}

_cmp (unsigned int X, unsigned int Y) {
	
	R[35] = R[35] & 0x18;
	
	if (R[X] == R[Y])
		R[35] = R[35] | 0x01;	// EQ = 001
	else if (R[X] < R[Y])
		R[35] = R[35] | 0x02;	// LT = 010
	else
		R[35] = R[35] | 0x04;	// GT = 100
	
	printf("cmp r%d, r%d\n", X, Y);
	printf("[U] FR = 0x%08X\n", R[35]);
}

_cmpi (unsigned int X, unsigned int IM16) {
	
	R[35] = R[35] & 0x18;
	
	if (R[X] == IM16)
		R[35] = R[35] | 0x01;	// EQ = 001
	else if (R[X] < IM16)
		R[35] = R[35] | 0x02;	// LT = 010
	else
		R[35] = R[35] | 0x04;	// GT = 100
	
	printf("cmpi r%d, %d\n", X, IM16);
	printf("[F] FR = 0x%08X\n", R[35]);
}

_shl (unsigned int Z, unsigned int X, unsigned int Y) {
	
	
	R[Z] = R[X] << (Y + 1);

	printf("shl r%d, r%d, %d\n", Z, X, Y);
	printf("[U] ER = 0x%08X, R%d = R%d << %d = 0x%08X\n", R[34], Z, X, (Y + 1), R[Z]);
}

_ldw (unsigned int IM16, unsigned int X, unsigned int Y) {

	R[X] = mem[R[Y] + IM16];
	
	printf("ldw r%d, r%d, 0x%04X\n", X, Y, IM16);
	printf("[F] R%d = MEM[(R%d + 0x%04X) << 2] = 0x%08X\n", X, Y, IM16, R[X]);
}

_stw (unsigned int IM16, unsigned int X, unsigned int Y) {

	mem[R[X] + IM16] = R[Y];
	
	printf("stw r%d, 0x%04X, r%d\n", X, IM16, Y);
	printf("[F] MEM[(R%d + 0x%04X) << 2] = R%d = 0x%08X\n", X, IM16, Y, mem[R[X] + IM16]);
}

_bun (unsigned int IM26) {

	R[32] = IM26;
	printf("bun 0x%08X\n", IM26);
	printf("[S] PC = 0x%08X\n", R[32] * 4);
}

_beq (unsigned int IM26) {
	
	if ((R[35] & 0x01) == 1)
		R[32] = IM26;
	else
		R[32]++;
		
	printf("beq 0x%08X\n", IM26);
	printf("[S] PC = 0x%08X\n", R[32] * 4);
}

_blt (unsigned int IM26) {
	
	if ((R[35] & 0x02) == 2)
		R[32] = IM26;
	else
		R[32]++;
		
	printf("blt 0x%08X\n", IM26);
	printf("[S] PC = 0x%08X\n", R[32] * 4);
}

_bgt (unsigned int IM26) {
	
	if ((R[35] & 0x04) == 4)
		R[32] = IM26;
	else
		R[32]++;
		
	printf("bgt 0x%08X\n", IM26);
	printf("[S] PC = 0x%08X\n", R[32] * 4);
}

_bne (unsigned int IM26) {

	if ((R[35] & 0x01) == 0)
		R[32] = IM26;
	else
		R[32]++;
		
	printf("bne 0x%08X\n", IM26);
	printf("[S] PC = 0x%08X\n", R[32] * 4);
}

_ble (unsigned int IM26) {
	
	if ((R[35] & 0x01) == 1 || (R[35] & 0x02) == 2)
		R[32] = IM26;
	else
		R[32]++;
		
	printf("ble 0x%08X\n", IM26);
	printf("[S] PC = 0x%08X\n", R[32] * 4);
}

_bge (unsigned int IM26) {
	
	if ((R[35] & 0x01) == 1 || (R[35] & 0x04) == 4)
		R[32] = IM26;
	else
		R[32]++;
		
	printf("bge 0x%08X\n", IM26);
	printf("[S] PC = 0x%08X\n", R[32] * 4);
}

int _int (unsigned int IM26) {
	
	R[32] = IM26;	// zera o PC
	
	printf("int %d\n", IM26);
	printf("[S] CR = 0x%08X, PC = 0x%08X\n", 0, R[32] * 4);
	
	if (IM26 == 0)
		return 0;
	return 1;
}
