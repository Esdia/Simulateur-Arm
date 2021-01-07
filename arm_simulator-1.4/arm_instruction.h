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
#ifndef __ARM_INSTRUCTION_H__
#define __ARM_INSTRUCTION_H__
#include "arm_core.h"

#define EQ 0b0000
#define NE 0b0001
#define CS 0b0010
#define CC 0b0011
#define MI 0b0100
#define PL 0b0101
#define VS 0b0110
#define VC 0b0111
#define HI 0b1000
#define LS 0b1001
#define GE 0b1010
#define LT 0b1011
#define GT 0b1100
#define LE 0b1101
#define AL 0b1110

enum CatInst {
	DATA_PROCESSING,
	PSR_TRANSFER,
	MULTIPLY,
	MULTIPLY_LONG,
	SINGLE_DATA_SWAP,
	BRANCH_EXCHANGE,
	HALFWORD_DATA_TRANSFER,
	SINGLE_DATA_TRANSFER,
	UNDEFINED,
	BLOCK_DATA_TRANSFER,
	BRANCH,
	COPROCESSOR_DATA_TRANSFER,
	COPROCESSOR_DATA_OPERATION,
	COPROCESSOR_REGISTER_TRANSFER,
	SOFTWARE_INTERRUPT_INST	// Ajout de _INST pour faire la difference avec l'exception
};

int arm_step(arm_core p);

#endif
