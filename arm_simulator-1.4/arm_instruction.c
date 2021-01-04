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

int is_set(arm_core p, int flag) {
    uint32_t val = arm_read_cpsr(p);

    return get_bit(val, flag);
}

int is_clear(arm_core p, int flag) {
    return !is_set(p, flag);
}

/*
    Cette fonction utilise le champ cond (bits 28 à 31)
    de l'instruction pour savoir si celle-ci doit être
    exécutée ou non
*/
int check_cond(arm_core p, uint32_t inst) {
    int cond_field = (inst >> 28) & 0xF; // On récupère les bits 28 à 31

    switch (cond_field) {
    case EQ:
        return is_set(p, Z);
    case NE:
        return is_clear(p, Z);
    case CS:
        return is_set(p, C);
    case CC:
        return is_clear(p, C);
    case MI:
        return is_set(p, N);
    case PL:
        return is_clear(p, N);
    case VS:
        return is_set(p, V);
    case VC:
        return is_clear(p, V);
    case HI:
        return is_set(p, C) && is_clear(p, Z);
    case LS:
        return is_clear(p, C) || is_set(p, Z);
    case GE:
        return is_set(p, N) == is_set(p, V);
    case LT:
        return is_set(p, N) != is_set(p, V);
    case GT:
        return is_clear(p, Z) && (is_set(p, N) == is_set(p, V));
    case LE:
        return is_set(p, Z) || is_set(p, N) != is_set(p, V);
    case AL:
        return 1;
    default: // Ne devrait jamais arriver
        return 0;
    }
}

static int arm_execute_instruction(arm_core p) {
    uint32_t inst;
    arm_fetch(p, &inst);

    printf("%x\n", inst);  // TMP

    if(!check_cond(p, inst)) return 0;

    if(!get_bit(inst, 27)) {
        if(!get_bit(inst, 26)) {
            if((((inst >> 20) & 0x1F) == 0b10010) && !get_bit(inst, 7)) {
                printf("BRANCH AND EXCHANGE\n");
            } else if(get_bit(inst, 25) || !(get_bit(inst, 7) && get_bit(inst, 4))) {
                printf("DATA PROCESSING / PSR TRANSFER\n");
            } else if(get_bit(inst, 6) || get_bit(inst, 5)) {
                printf("HALFWORD DATA TRANSFER\n");
            } else if(get_bit(inst, 24)) {
                printf("SINGLE DATA SWAP\n");
            } else if(get_bit(inst, 23)) {
                printf("MULTIPLY LONG\n");
            } else {
                printf("MULTIPLY\n");
            }
        } else {
            if(get_bit(inst, 25) && get_bit(inst, 4)) {
                printf("UNDEFINED\n");
            } else {
                printf("SINGLE DATA TRANSFER\n");
            }
        }
    } else {
        if(!get_bit(inst, 26)) {
            if(!get_bit(inst, 25)) {
                printf("BLOCK DATA TRANSFER\n");
            } else {
                printf("BRANCH\n");
            }
        } else {
            if(!get_bit(inst, 25)) {
                printf("COPROCESSOR DATA TRANSFER\n");
            } else {
                if(!get_bit(inst, 24)) {
                    if(!get_bit(inst, 4)) {
                        printf("COPROCESSOR DATA OPERATION\n");
                    } else {
                        printf("COPROCESSOR REGISTER TRANSFER\n");
                    }
                } else {
                    printf("SOFTWARE INTERRUPT\n");
                    return arm_coprocessor_others_swi(p, inst);
                }
            }
        }
    }

    return 0;
}

int arm_step(arm_core p) {
    int result;

    result = arm_execute_instruction(p);
    if (result)
        arm_exception(p, result);
    return result;
}
