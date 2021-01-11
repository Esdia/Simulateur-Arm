/*
Armator - simulateur de jeu d'instruction ARMv5T � but p�dagogique
Copyright (C) 2011 Guillaume Huard
Ce programme est libre, vous pouvez le redistribuer et/ou le modifier selon les
termes de la Licence Publique G�n�rale GNU publi�e par la Free Software
Foundation (version 2 ou bien toute autre version ult�rieure choisie par vous).

Ce programme est distribu� car potentiellement utile, mais SANS AUCUNE
GARANTIE, ni explicite ni implicite, y compris les garanties de
commercialisation ou d'adaptation dans un but sp�cifique. Reportez-vous � la
Licence Publique G�n�rale GNU pour plus de d�tails.

Vous devez avoir re�u une copie de la Licence Publique G�n�rale GNU en m�me
temps que ce programme ; si ce n'est pas le cas, �crivez � la Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307,
�tats-Unis.

Contact: Guillaume.Huard@imag.fr
	 B�timent IMAG
	 700 avenue centrale, domaine universitaire
	 38401 Saint Martin d'H�res
*/
#include "arm_data_processing.h"
#include "arm_exception.h"
#include "arm_constants.h"
#include "arm_branch_other.h"
#include "util.h"
#include "debug.h"

/* Decoding functions for different classes of instructions */
int arm_data_processing_shift(arm_core p, uint32_t ins) {
	return UNDEFINED_INSTRUCTION;
}
	
int arm_data_processing_immediate_msr(arm_core p, uint32_t ins) {
    uint8_t typeVal = get_bit(ins, 25); // Imm/RM 
	uint8_t PSR = get_bit(ins, 22); // SPSR/CPSR 
	uint32_t val;

	if(typeVal){
		val = ins & 0xFF;
		uint8_t rotate = (ins >> 8) & 0xF;
		val = ror(val, rotate * 2);
	}else{
		val = arm_read_register(p, (ins & 0xF));
		arm_write_spsr(p, val);
	}
	
	if(PSR){
		arm_write_spsr(p, val);
	}else{
		arm_write_cpsr(p, val)	;
	}
	
	return 0;
}

int arm_data_nepunepu_immediate_mrs(arm_core p, uint32_t ins) {
    uint8_t Rd = (ins >> 12) & 0xF;
	uint8_t PSR = get_bit(ins, 22); // SPSR/CPSR 
	uint32_t val;

	if(PSR){
		val = arm_read_spsr(p);
	}else{
		val = arm_read_cpsr(p);
	}
	
	arm_write_register(p,Rd,val);

	return 0;
}

int arm_psr_transfer(arm_core p, uint32_t inst) {
	if(get_bit(inst, 21)) {
		return arm_data_processing_immediate_msr(p, inst);
	} else {
		return arm_data_nepunepu_immediate_mrs(p, inst);
	}
}

/* Valeur de retour : la retenue sortante (C) */
int get_op2(arm_core p, uint32_t inst, uint32_t* op2, int immediate_op) {
	int c = 0;
	
	if(immediate_op) {
		uint32_t immed8 = inst & 0xFF;
		uint8_t rotate = (inst >> 8) & 0xF;
		*op2 = ror(immed8, rotate * 2);

		if(rotate == 0) {
			c = get_flag(p, C);
		} else {
			c = get_bit(*op2, 31);
		}
	} else {
		// OP2 = registre
		*op2 = arm_read_register(p, (inst & 0xF));
		uint8_t shift_amount = 0;

		if(get_bit(inst, 4) && !get_bit(inst, 7)) {
			// Valeur shift = registre
			shift_amount = (uint8_t) arm_read_register(p, (inst >> 8) & 0xF);
		} else if(!get_bit(inst, 4)) {
			// Valeur shift = immediate
			shift_amount = (inst >> 7) & 0x1F;
		} else {
			// Ne devrait jamais arriver
			*op2 = -1;
			return 0;
		}

		switch ((inst >> 5) & 0b11) {
			case 0b00:	// Logical Left
				if(shift_amount == 0) {
					c = get_flag(p, C);
				} else {
					c = get_bit(*op2, 32 - shift_amount);
				}
				*op2 <<= shift_amount;
				break;
			case 0b01:	// Logical Right
				c = get_bit(*op2, shift_amount - 1);
				*op2 >>= shift_amount;
				break;
			case 0b10:	// Arithmetic Right
				c = get_bit(*op2, shift_amount - 1);
				*op2 = asr(*op2, shift_amount);
				break;
			case 0b11:	// Rotate Right
				*op2 = ror(*op2, shift_amount);
				c = get_bit(*op2, 31);
				break;	
			default:
				break;
		}
	}

	return c;
}

/* Décode et exécute une instruction de type data processing */
int arm_data_processing(arm_core p, uint32_t inst) {
	int immediate_op = get_bit(inst, 25); // Bit I
	int set = get_bit(inst, 20); // Bit S

	uint8_t rn = (inst >> 16) & 0xF; // Bits 16 à 19
	uint32_t op1 = arm_read_register(p, rn);

	uint8_t rd = (inst >> 12) & 0xF; // Bits 12 à 15

	uint32_t op2 = 0;
	int c = get_op2(p, inst, &op2, immediate_op);

	int op_code = (inst >> 21) & 0xF; // Bits 21 à 24

	uint64_t result = 0;
	uint32_t tmp = 0;
	switch (op_code) {
		case AND:
		case TST:
			result = op1 & op2;
			break;
		case EOR:
		case TEQ:
			result = op1 ^ op2;
			break;
		case RSB:
			tmp = op1;
			op1 = op2;
			op2 = tmp;
		case SUB:
		case CMP:
			result = op1 - op2;
			break;
		case ADD:
		case CMN:
			result = op1 + op2;
			break;
		case ADC:
			result = op1 + op2 + get_flag(p, C);
			break;
		case RSC:
			tmp = op1;
			op1 = op2;
			op2 = tmp;
		case SBC:
			result = op1 - op2 + get_flag(p, C) - 1;
			break;
		case ORR:
			result = op1 | op2;
			break;
		case MOV:
			result = op2;
			break;
		case BIC:
			result = op1 & ~op2;
			break;
		case MVN:
			result = ~op2;
			break;
	
		default:
			return 1;
	}

	if(op_code == TST || op_code == TEQ || op_code == CMP || op_code == CMN) {
		set = 1;
	} else {
		arm_write_register(p, rd, (uint32_t) result);
	}

	if(set) {
		int n = get_bit(result, 31);
		int z = (result == 0);

		int v = 0;
		if(op_code == SUB || op_code == RSB || op_code == ADD || op_code == ADC || op_code == SBC || op_code == RSC || op_code == CMP || op_code == CMN) {
			// Opérations arithmétiques
			c = get_bit(result, 32);

			if(op_code == ADD || op_code == ADC || op_code == CMN) {
				v = (get_bit(op1, 31) == get_bit(op2, 31)) && (get_bit(op1, 31) != c);
			} else {
				v = (get_bit(op1, 31) != get_bit(op2, 31)) && (get_bit(op2, 31) == c);
			}
		} else {
			v = get_flag(p, V);
		}

		set_flags(p, n, z, c, v);
	}

	return 0;
}

int arm_multiply(arm_core p, uint32_t ins){
    uint8_t rd = (ins >> 16) & 0xF;
    uint8_t rs = (ins >>8) & 0xF;
    uint8_t rm = ins & 0xF;
    uint32_t result;
    
    uint32_t opm = arm_read_register(p,rm);
    uint32_t ops = arm_read_register(p,rs);
    uint8_t n,z;

    if(rd == rm){
        printf("Rd and Rm register cannot be the same");
        return DATA_ABORT;
    }

    if(get_bit(ins,21)){
        uint8_t rn = (ins >> 12) & 0xF;
        uint32_t opn = arm_read_register(p,rn);
        result = opm * ops + opn;
    }

    else{
        result = opm * ops;
    }
    
    if(get_bit(ins,20)){
        n = get_bit(result,31);
        
        if(result == 0){
            z = 1;
        }
        else{
            z = 0;
        }
        set_flags(p,n,z,0,0);
    }
    arm_write_register(p,rd,result);
    return 0; 
}

int arm_multiply_long(arm_core p, uint32_t ins){
    
    uint8_t rdHi = (ins >> 16) & 0xF;
    uint8_t rdLo = (ins >>12) & 0xF;
    uint8_t rs = (ins >> 8) & 0xF;
    uint8_t rm = ins & 0xF;
    uint8_t n,z;

    if(rdHi == rm || rdHi == rdLo || rdLo == rm){
        printf("All registers must be the different");
        return DATA_ABORT;
    }

    if(get_bit(ins,22)){
        
        uint64_t result;
        uint32_t opm = arm_read_register(p,rm);
        uint32_t ops = arm_read_register(p,rs);
        uint32_t opHi = arm_read_register(p,rdHi);
        uint32_t opLo = arm_read_register(p,rdLo);
        uint64_t op_long = opLo | (opHi << 31);
    

        if(get_bit(ins,21)){
            result = opm * ops + op_long;
        }

        else{
            result = opm * ops;
        }
    
        if(get_bit(ins,20)){
            n = get_bit(result,31);
        
            if(result == 0){
                z = 1;
            }
            else{
                z = 0;
            }
            set_flags(p,n,z,0,0);
        }
        uint32_t resultLo = result;
        uint32_t resultHi = result >> 31;

        arm_write_register(p,rdLo,resultLo);
        arm_write_register(p,rdHi,resultHi);
    }
    else{
        int64_t result;
        int32_t opm = arm_read_register(p,rm);
        int32_t ops = arm_read_register(p,rs);
        int32_t opHi = arm_read_register(p,rdHi);
        int32_t opLo = arm_read_register(p,rdLo);
        int64_t op_long = opLo | (opHi << 31);
    
        if(get_bit(ins,21)){
            result = opm * ops + op_long;
        }

        else{
            result = opm * ops;
        }
    
        if(get_bit(ins,20)){
            n = get_bit(result,31);
        
            if(result == 0){
                z = 1;
            }
            else{
                z = 0;
            }
            set_flags(p,n,z,0,0);
        }
        int32_t resultLo = result;
        int32_t resultHi = result >> 31;

        arm_write_register(p,rdLo,resultLo);
        arm_write_register(p,rdHi,resultHi);
    }

    return 0;
}
