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
#include "registers.h"
#include "arm_constants.h"
#include <stdlib.h>

#define nb_arm_registers 37

struct registers_data {
    uint32_t data[nb_arm_registers];
    size_t size;

    uint8_t mode;
};

registers registers_create() {
    registers r = malloc(sizeof(struct registers_data));
    r->size = nb_arm_registers;
    r->mode = USR;
    return r;
}

void registers_destroy(registers r) {
    free(r);
}

uint8_t get_mode(registers r) {
    return r->mode;
} 

int current_mode_has_spsr(registers r) {
    return r->mode != USR && r->mode != SYS;
}

int in_a_privileged_mode(registers r) {
    return r->mode != USR;
}

/*
    Cette fonction utilise le mode et le numéro de registre pour
    retrouver le bon indice dans le tableau data de r
*/
uint8_t get_index_by_mode(registers r, uint8_t reg) {
    switch (reg) {
    case R0: // Unbanked register
    case R1: // Unbanked register
    case R2: // Unbanked register
    case R3: // Unbanked register
    case R4: // Unbanked register
    case R5: // Unbanked register
    case R6: // Unbanked register
    case R7: // Unbanked register
    case PC:
    case CPSR:
        return reg;

    case SPSR: {
        switch (get_mode(r)) {
            case FIQ:
                return SPSR_fiq;
            case IRQ:
                return SPSR_irq;
            case UND:
                return SPSR_und;
            case ABT:
                return SPSR_abt;
            case SVC:
                return SPSR_svc;
            default: // USR ou SYS
                return R_Indefini;
        }
        break;
    }
    
    case R8:
    case R9:
    case R10:
    case R11:
    case R12: {
        // Les registres R8 à R12 ne sont remplacés que pour le mode FIQ
        if(get_mode(r) != FIQ) return reg;
        return R8_fiq + (reg - R8);
    }

    case R13:
    case R14: {
        switch (get_mode(r)) {
            case FIQ:
                return R13_fiq + (reg - R13);
            case IRQ:
                return R13_irq + (reg - R13);
            case UND:
                return R13_und + (reg - R13);;
            case ABT:
                return R13_abt + (reg - R13);;
            case SVC:
                return R13_svc + (reg - R13);;
            default: // USR ou SYS
                return reg;
        }
        break;
    }
    
    default:
        return R_Indefini;
    }

    return R_Indefini; // Ne devrait jamais arriver car tous les case du switch font un return
}

uint32_t read_register(registers r, uint8_t reg) {
    uint8_t index = get_index_by_mode(r, reg);
    if(index == R_Indefini) return -1;

    return r->data[index];
}

uint32_t read_usr_register(registers r, uint8_t reg) {
    if(reg > CPSR) return -1; // En mode USR, il n'y a pas de SPSR

    return r->data[reg];
}

uint32_t read_cpsr(registers r) {
    return read_usr_register(r, CPSR);
}

uint32_t read_spsr(registers r) {
    // La fonction read_register gère déjà le cas où on ne peut
    // pas lire le SPSR
    return read_register(r, SPSR);
}

void write_register(registers r, uint8_t reg, uint32_t value) {
    uint8_t index = get_index_by_mode(r, reg);
    if(index == R_Indefini) return;

    r->data[index] = value;
}

void write_usr_register(registers r, uint8_t reg, uint32_t value) {
    if(reg > CPSR) return;

    r->data[reg] = value;
}

void write_cpsr(registers r, uint32_t value) {
    write_usr_register(r, CPSR, value);
}

void write_spsr(registers r, uint32_t value) {
    // La fonction write_register gère déjà le cas où on ne peut
    // pas écrire dans le SPSR
    write_register(r, SPSR, value);
}
