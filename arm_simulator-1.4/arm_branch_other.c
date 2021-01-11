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
#include "arm_branch_other.h"
#include "arm_constants.h"
#include "util.h"
#include "registers.h"
#include <debug.h>
#include <stdlib.h>


int arm_branch(arm_core p, uint32_t ins) {
    uint8_t stock = get_bit(ins,24);

    uint32_t offset = ins & 0xFFFFFF;
    if(get_bit(offset, 23)) {   // Negative offset
        offset |= 0xFF000000;        
    }
    offset <<= 2;    
    
    uint32_t write_pc = arm_read_register(p,PC);

    if(stock == 1){
        arm_write_register(p,R14,write_pc);
        arm_write_register(p,PC,write_pc + offset);
    }

    else{
        arm_write_register(p,PC,write_pc + offset);
    }

    return 0;
}

int arm_coprocessor_others_swi(arm_core p, uint32_t ins) {
    if (get_bit(ins, 24)) {
        /* Here we implement the end of the simulation as swi 0x123456 */
        if ((ins & 0xFFFFFF) == 0x123456)
            exit(0);
        return SOFTWARE_INTERRUPT;
    } 
    return UNDEFINED_INSTRUCTION;
}

int arm_miscellaneous(arm_core p, uint32_t ins) {
    uint8_t fetch = ins & 0xF;
    uint8_t init = get_bit(ins,0);
    
    if(init == 0){
        uint32_t write_in = arm_read_register(p,fetch);
        arm_write_register(p,PC,write_in - 4);
    }
    else{
        printf("THUMB instructions are not handle in this version");
        return UNDEFINED_INSTRUCTION;
    }
    
    return 0;
}
