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
#include "arm_exception.h"
#include "arm_constants.h"
#include "arm_core.h"
#include "util.h"

#include <stdlib.h>

// Not supported below ARMv6, should read as 0
#define CP15_reg1_EEbit 0

#define Exception_bit_9 (CP15_reg1_EEbit << 9)

void arm_exception(arm_core p, unsigned char exception) {
    /* We only support RESET initially */
    /* Semantics of reset interrupt (ARM manual A2-18) */
    if (exception == RESET) {
        arm_write_cpsr(p, 0x1d3 | Exception_bit_9);
	    arm_write_usr_register(p, 15, 0);
    } else if(exception == UNDEFINED_INSTRUCTION) {
        exit(1);  
    } else if(exception == SOFTWARE_INTERRUPT) {
        exit(0);
    }
}
