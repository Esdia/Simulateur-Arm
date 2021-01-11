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
#include "arm_instruction.h"
#include "arm_exception.h"
#include "arm_data_processing.h"
#include "arm_load_store.h"
#include "arm_branch_other.h"
#include "arm_constants.h"
#include "util.h"

int is_set(uint32_t cpsr_val, int flag) {
    return get_bit(cpsr_val, flag);
}

int is_clear(uint32_t cpsr_val, int flag) {
    return !get_bit(cpsr_val, flag);
}

/*
    Cette fonction utilise le champ cond (bits 28 à 31)
    de l'instruction pour savoir si celle-ci doit être
    exécutée ou non
*/
int check_cond(arm_core p, uint32_t inst) {
    int cond_field = (inst >> 28) & 0xF; // On récupère les bits 28 à 31

    uint32_t cpsr_val = arm_read_cpsr(p);

    switch (cond_field) {
    case EQ:
        return is_set(cpsr_val, Z);
    case NE:
        return is_clear(cpsr_val, Z);
    case CS:
        return is_set(cpsr_val, C);
    case CC:
        return is_clear(cpsr_val, C);
    case MI:
        return is_set(cpsr_val, N);
    case PL:
        return is_clear(cpsr_val, N);
    case VS:
        return is_set(cpsr_val, V);
    case VC:
        return is_clear(cpsr_val, V);
    case HI:
        return is_set(cpsr_val, C) && is_clear(cpsr_val, Z);
    case LS:
        return is_clear(cpsr_val, C) || is_set(cpsr_val, Z);
    case GE:
        return is_set(cpsr_val, N) == is_set(cpsr_val, V);
    case LT:
        return is_set(cpsr_val, N) != is_set(cpsr_val, V);
    case GT:
        return is_clear(cpsr_val, Z) && (is_set(cpsr_val, N) == is_set(cpsr_val, V));
    case LE:
        return is_set(cpsr_val, Z) || is_set(cpsr_val, N) != is_set(cpsr_val, V);
    case AL:
        return 1;
    default: // Ne devrait jamais arriver
        return 0;
    }
}

int get_category_inst(uint32_t inst) {
    if(get_bit(inst, 27)) {
        if(get_bit(inst, 26)) {
            if(get_bit(inst, 25)) {
                if(get_bit(inst, 24)) {
                    return SOFTWARE_INTERRUPT_INST;
                } else {
                    if(get_bit(inst, 4)) {
                        return COPROCESSOR_REGISTER_TRANSFER;
                    } else {
                        return COPROCESSOR_DATA_OPERATION;
                    }
                }
            } else {
                return COPROCESSOR_DATA_TRANSFER;
            }
        } else {
            if(get_bit(inst, 25)) {
                return BRANCH;
            } else {
                return BLOCK_DATA_TRANSFER;
            }
        }
    } else {
        if(get_bit(inst, 26)) {
            if(get_bit(inst, 25) && get_bit(inst, 4)) {
                return UNDEFINED;
            } else {
                return SINGLE_DATA_TRANSFER;
            }
        } else {
            if(get_bit(inst, 24) && !get_bit(inst, 23) && !get_bit(inst, 20)) {
                if(get_bit(inst, 25) || !get_bit(inst, 4)) {
                    return PSR_TRANSFER;
                } else {
                    if(get_bit(inst, 6) || get_bit(inst, 5)) {
                        return HALFWORD_DATA_TRANSFER;
                    } else {
                        if(get_bit(inst, 21)) {
                            return BRANCH_EXCHANGE;
                        } else {
                            return SINGLE_DATA_SWAP;
                        }
                    }
                }
            } else {
                if(get_bit(inst, 25) || !(get_bit(inst, 7) && get_bit(inst, 4))) {
                    return DATA_PROCESSING;
                } else {
                    if(get_bit(inst, 6) || get_bit(inst, 5)) {
                        return HALFWORD_DATA_TRANSFER;
                    } else {
                        if(get_bit(inst, 23)) {
                            return MULTIPLY_LONG;
                        } else {
                            return MULTIPLY;
                        }
                    }
                }
            }
        }
    }
}

static int arm_execute_instruction(arm_core p) {
    uint32_t inst;
    arm_fetch(p, &inst);

    printf("%x\n", inst);   // TMP

    if(!check_cond(p, inst)) return 0;

    int e = 0;

    switch (get_category_inst(inst)) {
        case DATA_PROCESSING:
            printf("DATA_PROCESSING\n");
            e = arm_data_processing(p, inst);
            break;
        case PSR_TRANSFER:
            printf("PSR_TRANSFER\n");
            e = arm_psr_transfer(p, inst);
            break;
        case MULTIPLY:
            printf("MULTIPLY\n");
            break;
        case MULTIPLY_LONG:
            printf("MULTIPLY_LONG\n");
            break;
        case SINGLE_DATA_SWAP:
            printf("SINGLE_DATA_SWAP\n");
            break;
        case BRANCH_EXCHANGE:
            printf("BRANCH_EXCHANGE\n");
            e = arm_miscellaneous(p, inst);
            break;
        case HALFWORD_DATA_TRANSFER:
            printf("HALFWORD_DATA_TRANSFER\n");
            e = arm_load_store_halfword(p, inst);
            break;
        case SINGLE_DATA_TRANSFER:
            printf("SINGLE_DATA_TRANSFER\n");
            break;
        case UNDEFINED:
            printf("UNDEFINED\n");
            break;
        case BLOCK_DATA_TRANSFER:
            printf("BLOCK_DATA_TRANSFER\n");
            break;
        case BRANCH:
            printf("BRANCH\n");
            e = arm_branch(p,inst);
            break;
        case COPROCESSOR_DATA_TRANSFER:
            printf("COPROCESSOR_DATA_TRANSFER\n");
            break;
        case COPROCESSOR_DATA_OPERATION:
            printf("COPROCESSOR_DATA_OPERATION\n");
            break;
        case COPROCESSOR_REGISTER_TRANSFER:
            printf("COPROCESSOR_REGISTER_TRANSFER\n");
            break;
        case SOFTWARE_INTERRUPT_INST:
            printf("SOFTWARE_INTERRUPT_INST\n");
            e = arm_coprocessor_others_swi(p, inst);
            break;
        
        default:    // Ne devrait jamais arriver
            break;
    }

    return e;
}

int arm_step(arm_core p) {
    int result;

    result = arm_execute_instruction(p);
    if (result)
        arm_exception(p, result);
    return result;
}
