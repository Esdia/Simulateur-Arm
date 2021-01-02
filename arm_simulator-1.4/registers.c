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

/*
    Il y a 37 registres.
    Voici les correspondances indice - registre:
    0 à 14: R0 à R14
    15: PC
    16: CPSR
    17: R13_svc
    18: R14_svc
    19: SPSR_svc
    20: R13_abt
    21: R14_abt
    22: SPSR_abt
    23: R13_und
    24: R14_und
    25: SPSR_und
    26: R13_irq
    27: R14_irq
    28: SPSR_irq
    29: R8_fiq
    30: R9_fiq
    31: R10_fiq
    32: R11_fiq
    33: R12_fiq
    34: R13_fiq
    35: R14_fiq
    36: SPSR_fiq

    Pour toutes les fonctions dans lesquelles reg est passé en paramètre, il
    s'agira toujours d'un entier entre 0 et 17 (pour R0 à PC + CPSR + SPSR).
    C'est aux fonctions read_register et write_register de faire le lien entre
    le mode et le numéro de registre pour lire et écrire dans le bon registre.
*/
struct registers_data {
    uint32_t* data;
    size_t size;

    uint8_t mode;
};

registers registers_create() {
    registers r = malloc(sizeof(struct registers_data));

    r->size = 37; // 31 General purpose + 6 status
    r->data = malloc(r->size * sizeof(uint32_t));

    r->mode = USR;

    return r;
}

void registers_destroy(registers r) {
    free(r->data);
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
    On suppose que 0 <= reg <= 17
*/
uint8_t get_index_by_mode(registers r, uint8_t reg) {
    if(get_mode(r) == USR || get_mode(r) == SYS) return reg;

    if(reg <= 7) {
        // Unbanked registers
        return reg;
    } else if(reg == 15) {
        // PC
        return reg;
    } else if(reg == 16) {
        // CPSR
        return reg;
    } else if(reg == 17) {
        // SPSR
        switch (get_mode(r)) {
            case FIQ:
                return 36;
            case IRQ:
                return 28;
            case UND:
                return 25;
            case ABT:
                return 22;
            case SVC:
                return 19;            
            default:
                return -1; // Ne devrait jamais arriver
        }
    } else {
        // Banked registers
        if(reg <= 12) {
            // Les registres R8 à R12 ne sont remplacés que pour le mode FIQ
            if(get_mode(r) != FIQ) {
                return reg;
            } else {
                return reg + 21;
            }
        } else {
            switch (get_mode(r)) {
                case FIQ:
                    return reg + 21;
                case IRQ:
                    return reg + 13;
                case UND:
                    return reg + 10;
                case ABT:
                    return reg + 7;
                case SVC:
                    return reg + 4;
                default:
                    return -1; // Ne devrait jamais arriver
            }
        }
    }
}

/*
    Pour les correspondances entre mode/numéro de registre et l'indice
    utilisé, voir le commentaire ligne 27, sur struct registers
*/
uint32_t read_register(registers r, uint8_t reg) {
    if(reg > 17) return -1;
    if(reg == 17 && (!current_mode_has_spsr(r))) return -1;

    uint8_t index = get_index_by_mode(r, reg);
    return r->data[index];
}

uint32_t read_usr_register(registers r, uint8_t reg) {
    if(reg > 16) return -1; // En mode USR, il n'y a pas de SPSR

    return r->data[reg];
}

uint32_t read_cpsr(registers r) {
    return read_usr_register(r, 16);
}

uint32_t read_spsr(registers r) {
    // La fonction write_register gère déjà le cas où on ne peut
    // pas lire le SPSR
    return read_register(r, 17);
}

void write_register(registers r, uint8_t reg, uint32_t value) {
    if(reg > 17) return;
    if(reg == 17 && (!current_mode_has_spsr(r))) return;

    uint8_t index = get_index_by_mode(r, reg);
    r->data[index] = value;
}

void write_usr_register(registers r, uint8_t reg, uint32_t value) {
    if(reg > 16) return;
    r->data[reg] = value;
}

void write_cpsr(registers r, uint32_t value) {
    write_usr_register(r, 16, value);
}

void write_spsr(registers r, uint32_t value) {
    // La fonction write_register gère déjà le cas où on ne peut
    // pas écrire dans le SPSR
    write_register(r, 17, value);
}
