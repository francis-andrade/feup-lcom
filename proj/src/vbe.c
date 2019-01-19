#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>

#include "vbe.h"
#include "lmlib.h"


#define PB2BASE(x) (((x) >> 4) & 0x0F000)
#define PB2OFF(x) ((x) & 0x0FFFF)

int vbe_get_mode_info(unsigned short mode, vbe_mode_info_t *vmi_p) {

	mmap_t map;//virtual memory map
	if(lm_alloc(sizeof(vbe_mode_info_t), &map)==NULL){
		return 1;
	}

	struct reg86u reg;//cpu register used to send the command to vbe

	reg.u.b.ah=VBE_FUNCTION_INVOKE;
	reg.u.b.al=VBE_RETURN_MODE;
	reg.u.w.cx=mode;
	reg.u.w.di=PB2OFF(map.phys);
	reg.u.w.es=PB2BASE(map.phys);
	reg.u.b.intno=VBE_INTERRUPT;
	if(sys_int86(&reg)!=OK){
		lm_free(&map);
		return 1;
	}
	else if(reg.u.b.al==VBE_FUNCTION_SUPPORTED && reg.u.b.ah==VBE_FUNCTION_SUCCESS){
		*vmi_p=*((vbe_mode_info_t *)map.virtual);
		lm_free(&map);
		return 0;
	}
	else{
		lm_free(&map);
		return 1;
	}

}

int vbe_get_info_block(vbe_info_block_t * vmi_p){
	mmap_t map;//virtual memory map
	if(lm_alloc(sizeof(vbe_info_block_t), &map)==NULL){
		return 1;
	}

	struct reg86u reg;//cpu register used to send the command to vbe

	reg.u.b.ah=VBE_FUNCTION_INVOKE;
	reg.u.b.al=VBE_RETURN_CONTROLLER;
	reg.u.w.di=PB2OFF(map.phys);
	reg.u.w.es=PB2BASE(map.phys);
	reg.u.b.intno=VBE_INTERRUPT;
	if(sys_int86(&reg)!=OK){
		lm_free(&map);
		return 1;
	}
	else if(reg.u.b.al==VBE_FUNCTION_SUPPORTED && reg.u.b.ah==VBE_FUNCTION_SUCCESS){
		*vmi_p=*((vbe_info_block_t *)map.virtual);
		lm_free(&map);
		return 0;
	}
	else{
		lm_free(&map);
		return 1;
	}

}


