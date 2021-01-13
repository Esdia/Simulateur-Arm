/*
Armator - simulateur de jeu d'instruction ARMv5T à but pédagogique
Copyright (C) 2011 Guillaume Huard
Ce programme est libre, vous pouvez le redistribuer et/ou le modifier selon les
termes de la Licence Publique Générale GNU publiée par la Free Software
Foundation (version 2 ou bien toute autre version ultérieure choisie par vous).

Ce programme est distribué car potentiellement utile, mais SANS AUCUNE
GARANTIE, ni explicite ni implicite, y compris les garanties de
commercialisation ou d'adaptation dans un but spécifique. Reportez-vous à la
Licence Publique Générale GNU pour plus de détails.

Vous devez avoir reçu une copie de la Licence Publique Générale GNU en même
temps que ce programme ; si ce n'est pas le cas, écrivez à la Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307,
États-Unis.

Contact: Guillaume.Huard@imag.fr
	 Bâtiment IMAG
	 700 avenue centrale, domaine universitaire
	 38401 Saint Martin d'Hères
*/
#include "arm_load_store.h"
#include "arm_exception.h"
#include "arm_constants.h"
#include "util.h"
#include "debug.h"

#define CP15_reg1_Ubit 0 //Non word-aligned addresses n'est pas supporté dans notre implémentation donc valeur à 0
#define UNPREDICTABLE 0xFF 

int arm_load_store(arm_core p, uint32_t inst) {

  
  uint8_t decalage = inst & 0xFFF ; // 0 à 11  decalage
  uint8_t Rd = (inst >> 12 ) & 0xF ; // 12 à 15 registre 1 
  uint8_t Rn = (inst >> 16 ) & 0xF ; // 16 à 19 registre 2
  uint8_t L = get_bit(inst, 20); // 20 Load/store  
  uint8_t W = get_bit(inst, 21); // 21 si w = 1 récrit la valeur dans celui de depart aprés le decalage
  uint8_t B = get_bit(inst, 22); // 22 loadB ou load / store ou storeB  
  uint8_t U = get_bit(inst, 23); // 23 u = 1 u = 0 
  uint8_t P = get_bit(inst, 24); // 24 p decalage avant = 1 ou apres = 0 le transfer  si P = 0 alors w  = 0 
  uint8_t I = get_bit(inst, 25); // 25 il fait des trucs 
  // si load Rd source sinon destination 
  
  
 
  uint32_t rlu = arm_read_register(p,Rn);

  uint8_t offset ;
  if ( I == 0 ) {
    offset = decalage;

     }
  else {
      uint8_t Rm = inst & 0xF ;
      uint32_t valeur = arm_read_register (p,Rm);
      switch(decalage>>5 && 3 ){
         case '0': 
           offset = (valeur>>(decalage>>7));
           break ;
         case '1': 
           offset =(valeur<<(decalage>>7));
           break ;
         case '2': 
           offset = asr(valeur,(decalage>>7));
           break ;
         case '3': 
           offset = ror(valeur,(decalage>>7));
           break ;
      }
    }

 if  ( U == 0){
    offset = - offset ;
   }
 
 if (P == 1 ) {
   rlu = rlu  + offset ;
   joker_arsene (rlu,L,B,Rd,p);
   if (W == 1 ) {
      arm_write_register(p,Rn,rlu);
    }
  }

  else {
  joker_arsene (rlu,L,B,Rd,p);
  rlu = rlu + offset ;
  arm_write_register(p,Rn,rlu);  
  }
  return 0 ;    
}

void joker_arsene ( uint32_t rlu , int L , int B  , uint8_t rd , arm_core p){
  uint32_t  val ;
  if (L == 1 ){
    if ( B == 0 ) {
      arm_read_word(p,rlu, &val);
    }
    else {
      arm_read_byte(p,rlu,(uint8_t*)&val);
    }
    arm_write_register(p,rd,val);
  }
  else {
    val = arm_read_register(p,rd) ;
    if ( B == 0) {
      if (rlu % 4 != 0) {
          rlu = rlu & 0xFFFFFFFC;
        }
      arm_write_word(p,rlu,val);  
      }
    else {
      arm_write_byte(p,rlu,val);
    }
  } 
}

int arm_load_store_halfword(arm_core p, uint32_t ins) {
    uint16_t data;
    uint8_t rn = (ins >> 16) & 0xF;
    uint8_t rd = (ins >> 12) & 0xF;
    //int I = get_bit(ins, 22);
    int L = get_bit(ins, 20);
#if 0
    uint8_t addr_mode;
    if (I == 0) {
      addr_mode = ins & 0xF;
    } else {
      addr_mode = (ins & 0xF)  & ((ins >> 4) & 0xF) ;
    }
#endif
    //MemoryAccess(B-bit, E-bit)
    uint32_t address = arm_read_register(p, rn); // ?? [address,2]
    if (L==1) {
      if (CP15_reg1_Ubit == 0) {
       if (1) { // if (address[0] == 0) {
         // data = Memory[address,2] 
         arm_read_half(p, address, &data);
       } else {
         data = UNPREDICTABLE ;
       }
     } else { /* CP15_reg1_Ubit == 1 */
       //data = Memory[address,2]
       arm_read_half(p, address, &data) ;
     }
     //Rd = ZeroExtend(data[15:0])
     arm_write_register( p, rd, data);
    } else {
      //processor_id = ExecutingProcessor()
      data =  arm_read_register(p, rd);
      if (CP15_reg1_Ubit == 0) {
           arm_write_half( p, address, data);    
      }else{ /* CP15_reg1_Ubit ==1 */
        arm_write_half( p, address, data);
      }
      // Ceci est nécessaire pour les mémoires partagées non implémentées
      //if (Shared(address)) { /* ARMv6 */
        //physical_address = TLB(address);
        //ClearExclusiveByAddress(physical_address,processor_id,2); }
    }
    return 0;
}

int arm_load_store_multiple(arm_core p, uint32_t ins) {
    uint32_t data;
    uint16_t rn = (ins >> 16) & 0xF;
    int offset_pre=0;
    int offset_post=4;
    int i =0;
    if(rn==15){
      return UNDEFINED_INSTRUCTION;
    }
    int P = get_bit(ins, 24);
    int U = get_bit(ins, 23);
    //int S = get_bit(ins, 22);
    int W = get_bit(ins, 21);
    int L = get_bit(ins, 20);
    if(P==1){
      offset_pre=4;
      offset_post=0;
    }
    if(U==0){
      offset_post=-offset_post;
      offset_pre=-offset_pre;
    }
    uint32_t address = arm_read_register(p, rn);
    address&=0xFFFFFFFC;
    if(L==1){ //load
      for(i=0;i<15;i++){
        if(get_bit(ins, i) == 1){
          address=address+offset_pre;
          arm_read_word(p, address, &data);
          arm_write_register(p, i, data);
          address=address+offset_post;
        }
      }
    } else { //store
      for(i=0;i<16;i++){
        if(get_bit(ins, i) == 1){
          address=address+offset_pre;
          data = arm_read_register(p, i);
          arm_write_word(p, address, data);
          address=address+offset_post;          
        }
      }
    }
    if(W==1){
      arm_write_register(p, rn, address);
    }
    return 0;
}

int arm_coprocessor_load_store(arm_core p, uint32_t ins) {
    /* Not implemented */
    return UNDEFINED_INSTRUCTION;
}
