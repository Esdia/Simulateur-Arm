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
#include <stdlib.h>
#include "memory.h"
#include "util.h"

/*
  La mémoire sera représentée comme ceci:
    - Un tableau d'entiers
      - Chaque case du tableau correspond à un octet.
      - L'adresse d'un octet dans la mémoire est l'indice de sa case dans le
        tableau
      - La taille de la mémoire est le nombre d'octets qui la compose, donc
        la taille du tableau
    - Un entier qui correspond à la taille de la mémoire
    - Un entier qui indique si la mémoire est en big ou little endian
*/
struct memory_data {
    uint8_t* data;
    size_t size;
    int is_big_endian;
};

memory memory_create(size_t size, int is_big_endian) {
    memory mem = malloc(sizeof(struct memory_data));
    
    mem->size = size;
    mem->data = malloc(size * sizeof(uint8_t));
    
    mem->is_big_endian = is_big_endian;
    return mem;
}

size_t memory_get_size(memory mem) {
    return mem->size;
}

void memory_destroy(memory mem) {
    free(mem->data);
    free(mem);
}

int memory_read_byte(memory mem, uint32_t address, uint8_t *value) {
    if(address >= memory_get_size(mem)) return -1;

    *value = mem->data[address];
    return 1;
}

int memory_read_half(memory mem, uint32_t address, uint16_t *value) {
    // On fait -1 car on lit l'octet à address, et celui d'après
    if(address >= memory_get_size(mem) - 1) return -1;

    // Un half-word s'écrit sur une adresse multiple de 2
    if(address % 2 != 0) return -1;

    if(mem->is_big_endian) {
        *value = ((uint16_t) mem->data[address]) << 8 | mem->data[address+1];
    } else {
        *value = ((uint16_t) mem->data[address+1]) << 8 | mem->data[address];
    }
    return 1;
}

int memory_read_word(memory mem, uint32_t address, uint32_t *value) {
    // On fait -3 car on lit l'octet à address, et les trois suivants
    if(address >= memory_get_size(mem) - 3) return -1;

    // Un word s'écrit sur une adresse multiple de 4
    if(address % 4 != 0) return -1;

    if(mem->is_big_endian) {
        *value = (mem->data[address] << 24) | (mem->data[address+1] << 16) | (mem->data[address+2] << 8) | (mem->data[address+3]);
    } else {
        *value = (mem->data[address+3] << 24) | (mem->data[address+2] << 16) | (mem->data[address+1] << 8) | (mem->data[address]);
    }
    return 1;
}

int memory_write_byte(memory mem, uint32_t address, uint8_t value) {
    if(address >= memory_get_size(mem)) return 1;

    mem->data[address] = value;
    return 0;
}

int memory_write_half(memory mem, uint32_t address, uint16_t value) {
    // On fait -1 car on écrit sur l'octet à address, et celui d'après
    if(address >= memory_get_size(mem) - 1) return 1;

    // Un half-word s'écrit sur une adresse multiple de 2
    if(address % 2 != 0) return 1;

    uint16_t mask = 0x00FF;

    if(mem->is_big_endian) {
        mem->data[address] = value >> 8;
        mem->data[address+1] = value & mask;
    } else {
        mem->data[address+1] = value >> 8;
        mem->data[address] = value & mask;
    }
    return 0;
}

int memory_write_word(memory mem, uint32_t address, uint32_t value) {
    // On fait -3 car on lit l'octet à address, et les trois suivants
    if(address >= memory_get_size(mem) - 3) return 1;

    // Un word s'écrit sur une adresse multiple de 4
    if(address % 4 != 0) return 1;

    uint32_t mask = 0x000000FF;

    if(mem->is_big_endian) {
        mem->data[address] = value >> 24;
        mem->data[address+1] = value >> 16 & mask;
        mem->data[address+2] = value >> 8 & mask;
        mem->data[address+3] = value & mask;
    } else {
        mem->data[address+3] = value >> 24;
        mem->data[address+2] = value >> 16 & mask;
        mem->data[address+1] = value >> 8 & mask;
        mem->data[address] = value & mask;
    }
    return 0;
}
