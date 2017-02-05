#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

uint32_t R[36] = {0};	// registradores
uint32_t *mem;			// memória
FILE* input;			// ponteiro para arquivo de entrada
FILE* output;			// ponteiro para arquivo de saída

// R[32] = PC (Program Counter)
// R[33] = IR (Instruction Register)
// R[34] = ER (Extension Register)
// R[35] = FR (Flag Register)

void registerName (unsigned int n, char* reg) {
	
	switch (n) {
		case 32:
			sprintf(reg, "pc");
			break;
		case 33:
			sprintf(reg, "ir");
			break;
		case 34:
			sprintf(reg, "er");
			break;
		case 35:
			sprintf(reg, "fr");
			break;
		default:
			sprintf(reg, "r%d", n);
	}
}

void _add (unsigned int Z, unsigned int X, unsigned int Y) {
	
	uint64_t Rx = R[X];
	uint64_t Ry = R[Y];
	
	if (Z != 0) {
		R[Z] = R[X] + R[Y];
	}
	
	if (Rx + Ry > UINT32_MAX) {
		// Acende o campo OV do FR
		R[35] = R[35] | 0x10;
	} else {
		// Apaga o campo OV do FR
		R[35] = R[35] & 0xFFFFFFEF;
	}
	
	char reg_z[4];
	registerName(Z, reg_z);
	char reg_x[4];
	registerName(X, reg_x);
	char reg_y[4];
	registerName(Y, reg_y);
	
	fprintf(output, "add %s, %s, %s\n", reg_z, reg_x, reg_y);
	fprintf(output, "[U] FR = 0x%08X, %s = %s + %s = 0x%08X\n",
	R[35], strupr(reg_z), strupr(reg_x), strupr(reg_y), R[Z]);
}

void _addi (unsigned int IM16, unsigned int X, unsigned int Y) {
	
	uint64_t Ry = R[Y];
	uint64_t Im = IM16;
	
	if (X != 0) {
		R[X] = R[Y] + IM16;
	}
	
	if (Ry + Im > UINT32_MAX) {
		// Acende o campo OV do FR
		R[35] = R[35] | 0x10;
	} else {
		// Apaga o campo OV do FR
		R[35] = R[35] & 0xFFFFFFEF;
	}
	
	char reg_x[4];
	registerName(X, reg_x);
	char reg_y[4];
	registerName(Y, reg_y);
	
	fprintf(output, "addi %s, %s, %d\n", reg_x, reg_y, IM16);
	fprintf(output, "[F] FR = 0x%08X, %s = %s + 0x%04X = 0x%08X\n",
	R[35], strupr(reg_x), strupr(reg_y), IM16, R[X]);	
}

void _sub (unsigned int Z, unsigned int X, unsigned int Y) {
	
	uint64_t Rx = R[X];
	uint64_t Ry = R[Y];
			
	if (Z != 0) {
		R[Z] = R[X] - R[Y];
	}
	
	if (Rx - Ry > UINT32_MAX) {
		// Acende o campo OV do FR
		R[35] = R[35] | 0x10;
	} else {
		// Apaga o campo OV do FR
		R[35] = R[35] & 0xFFFFFFEF;
	}
	
	char reg_z[4];
	registerName(Z, reg_z);
	char reg_x[4];
	registerName(X, reg_x);
	char reg_y[4];
	registerName(Y, reg_y);
	
	fprintf(output, "sub %s, %s, %s\n", reg_z, reg_x, reg_y);
	fprintf(output, "[U] FR = 0x%08X, %s = %s - %s = 0x%08X\n",
	R[35], strupr(reg_z), strupr(reg_x), strupr(reg_y), R[Z]);
}

void _subi (unsigned int IM16, unsigned int X, unsigned int Y) {
	
	uint64_t Ry = R[Y];
	uint64_t Im = IM16;
	
	if (X != 0) {
		R[X] = R[Y] - IM16;
	}
	
	if (Ry - Im > UINT32_MAX) {
		// Acende o campo OV do FR
		R[35] = R[35] | 0x10;
	} else {
		// Apaga o campo OV do FR
		R[35] = R[35] & 0xFFFFFFEF;
	}
	
	char reg_x[4];
	registerName(X, reg_x);
	char reg_y[4];
	registerName(Y, reg_y);
	
	fprintf(output, "subi %s, %s, %d\n", reg_x, reg_y, IM16);
	fprintf(output, "[F] FR = 0x%08X, %s = %s - 0x%04X = 0x%08X\n",
	R[35], strupr(reg_x), strupr(reg_y), IM16, R[X]);
}

void _mul (unsigned int Z, unsigned int X, unsigned int Y) {
	
	uint64_t x = (uint64_t) R[X] * (uint64_t) R[Y];
	
	if (Z != 0) {
		R[Z] = (x & 0xFFFFFFFF);
	}
	R[34] = (x & 0xFFFFFFFF00000000) >> 32;
	
	if (x > UINT32_MAX) {
		// Acende o campo OV do FR
		R[35] = R[35] | 0x10;
	} else {
		// Apaga o campo OV do FR
		R[35] = R[35] & 0xFFFFFFEF;
	}
	
	char reg_z[4];
	registerName(Z, reg_z);
	char reg_x[4];
	registerName(X, reg_x);
	char reg_y[4];
	registerName(Y, reg_y);
	
	fprintf(output, "mul %s, %s, %s\n", reg_z, reg_x, reg_y);
	fprintf(output, "[U] FR = 0x%08X, ER = 0x%08X, %s = %s * %s = 0x%08X\n",
	R[35], R[34], strupr(reg_z), strupr(reg_x), strupr(reg_y), R[Z]);
}

void _muli (unsigned int IM16, unsigned int X, unsigned int Y) {
	
	uint64_t x = (uint64_t) R[Y] * (uint64_t) IM16;
	
	if (X != 0) {
		R[X] = (x & 0xFFFFFFFF);
	}
	R[34] = (x & 0xFFFFFFFF00000000) >> 32;
	
	if (x > UINT32_MAX) {
		// Acende o campo OV do FR
		R[35] = R[35] | 0x10;
	} else {
		// Apaga o campo OV do FR
		R[35] = R[35] & 0xFFFFFFEF;
	}
	
	char reg_x[4];
	registerName(X, reg_x);
	char reg_y[4];
	registerName(Y, reg_y);
	
	fprintf(output, "muli %s, %s, %d\n", reg_x, reg_y, IM16);
	fprintf(output, "[F] FR = 0x%08X, ER = 0x%08X, %s = %s * 0x%04X = 0x%08X\n",
	R[35], R[34], strupr(reg_x), strupr(reg_y), IM16, R[X]);
}

void _div (unsigned int Z, unsigned int X, unsigned int Y) {
	
	if (Y == 0) {
		// Acende o campo ZD do FR
		R[35] = R[35] | 0x08;
	} else {
		R[34] = R[X] % R[Y];
		if (Z != 0) {
			R[Z] = R[X] / R[Y];
		}
		// Apaga o campo ZD do FR
		R[35] = R[35] & 0xFFFFFFF7;
	}

	char reg_z[4];
	registerName(Z, reg_z);
	char reg_x[4];
	registerName(X, reg_x);
	char reg_y[4];
	registerName(Y, reg_y);
	
	fprintf(output, "div %s, %s, %s\n", reg_z, reg_x, reg_y);
	fprintf(output, "[U] FR = 0x%08X, ER = 0x%08X, %s = %s / %s = 0x%08X\n",
	R[35], R[34], strupr(reg_z), strupr(reg_x), strupr(reg_y), R[Z]);
}

void _divi (unsigned int IM16, unsigned int X, unsigned int Y) {
	
	if (IM16 == 0) {
		// Acende o campo ZD do FR
		R[35] = R[35] | 0x08;
	} else {
		R[34] = R[Y] % IM16;
		if (X != 0) {
			R[X] = R[Y] / IM16;
		}
		// Apaga o campo ZD do FR
		R[35] = R[35] & 0xFFFFFFF7;
	}
	
	char reg_x[4];
	registerName(X, reg_x);
	char reg_y[4];
	registerName(Y, reg_y);
	
	fprintf(output, "divi %s, %s, %d\n", reg_x, reg_y, IM16);
	fprintf(output, "[F] FR = 0x%08X, ER = 0x%08X, %s = %s / 0x%04X = 0x%08X\n",
	R[35], R[34], strupr(reg_x), strupr(reg_y), IM16, R[X]);
}

void _cmp (unsigned int X, unsigned int Y) {
	
	R[35] = R[35] & 0x18;
	
	if (R[X] == R[Y])
		R[35] = R[35] | 0x01;	// EQ = 001
	else if (R[X] < R[Y])
		R[35] = R[35] | 0x02;	// LT = 010
	else
		R[35] = R[35] | 0x04;	// GT = 100
	
	char reg_x[4];
	registerName(X, reg_x);
	char reg_y[4];
	registerName(Y, reg_y);
	
	fprintf(output, "cmp %s, %s\n", reg_x, reg_y);
	fprintf(output, "[U] FR = 0x%08X\n", R[35]);
}

void _cmpi (unsigned int X, unsigned int IM16) {
	
	R[35] = R[35] & 0x18;
	
	if (R[X] == IM16)
		R[35] = R[35] | 0x01;	// EQ = 001
	else if (R[X] < IM16)
		R[35] = R[35] | 0x02;	// LT = 010
	else
		R[35] = R[35] | 0x04;	// GT = 100
	
	char reg_x[4];
	registerName(X, reg_x);
	
	fprintf(output, "cmpi %s, %d\n", reg_x, IM16);
	fprintf(output, "[F] FR = 0x%08X\n", R[35]);
}

void _shl (unsigned int Z, unsigned int X, unsigned int Y) {
	
	uint64_t x = ((uint64_t) R[34] << 32) + R[X];
	x = x << (Y + 1);
	
	if (Z != 0) {
		R[Z] = (x & 0xFFFFFFFF);
	}
	R[34] = (x & 0xFFFFFFFF00000000) >> 32;
	
	char reg_z[4];
	registerName(Z, reg_z);
	char reg_x[4];
	registerName(X, reg_x);
	
	fprintf(output, "shl %s, %s, %d\n", reg_z, reg_x, Y);
	fprintf(output, "[U] ER = 0x%08X, %s = %s << %d = 0x%08X\n",
	R[34], strupr(reg_z), strupr(reg_x), (Y + 1), R[Z]);
}

void _shr (unsigned int Z, unsigned int X, unsigned int Y) {
	
	uint64_t x = ((uint64_t) R[34] << 32) + R[X];
	x = x >> (Y + 1);
	
	if (Z != 0) {
		R[Z] = (x & 0xFFFFFFFF);
	}
	R[34] = (x & 0xFFFFFFFF00000000) >> 32;
	
	char reg_z[4];
	registerName(Z, reg_z);
	char reg_x[4];
	registerName(X, reg_x);
	
	fprintf(output, "shr %s, %s, %d\n", reg_z, reg_x, Y);
	fprintf(output, "[U] ER = 0x%08X, %s = %s >> %d = 0x%08X\n",
	R[34], strupr(reg_z), strupr(reg_x), (Y + 1), R[Z]);
}

void _and (unsigned int Z, unsigned int X, unsigned int Y) {
	
	if (Z != 0) {
		R[Z] = R[X] & R[Y];
	}
	
	char reg_z[4];
	registerName(Z, reg_z);
	char reg_x[4];
	registerName(X, reg_x);
	char reg_y[4];
	registerName(Y, reg_y);
	
	fprintf(output, "and %s, %s, %s\n", reg_z, reg_x, reg_y);
	fprintf(output, "[U] %s = %s & %s = 0x%08X\n",
	strupr(reg_z), strupr(reg_x), strupr(reg_y), R[Z]);
}

void _andi (unsigned int IM16, unsigned int X, unsigned int Y) {
	
	if (X != 0) {
		R[X] = R[Y] & IM16;
	}
	
	char reg_x[4];
	registerName(X, reg_x);
	char reg_y[4];
	registerName(Y, reg_y);
	
	fprintf(output, "andi %s, %s, %d\n", reg_x, reg_y, IM16);
	fprintf(output, "[F] %s = %s & 0x%04X = 0x%08X\n",
	strupr(reg_x), strupr(reg_y), IM16, R[X]);
}

void _not (unsigned int X, unsigned int Y) {
	
	if (X != 0) {
		R[X] = ~R[Y];
	}
	
	char reg_x[4];
	registerName(X, reg_x);
	char reg_y[4];
	registerName(Y, reg_y);
	
	fprintf(output, "not %s, %s\n", reg_x, reg_y);
	fprintf(output, "[U] %s = ~%s = 0x%08X\n",
	strupr(reg_x), strupr(reg_y), R[X]);
}

void _noti (unsigned int IM16, unsigned int X) {
	
	if (X != 0) {
		R[X] = ~IM16;
	}
	
	char reg_x[4];
	registerName(X, reg_x);
	
	fprintf(output, "noti %s, %d\n", reg_x, IM16);
	fprintf(output, "[F] %s = ~0x%04X = 0x%08X\n", strupr(reg_x), IM16, R[X]);
}

void _or (unsigned int Z, unsigned int X, unsigned int Y) {
	
	if (Z != 0) {
		R[Z] = R[X] | R[Y];
	}
	
	char reg_z[4];
	registerName(Z, reg_z);
	char reg_x[4];
	registerName(X, reg_x);
	char reg_y[4];
	registerName(Y, reg_y);
	
	fprintf(output, "or %s, %s, %s\n", reg_z, reg_x, reg_y);
	fprintf(output, "[U] %s = %s | %s = 0x%08X\n",
	strupr(reg_z), strupr(reg_x), strupr(reg_y), R[Z]);
}

void _ori (unsigned int IM16, unsigned int X, unsigned int Y) {
	
	if (X != 0) {
		R[X] = R[Y] | IM16;
	}
	
	char reg_x[4];
	registerName(X, reg_x);
	char reg_y[4];
	registerName(Y, reg_y);
	
	fprintf(output, "ori %s, %s, %d\n", reg_x, reg_y, IM16);
	fprintf(output, "[F] %s = %s | 0x%04X = 0x%08X\n",
	strupr(reg_x), strupr(reg_y), IM16, R[X]);
}

void _xor (unsigned int Z, unsigned int X, unsigned int Y) {
	
	if (Z != 0) {
		R[Z] = R[X] ^ R[Y];
	}
	
	char reg_z[4];
	registerName(Z, reg_z);
	char reg_x[4];
	registerName(X, reg_x);
	char reg_y[4];
	registerName(Y, reg_y);
	
	fprintf(output, "xor %s, %s, %s\n", reg_z, reg_x, reg_y);
	fprintf(output, "[U] %s = %s ^ %s = 0x%08X\n",
	strupr(reg_z), strupr(reg_x), strupr(reg_y), R[Z]);
}

void _xori (unsigned int IM16, unsigned int X, unsigned int Y) {
	
	if (X != 0) {
		R[X] = R[Y] ^ IM16;
	}
	
	char reg_x[4];
	registerName(X, reg_x);
	char reg_y[4];
	registerName(Y, reg_y);
	
	fprintf(output, "xori %s, %s, %d\n", reg_x, reg_y, IM16);
	fprintf(output, "[F] %s = %s ^ 0x%04X = 0x%08X\n",
	strupr(reg_x), strupr(reg_y), IM16, R[X]);
}

void _ldw (unsigned int IM16, unsigned int X, unsigned int Y) {
	
	if (X != 0) {
		R[X] = mem[R[Y] + IM16];
	}
	
	char reg_x[4];
	registerName(X, reg_x);
	char reg_y[4];
	registerName(Y, reg_y);
	
	fprintf(output, "ldw %s, %s, 0x%04X\n", reg_x, reg_y, IM16);
	fprintf(output, "[F] %s = MEM[(%s + 0x%04X) << 2] = 0x%08X\n",
	strupr(reg_x), strupr(reg_y), IM16, R[X]);
}

void _ldb (unsigned int IM16, unsigned int X, unsigned int Y) {
	
	if (X != 0) {
		switch ((R[Y] + IM16) % 0x04) {
			case 0x00:
				R[X] = (mem[(R[Y] + IM16) >> 2] & 0xFF000000) >> 24;
				break;
			case 0x01:
				R[X] = (mem[(R[Y] + IM16) >> 2] & 0xFF0000) >> 16;
				break;
			case 0x02:
				R[X] = (mem[(R[Y] + IM16) >> 2] & 0xFF00) >> 8;
				break;
			default:
				R[X] = (mem[(R[Y] + IM16) >> 2] & 0xFF);
		}
	}
	
	char reg_x[4];
	registerName(X, reg_x);
	char reg_y[4];
	registerName(Y, reg_y);
	
	fprintf(output, "ldb %s, %s, 0x%04X\n", reg_x, reg_y, IM16);
	fprintf(output, "[F] %s = MEM[%s + 0x%04X] = 0x%02X\n",
	strupr(reg_x), strupr(reg_y), IM16, R[X]);
}

void _stw (unsigned int IM16, unsigned int X, unsigned int Y) {

	mem[R[X] + IM16] = R[Y];
	
	char reg_x[4];
	registerName(X, reg_x);
	char reg_y[4];
	registerName(Y, reg_y);
	
	fprintf(output, "stw %s, 0x%04X, %s\n", reg_x, IM16, reg_y);
	fprintf(output, "[F] MEM[(%s + 0x%04X) << 2] = %s = 0x%08X\n",
	strupr(reg_x), IM16, strupr(reg_y), mem[R[X] + IM16]);
}

void _stb (unsigned int IM16, unsigned int X, unsigned int Y) {
	
	mem[(R[X] + IM16) >> 2] = R[Y];
	
	char reg_x[4];
	registerName(X, reg_x);
	char reg_y[4];
	registerName(Y, reg_y);
	
	fprintf(output, "stb %s, 0x%04X, %s\n", reg_x, IM16, reg_y);
	fprintf(output, "[F] MEM[%s + 0x%04X] = %s = 0x%02X\n",
	strupr(reg_x), IM16, strupr(reg_y), mem[(R[X] + IM16) >> 2]);
}

void _push (unsigned int X, unsigned int Y) {
	
	R[X]--;
	mem[R[X]] = R[Y];
	
	char reg_x[4];
	registerName(X, reg_x);
	char reg_y[4];
	registerName(Y, reg_y);
	
	fprintf(output, "push %s, %s\n", reg_x, reg_y);
	fprintf(output, "[U] MEM[%s--] = %s = 0x%08X\n",
	strupr(reg_x), strupr(reg_y), mem[R[X]]);
}

void _pop (unsigned int X, unsigned int Y) {
	
	if (X != 0) {
		R[X] = mem[R[Y]++];
	}
	
	char reg_x[4];
	registerName(X, reg_x);
	char reg_y[4];
	registerName(Y, reg_y);
	
	fprintf(output, "pop %s, %s\n", reg_x, reg_y);
	fprintf(output, "[U] %s = MEM[++%s] = 0x%08X\n", strupr(reg_x), strupr(reg_y), R[X]);
}

void _bun (unsigned int IM26) {

	R[32] = IM26;
	
	fprintf(output, "bun 0x%08X\n", IM26);
	fprintf(output, "[S] PC = 0x%08X\n", R[32] * 4);
}

void _beq (unsigned int IM26) {
	
	if ((R[35] & 0x01) == 1)
		R[32] = IM26;
	else
		R[32]++;
		
	fprintf(output, "beq 0x%08X\n", IM26);
	fprintf(output, "[S] PC = 0x%08X\n", R[32] * 4);
}

void _blt (unsigned int IM26) {
	
	if ((R[35] & 0x02) == 2)
		R[32] = IM26;
	else
		R[32]++;
		
	fprintf(output, "blt 0x%08X\n", IM26);
	fprintf(output, "[S] PC = 0x%08X\n", R[32] * 4);
}

void _bgt (unsigned int IM26) {
	
	if ((R[35] & 0x04) == 4)
		R[32] = IM26;
	else
		R[32]++;
		
	fprintf(output, "bgt 0x%08X\n", IM26);
	fprintf(output, "[S] PC = 0x%08X\n", R[32] * 4);
}

void _bne (unsigned int IM26) {

	if ((R[35] & 0x01) == 0)
		R[32] = IM26;
	else
		R[32]++;
		
	fprintf(output, "bne 0x%08X\n", IM26);
	fprintf(output, "[S] PC = 0x%08X\n", R[32] * 4);
}

void _ble (unsigned int IM26) {
	
	if ((R[35] & 0x01) == 1 || (R[35] & 0x02) == 2)
		R[32] = IM26;
	else
		R[32]++;
		
	fprintf(output, "ble 0x%08X\n", IM26);
	fprintf(output, "[S] PC = 0x%08X\n", R[32] * 4);
}

void _bge (unsigned int IM26) {
	
	if ((R[35] & 0x01) == 1 || (R[35] & 0x04) == 4)
		R[32] = IM26;
	else
		R[32]++;
		
	fprintf(output, "bge 0x%08X\n", IM26);
	fprintf(output, "[S] PC = 0x%08X\n", R[32] * 4);
}

void _call (unsigned int IM16, unsigned int X, unsigned int Y) {
	
	if (X != 0) {
		R[X] = R[32] + 1;
	}
	R[32] = R[Y] + IM16;
	
	char reg_x[4];
	registerName(X, reg_x);
	char reg_y[4];
	registerName(Y, reg_y);
	
	fprintf(output, "call %s, %s, 0x%04X\n", reg_x, reg_y, IM16);
	fprintf(output, "[F] %s = (PC + 4) >> 2 = 0x%08X, PC = (%s + 0x%04X) << 2 = 0x%08X\n",
	strupr(reg_x), R[X], strupr(reg_y), IM16, R[32] * 4);	
}

void _ret (unsigned int X) {
	
	R[32] = R[X];
	
	char reg_x[4];
	registerName(X, reg_x);
	
	fprintf(output, "ret %s\n", reg_x);
	fprintf(output, "[F] PC = %s << 2 = 0x%08X\n", strupr(reg_x), R[32] * 4);
}

int _int (unsigned int IM26) {
	
	if (IM26 == 0) {
		// zera o PC
		R[32] = IM26;
	
		fprintf(output, "int %d\n", IM26);
		fprintf(output, "[S] CR = 0x%08X, PC = 0x%08X\n", 0, R[32] * 4);

		return 0;
	}
	return 1;
}

int main (int argc, char* argv[]) {
	
	input = fopen(argv[1], "r");
	output = fopen(argv[2], "w");
	
	mem = (uint32_t *) malloc(sizeof(uint32_t) * 128);
	
	if (input == NULL) {
        fprintf(output, "[SOFTWARE INTERRUPTION]\n");
        exit(1);
    }
    int i = 0;
    while (fscanf(input, "%X", &mem[i]) != EOF) {
        i++;
    }
    
	fprintf(output, "[START OF SIMULATION]\n");

	int execute = 1;

	while (execute) {
		
		int desvio = 0;
		
		R[33] = (mem[R[32]] & 0xFC000000) >> 26;
		
		unsigned int IM26 = (mem[R[32]] & 0x3FFFFFF);
		unsigned int IM16 = (mem[R[32]] & 0x3FFFC00) >> 10;
		
		unsigned int Y = mem[R[32]] & 0x1F;
		unsigned int X = (mem[R[32]] & 0x3E0) >> 5;
		unsigned int Z = (mem[R[32]] & 0x7C00) >> 10;
		
		unsigned int Ey = (mem[R[32]] & 0x8000) >> 10;
		unsigned int Ex = (mem[R[32]] & 0x10000) >> 11;
		unsigned int Ez = (mem[R[32]] & 0x20000) >> 12;

		switch (R[33]) {
			case 0x00:
				_add(Ez + Z, Ex + X, Ey + Y);
				break;
			case 0x01:
				_addi(IM16, X, Y);
				break;
			case 0x02:
				_sub(Ez + Z, Ex + X, Ey + Y);
				break;
			case 0x03:
				_subi(IM16, X, Y);
				break;
			case 0x04:
				_mul(Ez + Z, Ex + X, Ey + Y);
				break;
			case 0x05:
				_muli(IM16, X, Y);
				break;
			case 0x06:
				_div(Ez + Z, Ex + X, Ey + Y);
				break;
			case 0x07:
				_divi(IM16, X, Y);
				break;
			case 0x08:
				_cmp(Ex + X, Ey + Y);
				break;
			case 0x09:
				_cmpi(X, IM16);
				break;
			case 0x0A:
				_shl(Ez + Z, Ex + X, Ey + Y);
				break;
			case 0x0B:
				_shr(Ez + Z, Ex + X, Ey + Y);
				break;
			case 0x0C:
				_and(Ez + Z, Ex + X, Ey + Y);
				break;
			case 0x0D:
				_andi(IM16, X, Y);
				break;
			case 0x0E:
				_not(Ex + X, Ey + Y);
				break;
			case 0x0F:
				_noti(IM16, X);
				break;
			case 0x10:
				_or(Ez + Z, Ex + X, Ey + Y);
				break;
			case 0x11:
				_ori(IM16, X, Y);
				break;
			case 0x12:
				_xor(Ez + Z, Ex + X, Ey + Y);
				break;
			case 0x13:
				_xori(IM16, X, Y);
				break;
			case 0x14:
				_ldw(IM16, X, Y);
				break;
			case 0x15:
				_ldb(IM16, X, Y);
				break;
			case 0x16:
				_stw(IM16, X, Y);
				break;
			case 0x17:
				_stb(IM16, X, Y);
				break;
			case 0x18:
				_push(Ex + X, Ey + Y);
				break;
			case 0x19:
				_pop(Ex + X, Ey + Y);
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
			case 0x25:
				_call(IM16, X, Y);
				desvio = 1;
				break;
			case 0x26:
				_ret(X);
				desvio = 1;
				break;
			case 0x3F:
				execute = _int(IM26);
				desvio = 1;
				break;
			default:
				fprintf(output, "[INVALID INSTRUCTION @ 0x%08X]\n", R[32] * 4);
		}
				
		if (!desvio)
			R[32]++;
	}
	
	fprintf(output, "[END OF SIMULATION]\n");
	
	fclose(input);
	fclose(output);
	
	return 0;
}
