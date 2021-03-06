/****************************************************************************
*
*						Realmode X86 Emulator Library
*
*            	Copyright (C) 1996-1999 SciTech Software, Inc.
* 				     Copyright (C) David Mosberger-Tang
* 					   Copyright (C) 1999 Egbert Eich
*
*  ========================================================================
*
*  Permission to use, copy, modify, distribute, and sell this software and
*  its documentation for any purpose is hereby granted without fee,
*  provided that the above copyright notice appear in all copies and that
*  both that copyright notice and this permission notice appear in
*  supporting documentation, and that the name of the authors not be used
*  in advertising or publicity pertaining to distribution of the software
*  without specific, written prior permission.  The authors makes no
*  representations about the suitability of this software for any purpose.
*  It is provided "as is" without express or implied warranty.
*
*  THE AUTHORS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
*  INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
*  EVENT SHALL THE AUTHORS BE LIABLE FOR ANY SPECIAL, INDIRECT OR
*  CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
*  USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
*  OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
*  PERFORMANCE OF THIS SOFTWARE.
*
*  ========================================================================
*
* Language:		ANSI C
* Environment:	Any
* Developer:    Kendall Bennett
*
* Description:  This file includes subroutines to implement the decoding
*               and emulation of all the x86 extended two-byte processor
*               instructions.
*
****************************************************************************/

#include "x86emu/x86emui.h"

/*----------------------------- Implementation ----------------------------*/

/****************************************************************************
PARAMETERS:
op1 - Instruction op code

REMARKS:
Handles illegal opcodes.
****************************************************************************/
static void x86emuOp2_illegal_op(X86EMU_sysEnv *env, u8 op2)
{    
    START_OF_INSTR();
    if (!env->illfuncs.ill_op2 || !env->illfuncs.ill_op2(env, op2)) {
	DECODE_PRINTF("ILLEGAL EXTENDED X86 OPCODE\n");
	TRACE_REGS();
	printf("%04x:%04x: %02X ILLEGAL EXTENDED X86 OPCODE!\n",
	       env->x86.R_CS, env->x86.R_IP-2,op2);
	HALT_SYS();
    }
    DECODE_CLEAR_SEGOVR();
    END_OF_INSTR();
}

/****************************************************************************
REMARKS:
Handles opcode 0x0f,0x80-0x8F
****************************************************************************/
static void x86emuOp2_long_jump(X86EMU_sysEnv *env, u8 op2)
{
    s32 target;

    /* conditional jump to word offset. */
    START_OF_INSTR();
    DECODE_PRINTF2("J%s\t", x86emu_decode_cond_name(op2));
    if (env->x86.mode & SYSMODE_PREFIX_DATA) {
    	target = fetch_long_imm();
    } else {
	target = (s16) fetch_word_imm();
    }
    target += (s16) env->x86.R_IP;
    DECODE_PRINTF2("%04x\n", target);
    TRACE_AND_STEP();
    if (x86emu_decode_test_cond(env, op2))
	env->x86.R_IP = (u16)target;
    DECODE_CLEAR_SEGOVR();
    END_OF_INSTR();
}

/****************************************************************************
REMARKS:
Handles opcode 0x0f,0x90-0x9F
****************************************************************************/
static void x86emuOp2_set_byte(X86EMU_sysEnv *env, u8 op2)
{
    int mod, rl, rh;
    u32 destoffset;
    u8  *destreg;
    int cond;

    START_OF_INSTR();
    DECODE_PRINTF2("SET%s\t", x86emu_decode_cond_name(op2));
    FETCH_DECODE_MODRM(mod, rh, rl);
    cond = x86emu_decode_test_cond(env, op2);
    if (mod != 3) {
        destoffset = decode_rm_address(mod, rl);
	DECODE_PRINTF("\n");
        TRACE_AND_STEP();
        store_data_byte(destoffset, cond ? 0x01 : 0x00);
    } else {                     /* register to register */
        destreg = DECODE_RM_BYTE_REGISTER(rl);
	DECODE_PRINTF("\n");
        TRACE_AND_STEP();
        *destreg = cond ? 0x01 : 0x00;
    }
    DECODE_CLEAR_SEGOVR();
    END_OF_INSTR();
}

/****************************************************************************
REMARKS:
Handles opcode 0x0f,0xa0
****************************************************************************/
static void x86emuOp2_push_FS(X86EMU_sysEnv *env, u8 op2)
{
    START_OF_INSTR();
    if (env->x86.mode & SYSMODE_PREFIX_DATA) {
	DECODE_PRINTF("PUSHD\tFS\n");
	TRACE_AND_STEP();
	push_long(env->x86.R_FS);

    } else {
	DECODE_PRINTF("PUSH\tFS\n");
	TRACE_AND_STEP();
	push_word(env->x86.R_FS);
    }
    DECODE_CLEAR_SEGOVR();
    END_OF_INSTR();
}

/****************************************************************************
REMARKS:
Handles opcode 0x0f,0xa1
****************************************************************************/
static void x86emuOp2_pop_FS(X86EMU_sysEnv *env, u8 op2)
{
    START_OF_INSTR();
    if (env->x86.mode & SYSMODE_PREFIX_DATA) {
	DECODE_PRINTF("POPD\tFS\n");
	TRACE_AND_STEP();
	env->x86.R_FS = pop_long();
    } else {
	DECODE_PRINTF("POP\tFS\n");
	TRACE_AND_STEP();
	env->x86.R_FS = pop_word();
    }
    DECODE_CLEAR_SEGOVR();
    END_OF_INSTR();
}

/****************************************************************************
REMARKS:
Handles opcode 0x0f,0xa3
****************************************************************************/
static void x86emuOp2_bt_R(X86EMU_sysEnv *env, u8 op2)
{
    int mod, rl, rh;
    u32 srcoffset;
    int bit,disp;

    START_OF_INSTR();
    DECODE_PRINTF("BT\t");
    FETCH_DECODE_MODRM(mod, rh, rl);
    if (mod != 3) {
        if (env->x86.mode & SYSMODE_PREFIX_DATA) {
            u32 srcval;
            u32 *shiftreg;

            srcoffset = decode_rm_address(mod, rl);
            DECODE_PRINTF(",");
            shiftreg = DECODE_RM_LONG_REGISTER(rh);
	    DECODE_PRINTF("\n");
            TRACE_AND_STEP();
            bit = *shiftreg & 0x1F;
            disp = (s16)*shiftreg >> 5;
            srcval = fetch_data_long(srcoffset+disp);
            CONDITIONAL_SET_FLAG(srcval & (0x1 << bit),F_CF);
        } else {
            u16 srcval;
            u16 *shiftreg;

            srcoffset = decode_rm_address(mod, rl);
            DECODE_PRINTF(",");
            shiftreg = DECODE_RM_WORD_REGISTER(rh);
	    DECODE_PRINTF("\n");
            TRACE_AND_STEP();
            bit = *shiftreg & 0xF;
            disp = (s16)*shiftreg >> 4;
            srcval = fetch_data_word(srcoffset+disp);
            CONDITIONAL_SET_FLAG(srcval & (0x1 << bit),F_CF);
        }
    } else {                     /* register to register */
        if (env->x86.mode & SYSMODE_PREFIX_DATA) {
            u32 *srcreg,*shiftreg;

            srcreg = DECODE_RM_LONG_REGISTER(rl);
            DECODE_PRINTF(",");
            shiftreg = DECODE_RM_LONG_REGISTER(rh);
	    DECODE_PRINTF("\n");
            TRACE_AND_STEP();
            bit = *shiftreg & 0x1F;
            CONDITIONAL_SET_FLAG(*srcreg & (0x1 << bit),F_CF);
        } else {
            u16 *srcreg,*shiftreg;

            srcreg = DECODE_RM_WORD_REGISTER(rl);
            DECODE_PRINTF(",");
            shiftreg = DECODE_RM_WORD_REGISTER(rh);
	    DECODE_PRINTF("\n");
            TRACE_AND_STEP();
            bit = *shiftreg & 0xF;
            CONDITIONAL_SET_FLAG(*srcreg & (0x1 << bit),F_CF);
        }
    }
    DECODE_CLEAR_SEGOVR();
    END_OF_INSTR();
}

/****************************************************************************
REMARKS:
Handles opcode 0x0f,0xa4
****************************************************************************/
static void x86emuOp2_shld_IMM(X86EMU_sysEnv *env, u8 op2)
{
    int mod, rl, rh;
    u32 destoffset;
	u8 shift;

    START_OF_INSTR();
    DECODE_PRINTF("SHLD\t");
    FETCH_DECODE_MODRM(mod, rh, rl);
    if (mod != 3) {
        if (env->x86.mode & SYSMODE_PREFIX_DATA) {
            u32 destval;
            u32 *shiftreg;

            destoffset = decode_rm_address(mod, rl);
            DECODE_PRINTF(",");
            shiftreg = DECODE_RM_LONG_REGISTER(rh);
            DECODE_PRINTF(",");
            shift = fetch_byte_imm();
            DECODE_PRINTF2("%d\n", shift);
            TRACE_AND_STEP();
            destval = fetch_data_long(destoffset);
            destval = shld_long(destval,*shiftreg,shift);
            store_data_long(destoffset, destval);
        } else {
            u16 destval;
            u16 *shiftreg;

            destoffset = decode_rm_address(mod, rl);
            DECODE_PRINTF(",");
            shiftreg = DECODE_RM_WORD_REGISTER(rh);
            DECODE_PRINTF(",");
            shift = fetch_byte_imm();
            DECODE_PRINTF2("%d\n", shift);
            TRACE_AND_STEP();
            destval = fetch_data_word(destoffset);
            destval = shld_word(destval,*shiftreg,shift);
            store_data_word(destoffset, destval);
        }
    } else {                     /* register to register */
        if (env->x86.mode & SYSMODE_PREFIX_DATA) {
            u32 *destreg,*shiftreg;

            destreg = DECODE_RM_LONG_REGISTER(rl);
            DECODE_PRINTF(",");
            shiftreg = DECODE_RM_LONG_REGISTER(rh);
            DECODE_PRINTF(",");
            shift = fetch_byte_imm();
            DECODE_PRINTF2("%d\n", shift);
            TRACE_AND_STEP();
            *destreg = shld_long(*destreg,*shiftreg,shift);
        } else {
            u16 *destreg,*shiftreg;

            destreg = DECODE_RM_WORD_REGISTER(rl);
            DECODE_PRINTF(",");
            shiftreg = DECODE_RM_WORD_REGISTER(rh);
            DECODE_PRINTF(",");
            shift = fetch_byte_imm();
            DECODE_PRINTF2("%d\n", shift);
            TRACE_AND_STEP();
            *destreg = shld_word(*destreg,*shiftreg,shift);
        }
    }
    DECODE_CLEAR_SEGOVR();
    END_OF_INSTR();
}

/****************************************************************************
REMARKS:
Handles opcode 0x0f,0xa5
****************************************************************************/
static void x86emuOp2_shld_CL(X86EMU_sysEnv *env, u8 op2)
{
    int mod, rl, rh;
    u32 destoffset;

    START_OF_INSTR();
    DECODE_PRINTF("SHLD\t");
    FETCH_DECODE_MODRM(mod, rh, rl);
    if (mod != 3) {
        if (env->x86.mode & SYSMODE_PREFIX_DATA) {
            u32 destval;
            u32 *shiftreg;

            destoffset = decode_rm_address(mod, rl);
            DECODE_PRINTF(",");
            shiftreg = DECODE_RM_LONG_REGISTER(rh);
            DECODE_PRINTF(",CL\n");
            TRACE_AND_STEP();
            destval = fetch_data_long(destoffset);
            destval = shld_long(destval,*shiftreg,env->x86.R_CL);
            store_data_long(destoffset, destval);
        } else {
            u16 destval;
            u16 *shiftreg;

            destoffset = decode_rm_address(mod, rl);
            DECODE_PRINTF(",");
            shiftreg = DECODE_RM_WORD_REGISTER(rh);
            DECODE_PRINTF(",CL\n");
            TRACE_AND_STEP();
            destval = fetch_data_word(destoffset);
            destval = shld_word(destval,*shiftreg,env->x86.R_CL);
            store_data_word(destoffset, destval);
        }
    } else {                     /* register to register */
        if (env->x86.mode & SYSMODE_PREFIX_DATA) {
            u32 *destreg,*shiftreg;

            destreg = DECODE_RM_LONG_REGISTER(rl);
            DECODE_PRINTF(",");
            shiftreg = DECODE_RM_LONG_REGISTER(rh);
            DECODE_PRINTF(",CL\n");
            TRACE_AND_STEP();
            *destreg = shld_long(*destreg,*shiftreg,env->x86.R_CL);
        } else {
            u16 *destreg,*shiftreg;

            destreg = DECODE_RM_WORD_REGISTER(rl);
            DECODE_PRINTF(",");
            shiftreg = DECODE_RM_WORD_REGISTER(rh);
            DECODE_PRINTF(",CL\n");
            TRACE_AND_STEP();
            *destreg = shld_word(*destreg,*shiftreg,env->x86.R_CL);
        }
    }
    DECODE_CLEAR_SEGOVR();
    END_OF_INSTR();
}

/****************************************************************************
REMARKS:
Handles opcode 0x0f,0xa8
****************************************************************************/
static void x86emuOp2_push_GS(X86EMU_sysEnv *env, u8 op2)
{
    START_OF_INSTR();
    if (env->x86.mode & SYSMODE_PREFIX_DATA) {
	DECODE_PRINTF("PUSHD\tGS\n");
	TRACE_AND_STEP();
	push_long(env->x86.R_GS);
    } else {
	DECODE_PRINTF("PUSH\tGS\n");
	TRACE_AND_STEP();
	push_word(env->x86.R_GS);
    }
    DECODE_CLEAR_SEGOVR();
    END_OF_INSTR();
}

/****************************************************************************
REMARKS:
Handles opcode 0x0f,0xa9
****************************************************************************/
static void x86emuOp2_pop_GS(X86EMU_sysEnv *env, u8 op2)
{
    START_OF_INSTR();
    if (env->x86.mode & SYSMODE_PREFIX_DATA) {
	DECODE_PRINTF("POPD\tGS\n");
	TRACE_AND_STEP();
	env->x86.R_GS = pop_long();
    } else {
	DECODE_PRINTF("POP\tGS\n");
	TRACE_AND_STEP();
	env->x86.R_GS = pop_word();
    }
    DECODE_CLEAR_SEGOVR();
    END_OF_INSTR();
}

/****************************************************************************
REMARKS:
Handles opcode 0x0f,0xab
****************************************************************************/
static void x86emuOp2_bts_R(X86EMU_sysEnv *env, u8 op2)
{
    int mod, rl, rh;
    u32 srcoffset;
    int bit,disp;

    START_OF_INSTR();
    DECODE_PRINTF("BTS\t");
    FETCH_DECODE_MODRM(mod, rh, rl);
    if (mod != 3) {
        if (env->x86.mode & SYSMODE_PREFIX_DATA) {
            u32 srcval,mask;
            u32 *shiftreg;

            srcoffset = decode_rm_address(mod, rl);
            DECODE_PRINTF(",");
            shiftreg = DECODE_RM_LONG_REGISTER(rh);
            DECODE_PRINTF("\n");
            TRACE_AND_STEP();
            bit = *shiftreg & 0x1F;
            disp = (s16)*shiftreg >> 5;
            srcval = fetch_data_long(srcoffset+disp);
            mask = (0x1 << bit);
            CONDITIONAL_SET_FLAG(srcval & mask,F_CF);
            store_data_long(srcoffset+disp, srcval | mask);
        } else {
            u16 srcval,mask;
            u16 *shiftreg;

            srcoffset = decode_rm_address(mod, rl);
            DECODE_PRINTF(",");
            shiftreg = DECODE_RM_WORD_REGISTER(rh);
            DECODE_PRINTF("\n");
            TRACE_AND_STEP();
            bit = *shiftreg & 0xF;
            disp = (s16)*shiftreg >> 4;
            srcval = fetch_data_word(srcoffset+disp);
			mask = (u16)(0x1 << bit);
            CONDITIONAL_SET_FLAG(srcval & mask,F_CF);
            store_data_word(srcoffset+disp, srcval | mask);
        }
    } else {                     /* register to register */
		if (env->x86.mode & SYSMODE_PREFIX_DATA) {
			u32 *srcreg,*shiftreg;
			u32 mask;

			srcreg = DECODE_RM_LONG_REGISTER(rl);
			DECODE_PRINTF(",");
			shiftreg = DECODE_RM_LONG_REGISTER(rh);
			DECODE_PRINTF("\n");
			TRACE_AND_STEP();
			bit = *shiftreg & 0x1F;
			mask = (0x1 << bit);
			CONDITIONAL_SET_FLAG(*srcreg & mask,F_CF);
			*srcreg |= mask;
		} else {
			u16 *srcreg,*shiftreg;
			u16 mask;

			srcreg = DECODE_RM_WORD_REGISTER(rl);
			DECODE_PRINTF(",");
			shiftreg = DECODE_RM_WORD_REGISTER(rh);
			DECODE_PRINTF("\n");
			TRACE_AND_STEP();
			bit = *shiftreg & 0xF;
			mask = (u16)(0x1 << bit);
            CONDITIONAL_SET_FLAG(*srcreg & mask,F_CF);
            *srcreg |= mask;
        }
    }
    DECODE_CLEAR_SEGOVR();
    END_OF_INSTR();
}

/****************************************************************************
REMARKS:
Handles opcode 0x0f,0xac
****************************************************************************/
static void x86emuOp2_shrd_IMM(X86EMU_sysEnv *env, u8 op2)
{
    int mod, rl, rh;
    u32 destoffset;
	u8 shift;

    START_OF_INSTR();
    DECODE_PRINTF("SHLD\t");
    FETCH_DECODE_MODRM(mod, rh, rl);
    if (mod != 3) {
        if (env->x86.mode & SYSMODE_PREFIX_DATA) {
            u32 destval;
            u32 *shiftreg;

            destoffset = decode_rm_address(mod, rl);
            DECODE_PRINTF(",");
            shiftreg = DECODE_RM_LONG_REGISTER(rh);
            DECODE_PRINTF(",");
            shift = fetch_byte_imm();
            DECODE_PRINTF2("%d\n", shift);
            TRACE_AND_STEP();
            destval = fetch_data_long(destoffset);
            destval = shrd_long(destval,*shiftreg,shift);
            store_data_long(destoffset, destval);
        } else {
            u16 destval;
            u16 *shiftreg;

            destoffset = decode_rm_address(mod, rl);
            DECODE_PRINTF(",");
            shiftreg = DECODE_RM_WORD_REGISTER(rh);
            DECODE_PRINTF(",");
            shift = fetch_byte_imm();
            DECODE_PRINTF2("%d\n", shift);
            TRACE_AND_STEP();
            destval = fetch_data_word(destoffset);
            destval = shrd_word(destval,*shiftreg,shift);
            store_data_word(destoffset, destval);
        }
    } else {                     /* register to register */
        if (env->x86.mode & SYSMODE_PREFIX_DATA) {
            u32 *destreg,*shiftreg;

            destreg = DECODE_RM_LONG_REGISTER(rl);
            DECODE_PRINTF(",");
            shiftreg = DECODE_RM_LONG_REGISTER(rh);
            DECODE_PRINTF(",");
            shift = fetch_byte_imm();
            DECODE_PRINTF2("%d\n", shift);
            TRACE_AND_STEP();
            *destreg = shrd_long(*destreg,*shiftreg,shift);
        } else {
            u16 *destreg,*shiftreg;

            destreg = DECODE_RM_WORD_REGISTER(rl);
            DECODE_PRINTF(",");
            shiftreg = DECODE_RM_WORD_REGISTER(rh);
            DECODE_PRINTF(",");
            shift = fetch_byte_imm();
            DECODE_PRINTF2("%d\n", shift);
            TRACE_AND_STEP();
            *destreg = shrd_word(*destreg,*shiftreg,shift);
        }
    }
    DECODE_CLEAR_SEGOVR();
    END_OF_INSTR();
}

/****************************************************************************
REMARKS:
Handles opcode 0x0f,0xad
****************************************************************************/
static void x86emuOp2_shrd_CL(X86EMU_sysEnv *env, u8 op2)
{
    int mod, rl, rh;
    u32 destoffset;

    START_OF_INSTR();
    DECODE_PRINTF("SHLD\t");
    FETCH_DECODE_MODRM(mod, rh, rl);
    if (mod != 3) {
        if (env->x86.mode & SYSMODE_PREFIX_DATA) {
            u32 destval;
            u32 *shiftreg;

            destoffset = decode_rm_address(mod, rl);
            DECODE_PRINTF(",");
            shiftreg = DECODE_RM_LONG_REGISTER(rh);
            DECODE_PRINTF(",CL\n");
            TRACE_AND_STEP();
            destval = fetch_data_long(destoffset);
            destval = shrd_long(destval,*shiftreg,env->x86.R_CL);
            store_data_long(destoffset, destval);
        } else {
            u16 destval;
            u16 *shiftreg;

            destoffset = decode_rm_address(mod, rl);
            DECODE_PRINTF(",");
            shiftreg = DECODE_RM_WORD_REGISTER(rh);
            DECODE_PRINTF(",CL\n");
            TRACE_AND_STEP();
            destval = fetch_data_word(destoffset);
            destval = shrd_word(destval,*shiftreg,env->x86.R_CL);
            store_data_word(destoffset, destval);
        }
    } else {                     /* register to register */
        if (env->x86.mode & SYSMODE_PREFIX_DATA) {
            u32 *destreg,*shiftreg;

            destreg = DECODE_RM_LONG_REGISTER(rl);
            DECODE_PRINTF(",");
            shiftreg = DECODE_RM_LONG_REGISTER(rh);
            DECODE_PRINTF(",CL\n");
            TRACE_AND_STEP();
            *destreg = shrd_long(*destreg,*shiftreg,env->x86.R_CL);
        } else {
            u16 *destreg,*shiftreg;

            destreg = DECODE_RM_WORD_REGISTER(rl);
            DECODE_PRINTF(",");
            shiftreg = DECODE_RM_WORD_REGISTER(rh);
            DECODE_PRINTF(",CL\n");
            TRACE_AND_STEP();
            *destreg = shrd_word(*destreg,*shiftreg,env->x86.R_CL);
        }
    }
    DECODE_CLEAR_SEGOVR();
    END_OF_INSTR();
}

/****************************************************************************
REMARKS:
Handles opcode 0x0f,0xaf
****************************************************************************/
static void x86emuOp2_imul_R_RM(X86EMU_sysEnv *env, u8 op2)
{
    int mod, rl, rh;
    u32 srcoffset;

    START_OF_INSTR();
    DECODE_PRINTF("IMUL\t");
    FETCH_DECODE_MODRM(mod, rh, rl);
    if (mod != 3) {
        if (env->x86.mode & SYSMODE_PREFIX_DATA) {
            u32 *destreg;
            u32 srcval;
            u32 res_lo,res_hi;

            destreg = DECODE_RM_LONG_REGISTER(rh);
            DECODE_PRINTF(",");
            srcoffset = decode_rm_address(mod, rl);
            DECODE_PRINTF(",");
            srcval = fetch_data_long(srcoffset);
	    DECODE_PRINTF2("%u\n", srcval);
            TRACE_AND_STEP();
            imul_long_direct(&res_lo,&res_hi,(s32)*destreg,(s32)srcval);
            if (res_hi != 0) {
                SET_FLAG(F_CF);
                SET_FLAG(F_OF);
            } else {
                CLEAR_FLAG(F_CF);
                CLEAR_FLAG(F_OF);
            }
            *destreg = (u32)res_lo;
        } else {
            u16 *destreg;
            u16 srcval;
            u32 res;

            destreg = DECODE_RM_WORD_REGISTER(rh);
            DECODE_PRINTF(",");
            srcoffset = decode_rm_address(mod, rl);
            DECODE_PRINTF(",");
            srcval = fetch_data_word(srcoffset);
	    DECODE_PRINTF2("%u\n", srcval);
            TRACE_AND_STEP();
            res = (s16)*destreg * (s16)srcval;
            if (res > 0xFFFF) {
                SET_FLAG(F_CF);
                SET_FLAG(F_OF);
            } else {
                CLEAR_FLAG(F_CF);
                CLEAR_FLAG(F_OF);
            }
            *destreg = (u16)res;
        }
    } else {                     /* register to register */
        if (env->x86.mode & SYSMODE_PREFIX_DATA) {
            u32 *destreg,*srcreg;
            u32 res_lo,res_hi;

            destreg = DECODE_RM_LONG_REGISTER(rh);
            DECODE_PRINTF(",");
            srcreg = DECODE_RM_LONG_REGISTER(rl);
            DECODE_PRINTF("\n");
            TRACE_AND_STEP();
            imul_long_direct(&res_lo,&res_hi,(s32)*destreg,(s32)*srcreg);
            if (res_hi != 0) {
                SET_FLAG(F_CF);
                SET_FLAG(F_OF);
            } else {
                CLEAR_FLAG(F_CF);
                CLEAR_FLAG(F_OF);
            }
            *destreg = (u32)res_lo;
        } else {
            u16 *destreg,*srcreg;
            u32 res;

            destreg = DECODE_RM_WORD_REGISTER(rh);
            DECODE_PRINTF(",");
            srcreg = DECODE_RM_WORD_REGISTER(rl);
            DECODE_PRINTF("\n");
	    TRACE_AND_STEP();
            res = (s16)*destreg * (s16)*srcreg;
            if (res > 0xFFFF) {
                SET_FLAG(F_CF);
                SET_FLAG(F_OF);
            } else {
                CLEAR_FLAG(F_CF);
                CLEAR_FLAG(F_OF);
            }
            *destreg = (u16)res;
        }
    }
    DECODE_CLEAR_SEGOVR();
    END_OF_INSTR();
}

/****************************************************************************
REMARKS:
Handles opcode 0x0f,0xb2
****************************************************************************/
static void x86emuOp2_lss_R_IMM(X86EMU_sysEnv *env, u8 op2)
{
	int mod, rh, rl;
    u16 *dstreg;
    u32 srcoffset;

    START_OF_INSTR();
    DECODE_PRINTF("LSS\t");
    FETCH_DECODE_MODRM(mod, rh, rl);
    if (mod != 3) {
        dstreg = DECODE_RM_WORD_REGISTER(rh);
        DECODE_PRINTF(",");
        srcoffset = decode_rm_address(mod, rl);
        DECODE_PRINTF("\n");
        TRACE_AND_STEP();
        *dstreg = fetch_data_word(srcoffset);
        env->x86.R_SS = fetch_data_word(srcoffset + 2);
    } else {                     /* register to register */
        /* UNDEFINED! */
        TRACE_AND_STEP();
    }
    DECODE_CLEAR_SEGOVR();
    END_OF_INSTR();
}

/****************************************************************************
REMARKS:
Handles opcode 0x0f,0xb3
****************************************************************************/
static void x86emuOp2_btr_R(X86EMU_sysEnv *env, u8 op2)
{
    int mod, rl, rh;
    u32 srcoffset;
    int bit,disp;

    START_OF_INSTR();
    DECODE_PRINTF("BTR\t");
    FETCH_DECODE_MODRM(mod, rh, rl);
    if (mod != 3) {
	if (env->x86.mode & SYSMODE_PREFIX_DATA) {
	    u32 srcval,mask;
	    u32 *shiftreg;

	    srcoffset = decode_rm_address(mod, rl);
	    DECODE_PRINTF(",");
	    shiftreg = DECODE_RM_LONG_REGISTER(rh);
	    DECODE_PRINTF("\n");
	    TRACE_AND_STEP();
	    bit = *shiftreg & 0x1F;
	    disp = (s16)*shiftreg >> 5;
	    srcval = fetch_data_long(srcoffset+disp);
	    mask = (0x1 << bit);
	    CONDITIONAL_SET_FLAG(srcval & mask,F_CF);
	    store_data_long(srcoffset+disp, srcval & ~mask);
	} else {
	    u16 srcval,mask;
	    u16 *shiftreg;

	    srcoffset = decode_rm_address(mod, rl);
	    DECODE_PRINTF(",");
	    shiftreg = DECODE_RM_WORD_REGISTER(rh);
	    DECODE_PRINTF("\n");
	    TRACE_AND_STEP();
	    bit = *shiftreg & 0xF;
	    disp = (s16)*shiftreg >> 4;
	    srcval = fetch_data_word(srcoffset+disp);
	    mask = (u16)(0x1 << bit);
	    CONDITIONAL_SET_FLAG(srcval & mask,F_CF);
	    store_data_word(srcoffset+disp, (u16)(srcval & ~mask));
	}
    } else {                       /* register to register */
	if (env->x86.mode & SYSMODE_PREFIX_DATA) {
	    u32 *srcreg,*shiftreg;
	    u32 mask;

	    srcreg = DECODE_RM_LONG_REGISTER(rl);
	    DECODE_PRINTF(",");
	    shiftreg = DECODE_RM_LONG_REGISTER(rh);
	    DECODE_PRINTF("\n");
	    TRACE_AND_STEP();
	    bit = *shiftreg & 0x1F;
	    mask = (0x1 << bit);
	    CONDITIONAL_SET_FLAG(*srcreg & mask,F_CF);
	    *srcreg &= ~mask;
	} else {
	    u16 *srcreg,*shiftreg;
	    u16 mask;

	    srcreg = DECODE_RM_WORD_REGISTER(rl);
	    DECODE_PRINTF(",");
	    shiftreg = DECODE_RM_WORD_REGISTER(rh);
	    DECODE_PRINTF("\n");
	    TRACE_AND_STEP();
	    bit = *shiftreg & 0xF;
	    mask = (u16)(0x1 << bit);
	    CONDITIONAL_SET_FLAG(*srcreg & mask,F_CF);
	    *srcreg &= ~mask;
	}
    }
    DECODE_CLEAR_SEGOVR();
    END_OF_INSTR();
}

/****************************************************************************
REMARKS:
Handles opcode 0x0f,0xb4
****************************************************************************/
static void x86emuOp2_lfs_R_IMM(X86EMU_sysEnv *env, u8 op2)
{
	int mod, rh, rl;
    u16 *dstreg;
    u32 srcoffset;

    START_OF_INSTR();
    DECODE_PRINTF("LFS\t");
    FETCH_DECODE_MODRM(mod, rh, rl);
    if (mod != 3) {
        dstreg = DECODE_RM_WORD_REGISTER(rh);
        DECODE_PRINTF(",");
        srcoffset = decode_rm_address(mod, rl);
        DECODE_PRINTF("\n");
        TRACE_AND_STEP();
        *dstreg = fetch_data_word(srcoffset);
        env->x86.R_FS = fetch_data_word(srcoffset + 2);
    } else {                     /* register to register */
        /* UNDEFINED! */
        TRACE_AND_STEP();
    }
    DECODE_CLEAR_SEGOVR();
    END_OF_INSTR();
}

/****************************************************************************
REMARKS:
Handles opcode 0x0f,0xb5
****************************************************************************/
static void x86emuOp2_lgs_R_IMM(X86EMU_sysEnv *env, u8 op2)
{
	int mod, rh, rl;
    u16 *dstreg;
    u32 srcoffset;

    START_OF_INSTR();
    DECODE_PRINTF("LGS\t");
    FETCH_DECODE_MODRM(mod, rh, rl);
    if (mod != 3) {
        dstreg = DECODE_RM_WORD_REGISTER(rh);
        DECODE_PRINTF(",");
        srcoffset = decode_rm_address(mod, rl);
        DECODE_PRINTF("\n");
        TRACE_AND_STEP();
        *dstreg = fetch_data_word(srcoffset);
        env->x86.R_GS = fetch_data_word(srcoffset + 2);
    } else {                     /* register to register */
        /* UNDEFINED! */
        TRACE_AND_STEP();
    }
    DECODE_CLEAR_SEGOVR();
    END_OF_INSTR();
}

/****************************************************************************
REMARKS:
Handles opcode 0x0f,0xb6
****************************************************************************/
static void x86emuOp2_movzx_byte_R_RM(X86EMU_sysEnv *env, u8 op2)
{
    int mod, rl, rh;
    u32 srcoffset;

    START_OF_INSTR();
    DECODE_PRINTF("MOVZX\t");
    FETCH_DECODE_MODRM(mod, rh, rl);
    if (mod != 3) {
        if (env->x86.mode & SYSMODE_PREFIX_DATA) {
            u32 *destreg;
            u32 srcval;

            destreg = DECODE_RM_LONG_REGISTER(rh);
            DECODE_PRINTF(",");
            srcoffset = decode_rm_address(mod, rl);
            srcval = fetch_data_byte(srcoffset);
            DECODE_PRINTF("\n");
            TRACE_AND_STEP();
            *destreg = srcval;
        } else {
            u16 *destreg;
            u16 srcval;

            destreg = DECODE_RM_WORD_REGISTER(rh);
            DECODE_PRINTF(",");
            srcoffset = decode_rm_address(mod, rl);
            srcval = fetch_data_byte(srcoffset);
            DECODE_PRINTF("\n");
            TRACE_AND_STEP();
            *destreg = srcval;
        }
    } else {                     /* register to register */
        if (env->x86.mode & SYSMODE_PREFIX_DATA) {
            u32 *destreg;
            u8  *srcreg;

            destreg = DECODE_RM_LONG_REGISTER(rh);
            DECODE_PRINTF(",");
            srcreg = DECODE_RM_BYTE_REGISTER(rl);
            DECODE_PRINTF("\n");
            TRACE_AND_STEP();
            *destreg = *srcreg;
        } else {
            u16 *destreg;
            u8  *srcreg;

            destreg = DECODE_RM_WORD_REGISTER(rh);
            DECODE_PRINTF(",");
            srcreg = DECODE_RM_BYTE_REGISTER(rl);
            DECODE_PRINTF("\n");
            TRACE_AND_STEP();
            *destreg = *srcreg;
        }
    }
    DECODE_CLEAR_SEGOVR();
    END_OF_INSTR();
}

/****************************************************************************
REMARKS:
Handles opcode 0x0f,0xb7
****************************************************************************/
static void x86emuOp2_movzx_word_R_RM(X86EMU_sysEnv *env, u8 op2)
{
    int mod, rl, rh;
    u32 srcoffset;
    u32 *destreg;
    u32 srcval;
    u16 *srcreg;

    START_OF_INSTR();
    DECODE_PRINTF("MOVZX\t");
    FETCH_DECODE_MODRM(mod, rh, rl);
    if (mod != 3) {
        destreg = DECODE_RM_LONG_REGISTER(rh);
        DECODE_PRINTF(",");
        srcoffset = decode_rm_address(mod, rl);
        srcval = fetch_data_word(srcoffset);
        DECODE_PRINTF("\n");
        TRACE_AND_STEP();
        *destreg = srcval;
    } else {                     /* register to register */
        destreg = DECODE_RM_LONG_REGISTER(rh);
        DECODE_PRINTF(",");
        srcreg = DECODE_RM_WORD_REGISTER(rl);
        DECODE_PRINTF("\n");
        TRACE_AND_STEP();
        *destreg = *srcreg;
    }
    DECODE_CLEAR_SEGOVR();
    END_OF_INSTR();
}

/****************************************************************************
REMARKS:
Handles opcode 0x0f,0xba
****************************************************************************/
static void x86emuOp2_btX_I(X86EMU_sysEnv *env, u8 op2)
{
    int mod, rl, rh;
    u32 srcoffset;
    int bit;

    START_OF_INSTR();
    FETCH_DECODE_MODRM(mod, rh, rl);
    switch (rh) {
    case 4:
	DECODE_PRINTF("BT\t");
	break;
    case 5:
	DECODE_PRINTF("BTS\t");
	break;
    case 6:
	DECODE_PRINTF("BTR\t");
	break;
    case 7:
	DECODE_PRINTF("BTC\t");
	break;
    default:
	DECODE_PRINTF("ILLEGAL EXTENDED X86 OPCODE\n");
	TRACE_REGS();
	printf("%04x:%04x: %02X%02X ILLEGAL EXTENDED X86 OPCODE EXTENSION!\n",
	       env->x86.R_CS, env->x86.R_IP-3,op2, (mod<<6)|(rh<<3)|rl);
	HALT_SYS();
    }
    if (mod != 3) {
        if (env->x86.mode & SYSMODE_PREFIX_DATA) {
            u32 srcval, mask;
            u8 shift;

            srcoffset = decode_rm_address(mod, rl);
            DECODE_PRINTF(",");
            shift = fetch_byte_imm();
	    DECODE_PRINTF2("%u\n", shift);
            TRACE_AND_STEP();
            bit = shift & 0x1F;
            srcval = fetch_data_long(srcoffset);
	    mask = (0x1 << bit);
            CONDITIONAL_SET_FLAG(srcval & mask,F_CF);
	    switch (rh) {
	    case 5:
		store_data_long(srcoffset, srcval | mask);
		break;
	    case 6:
		store_data_long(srcoffset, srcval & ~mask);
		break;
	    case 7:
		store_data_long(srcoffset, srcval ^ mask);
		break;
	    default:
		break;
	    }
        } else {
            u16 srcval, mask;
            u8 shift;

            srcoffset = decode_rm_address(mod, rl);
            DECODE_PRINTF(",");
            shift = fetch_byte_imm();
	    DECODE_PRINTF2("%u\n", shift);
            TRACE_AND_STEP();
            bit = shift & 0xF;
            srcval = fetch_data_word(srcoffset);
	    mask = (0x1 << bit);
            CONDITIONAL_SET_FLAG(srcval & mask,F_CF);
	    switch (rh) {
	    case 5:
		store_data_word(srcoffset, srcval | mask);
		break;
	    case 6:
		store_data_word(srcoffset, srcval & ~mask);
		break;
	    case 7:
		store_data_word(srcoffset, srcval ^ mask);
		break;
	    default:
		break;
	    }
        }
    } else {                     /* register to register */
        if (env->x86.mode & SYSMODE_PREFIX_DATA) {
            u32 *srcreg;
	    u32 mask;
	    u8 shift;

            srcreg = DECODE_RM_LONG_REGISTER(rl);
            DECODE_PRINTF(",");
            shift = fetch_byte_imm();
	    DECODE_PRINTF2("%u\n", shift);
            TRACE_AND_STEP();
            bit = shift & 0x1F;
	    mask = (0x1 << bit);
            CONDITIONAL_SET_FLAG(*srcreg & mask,F_CF);
	    switch (rh) {
	    case 5:
		*srcreg |= mask;
		break;
	    case 6:
		*srcreg &= ~mask;
		break;
	    case 7:
		*srcreg ^= mask;
		break;
	    default:
		break;
	    }
        } else {
            u16 *srcreg;
	    u16 mask;
	    u8 shift;

            srcreg = DECODE_RM_WORD_REGISTER(rl);
            DECODE_PRINTF(",");
            shift = fetch_byte_imm();
	    DECODE_PRINTF2("%u\n", shift);
            TRACE_AND_STEP();
            bit = shift & 0xF;
	    mask = (0x1 << bit);
            CONDITIONAL_SET_FLAG(*srcreg & mask,F_CF);
	    switch (rh) {
	    case 5:
		*srcreg |= mask;
		break;
	    case 6:
		*srcreg &= ~mask;
		break;
	    case 7:
		*srcreg ^= mask;
		break;
	    default:
		break;
	    }
        }
    }
    DECODE_CLEAR_SEGOVR();
    END_OF_INSTR();
}

/****************************************************************************
REMARKS:
Handles opcode 0x0f,0xbb
****************************************************************************/
static void x86emuOp2_btc_R(X86EMU_sysEnv *env, u8 op2)
{
    int mod, rl, rh;
    u32 srcoffset;
    int bit,disp;

    START_OF_INSTR();
    DECODE_PRINTF("BTC\t");
    FETCH_DECODE_MODRM(mod, rh, rl);
    if (mod != 3) {
        if (env->x86.mode & SYSMODE_PREFIX_DATA) {
            u32 srcval,mask;
            u32 *shiftreg;

            srcoffset = decode_rm_address(mod, rl);
            DECODE_PRINTF(",");
            shiftreg = DECODE_RM_LONG_REGISTER(rh);
            DECODE_PRINTF("\n");
            TRACE_AND_STEP();
            bit = *shiftreg & 0x1F;
            disp = (s16)*shiftreg >> 5;
            srcval = fetch_data_long(srcoffset+disp);
            mask = (0x1 << bit);
            CONDITIONAL_SET_FLAG(srcval & mask,F_CF);
            store_data_long(srcoffset+disp, srcval ^ mask);
        } else {
            u16 srcval,mask;
            u16 *shiftreg;

            srcoffset = decode_rm_address(mod, rl);
            DECODE_PRINTF(",");
            shiftreg = DECODE_RM_WORD_REGISTER(rh);
            DECODE_PRINTF("\n");
            TRACE_AND_STEP();
            bit = *shiftreg & 0xF;
            disp = (s16)*shiftreg >> 4;
            srcval = fetch_data_word(srcoffset+disp);
			mask = (u16)(0x1 << bit);
            CONDITIONAL_SET_FLAG(srcval & mask,F_CF);
			store_data_word(srcoffset+disp, (u16)(srcval ^ mask));
        }
    } else {                     /* register to register */
        if (env->x86.mode & SYSMODE_PREFIX_DATA) {
			u32 *srcreg,*shiftreg;
            u32 mask;

            srcreg = DECODE_RM_LONG_REGISTER(rl);
            DECODE_PRINTF(",");
            shiftreg = DECODE_RM_LONG_REGISTER(rh);
            DECODE_PRINTF("\n");
            TRACE_AND_STEP();
            bit = *shiftreg & 0x1F;
            mask = (0x1 << bit);
			CONDITIONAL_SET_FLAG(*srcreg & mask,F_CF);
			*srcreg ^= mask;
		} else {
			u16 *srcreg,*shiftreg;
			u16 mask;

			srcreg = DECODE_RM_WORD_REGISTER(rl);
			DECODE_PRINTF(",");
			shiftreg = DECODE_RM_WORD_REGISTER(rh);
			DECODE_PRINTF("\n");
			TRACE_AND_STEP();
			bit = *shiftreg & 0xF;
			mask = (u16)(0x1 << bit);
            CONDITIONAL_SET_FLAG(*srcreg & mask,F_CF);
            *srcreg ^= mask;
        }
    }
    DECODE_CLEAR_SEGOVR();
    END_OF_INSTR();
}

/****************************************************************************
REMARKS:
Handles opcode 0x0f,0xbc
****************************************************************************/
static void x86emuOp2_bsf(X86EMU_sysEnv *env, u8 op2)
{
    int mod, rl, rh;
    u32 srcoffset;

    START_OF_INSTR();
    DECODE_PRINTF("BSF\t");
    FETCH_DECODE_MODRM(mod, rh, rl);
    if (mod != 3) {
	if (env->x86.mode & SYSMODE_PREFIX_DATA) {
	    u32 srcval, *dstreg;

	    srcoffset = decode_rm_address(mod, rl);
	    DECODE_PRINTF(",");
	    dstreg = DECODE_RM_LONG_REGISTER(rh);
            DECODE_PRINTF("\n");
	    TRACE_AND_STEP();
	    srcval = fetch_data_long(srcoffset);
	    CONDITIONAL_SET_FLAG(srcval == 0, F_ZF);
	    for(*dstreg = 0; *dstreg < 32; (*dstreg)++)
		if ((srcval >> *dstreg) & 1) break;
	} else {
	    u16 srcval, *dstreg;

	    srcoffset = decode_rm_address(mod, rl);
	    DECODE_PRINTF(",");
	    dstreg = DECODE_RM_WORD_REGISTER(rh);
            DECODE_PRINTF("\n");
	    TRACE_AND_STEP();
	    srcval = fetch_data_word(srcoffset);
	    CONDITIONAL_SET_FLAG(srcval == 0, F_ZF);
	    for(*dstreg = 0; *dstreg < 16; (*dstreg)++)
		if ((srcval >> *dstreg) & 1) break;
	}
    } else {				/* register to register */
	if (env->x86.mode & SYSMODE_PREFIX_DATA) {
	    u32 srcval, *dstreg;

	    srcval = *DECODE_RM_LONG_REGISTER(rl);
	    DECODE_PRINTF(",");
	    dstreg = DECODE_RM_LONG_REGISTER(rh);
            DECODE_PRINTF("\n");
	    TRACE_AND_STEP();
	    CONDITIONAL_SET_FLAG(srcval == 0, F_ZF);
	    for(*dstreg = 0; *dstreg < 32; (*dstreg)++)
		if ((srcval >> *dstreg) & 1) break;
	} else {
	    u16 srcval, *dstreg;

	    srcval = *DECODE_RM_WORD_REGISTER(rl);
	    DECODE_PRINTF(",");
	    dstreg = DECODE_RM_WORD_REGISTER(rh);
            DECODE_PRINTF("\n");
	    TRACE_AND_STEP();
	    CONDITIONAL_SET_FLAG(srcval == 0, F_ZF);
	    for(*dstreg = 0; *dstreg < 16; (*dstreg)++)
		if ((srcval >> *dstreg) & 1) break;
	}
    }
    DECODE_CLEAR_SEGOVR();
    END_OF_INSTR();
}

/****************************************************************************
REMARKS:
Handles opcode 0x0f,0xbd
****************************************************************************/
static void x86emuOp2_bsr(X86EMU_sysEnv *env, u8 op2)
{
    int mod, rl, rh;
    u32 srcoffset;

    START_OF_INSTR();
    DECODE_PRINTF("BSR\t");
    FETCH_DECODE_MODRM(mod, rh, rl);
    if (mod != 3) {
	if (env->x86.mode & SYSMODE_PREFIX_DATA) {
	    u32 srcval, *dstreg;

	    srcoffset = decode_rm_address(mod, rl);
	    DECODE_PRINTF(",");
	    dstreg = DECODE_RM_LONG_REGISTER(rh);
	    DECODE_PRINTF("\n");
	    TRACE_AND_STEP();
	    srcval = fetch_data_long(srcoffset);
	    CONDITIONAL_SET_FLAG(srcval == 0, F_ZF);
	    for(*dstreg = 31; *dstreg > 0; (*dstreg)--)
		if ((srcval >> *dstreg) & 1) break;
	} else {
	    u16 srcval, *dstreg;

	    srcoffset = decode_rm_address(mod, rl);
	    DECODE_PRINTF(",");
	    dstreg = DECODE_RM_WORD_REGISTER(rh);
	    DECODE_PRINTF("\n");
	    TRACE_AND_STEP();
	    srcval = fetch_data_word(srcoffset);
	    CONDITIONAL_SET_FLAG(srcval == 0, F_ZF);
	    for(*dstreg = 15; *dstreg > 0; (*dstreg)--)
		if ((srcval >> *dstreg) & 1) break;
	}
    } else {				/* register to register */
	if (env->x86.mode & SYSMODE_PREFIX_DATA) {
	    u32 srcval, *dstreg;

	    srcval = *DECODE_RM_LONG_REGISTER(rl);
	    DECODE_PRINTF(",");
	    dstreg = DECODE_RM_LONG_REGISTER(rh);
	    DECODE_PRINTF("\n");
	    TRACE_AND_STEP();
	    CONDITIONAL_SET_FLAG(srcval == 0, F_ZF);
	    for(*dstreg = 31; *dstreg > 0; (*dstreg)--)
		if ((srcval >> *dstreg) & 1) break;
	} else {
	    u16 srcval, *dstreg;

	    srcval = *DECODE_RM_WORD_REGISTER(rl);
	    DECODE_PRINTF(",");
	    dstreg = DECODE_RM_WORD_REGISTER(rh);
	    DECODE_PRINTF("\n");
	    TRACE_AND_STEP();
	    CONDITIONAL_SET_FLAG(srcval == 0, F_ZF);
	    for(*dstreg = 15; *dstreg > 0; (*dstreg)--)
		if ((srcval >> *dstreg) & 1) break;
	}
    }
    DECODE_CLEAR_SEGOVR();
    END_OF_INSTR();
}

/****************************************************************************
REMARKS:
Handles opcode 0x0f,0xbe
****************************************************************************/
static void x86emuOp2_movsx_byte_R_RM(X86EMU_sysEnv *env, u8 op2)
{
    int mod, rl, rh;
    u32 srcoffset;

    START_OF_INSTR();
    DECODE_PRINTF("MOVSX\t");
    FETCH_DECODE_MODRM(mod, rh, rl);
    if (mod != 3) {
        if (env->x86.mode & SYSMODE_PREFIX_DATA) {
            u32 *destreg;
            u32 srcval;

            destreg = DECODE_RM_LONG_REGISTER(rh);
            DECODE_PRINTF(",");
            srcoffset = decode_rm_address(mod, rl);
            srcval = (s32)((s8)fetch_data_byte(srcoffset));
            DECODE_PRINTF("\n");
            TRACE_AND_STEP();
            *destreg = srcval;
        } else {
            u16 *destreg;
            u16 srcval;

            destreg = DECODE_RM_WORD_REGISTER(rh);
            DECODE_PRINTF(",");
            srcoffset = decode_rm_address(mod, rl);
            srcval = (s16)((s8)fetch_data_byte(srcoffset));
            DECODE_PRINTF("\n");
            TRACE_AND_STEP();
            *destreg = srcval;
        }
    } else {                     /* register to register */
        if (env->x86.mode & SYSMODE_PREFIX_DATA) {
            u32 *destreg;
            u8  *srcreg;

            destreg = DECODE_RM_LONG_REGISTER(rh);
            DECODE_PRINTF(",");
            srcreg = DECODE_RM_BYTE_REGISTER(rl);
            DECODE_PRINTF("\n");
            TRACE_AND_STEP();
            *destreg = (s32)((s8)*srcreg);
        } else {
            u16 *destreg;
            u8  *srcreg;

            destreg = DECODE_RM_WORD_REGISTER(rh);
            DECODE_PRINTF(",");
            srcreg = DECODE_RM_BYTE_REGISTER(rl);
            DECODE_PRINTF("\n");
            TRACE_AND_STEP();
            *destreg = (s16)((s8)*srcreg);
        }
    }
    DECODE_CLEAR_SEGOVR();
    END_OF_INSTR();
}

/****************************************************************************
REMARKS:
Handles opcode 0x0f,0xbf
****************************************************************************/
static void x86emuOp2_movsx_word_R_RM(X86EMU_sysEnv *env, u8 op2)
{
    int mod, rl, rh;
    u32 srcoffset;
    u32 *destreg;
    u32 srcval;
    u16 *srcreg;

    START_OF_INSTR();
    DECODE_PRINTF("MOVSX\t");
    FETCH_DECODE_MODRM(mod, rh, rl);
    if (mod != 3) {
        destreg = DECODE_RM_LONG_REGISTER(rh);
        DECODE_PRINTF(",");
        srcoffset = decode_rm_address(mod, rl);
        srcval = (s32)((s16)fetch_data_word(srcoffset));
        DECODE_PRINTF("\n");
        TRACE_AND_STEP();
        *destreg = srcval;
    } else {                     /* register to register */
        destreg = DECODE_RM_LONG_REGISTER(rh);
        DECODE_PRINTF(",");
        srcreg = DECODE_RM_WORD_REGISTER(rl);
        DECODE_PRINTF("\n");
        TRACE_AND_STEP();
        *destreg = (s32)((s16)*srcreg);
    }
    DECODE_CLEAR_SEGOVR();
    END_OF_INSTR();
}

static inline u32
bswap(u32 x)
{
    u8 tmp;
    union {
	u8 bytes[4];
	u32 uint;
    } u;

    u.uint = x;

    tmp = u.bytes[0];
    u.bytes[0] = u.bytes[3];
    u.bytes[3] = tmp;

    tmp = u.bytes[1];
    u.bytes[1] = u.bytes[2];
    u.bytes[2] = tmp;

    return u.uint;
}

/****************************************************************************
REMARKS:
Handles opcode 0x0f, {0xc8 - 0xcf}
****************************************************************************/
static void x86emuOp2_bswap_R(X86EMU_sysEnv *env, u8 op1)
{
    int r = op1 & 7;
    u32 *reg;
    START_OF_INSTR();
    DECODE_PRINTF("BSWAP\t");

    reg = DECODE_RM_LONG_REGISTER(r);
    DECODE_PRINTF("\n");
    TRACE_AND_STEP();
    *reg = bswap(*reg);

    DECODE_CLEAR_SEGOVR();
    END_OF_INSTR();
}

/***************************************************************************
 * Double byte operation code table:
 **************************************************************************/
void (*x86emu_optab2[256])(X86EMU_sysEnv *, u8) =
{
/*  0x00 */ x86emuOp2_illegal_op,  /* Group F (ring 0 PM)      */
/*  0x01 */ x86emuOp2_illegal_op,  /* Group G (ring 0 PM)      */
/*  0x02 */ x86emuOp2_illegal_op,  /* lar (ring 0 PM)          */
/*  0x03 */ x86emuOp2_illegal_op,  /* lsl (ring 0 PM)          */
/*  0x04 */ x86emuOp2_illegal_op,
/*  0x05 */ x86emuOp2_illegal_op,  /* loadall (undocumented)   */
/*  0x06 */ x86emuOp2_illegal_op,  /* clts (ring 0 PM)         */
/*  0x07 */ x86emuOp2_illegal_op,  /* loadall (undocumented)   */
/*  0x08 */ x86emuOp2_illegal_op,  /* invd (ring 0 PM)         */
/*  0x09 */ x86emuOp2_illegal_op,  /* wbinvd (ring 0 PM)       */
/*  0x0a */ x86emuOp2_illegal_op,
/*  0x0b */ x86emuOp2_illegal_op,
/*  0x0c */ x86emuOp2_illegal_op,
/*  0x0d */ x86emuOp2_illegal_op,
/*  0x0e */ x86emuOp2_illegal_op,
/*  0x0f */ x86emuOp2_illegal_op,

/*  0x10 */ x86emuOp2_illegal_op,
/*  0x11 */ x86emuOp2_illegal_op,
/*  0x12 */ x86emuOp2_illegal_op,
/*  0x13 */ x86emuOp2_illegal_op,
/*  0x14 */ x86emuOp2_illegal_op,
/*  0x15 */ x86emuOp2_illegal_op,
/*  0x16 */ x86emuOp2_illegal_op,
/*  0x17 */ x86emuOp2_illegal_op,
/*  0x18 */ x86emuOp2_illegal_op,
/*  0x19 */ x86emuOp2_illegal_op,
/*  0x1a */ x86emuOp2_illegal_op,
/*  0x1b */ x86emuOp2_illegal_op,
/*  0x1c */ x86emuOp2_illegal_op,
/*  0x1d */ x86emuOp2_illegal_op,
/*  0x1e */ x86emuOp2_illegal_op,
/*  0x1f */ x86emuOp2_illegal_op,

/*  0x20 */ x86emuOp2_illegal_op,  /* mov reg32,creg (ring 0 PM) */
/*  0x21 */ x86emuOp2_illegal_op,  /* mov reg32,dreg (ring 0 PM) */
/*  0x22 */ x86emuOp2_illegal_op,  /* mov creg,reg32 (ring 0 PM) */
/*  0x23 */ x86emuOp2_illegal_op,  /* mov dreg,reg32 (ring 0 PM) */
/*  0x24 */ x86emuOp2_illegal_op,  /* mov reg32,treg (ring 0 PM) */
/*  0x25 */ x86emuOp2_illegal_op,
/*  0x26 */ x86emuOp2_illegal_op,  /* mov treg,reg32 (ring 0 PM) */
/*  0x27 */ x86emuOp2_illegal_op,
/*  0x28 */ x86emuOp2_illegal_op,
/*  0x29 */ x86emuOp2_illegal_op,
/*  0x2a */ x86emuOp2_illegal_op,
/*  0x2b */ x86emuOp2_illegal_op,
/*  0x2c */ x86emuOp2_illegal_op,
/*  0x2d */ x86emuOp2_illegal_op,
/*  0x2e */ x86emuOp2_illegal_op,
/*  0x2f */ x86emuOp2_illegal_op,

/*  0x30 */ x86emuOp2_illegal_op,
/*  0x31 */ x86emuOp2_illegal_op,
/*  0x32 */ x86emuOp2_illegal_op,
/*  0x33 */ x86emuOp2_illegal_op,
/*  0x34 */ x86emuOp2_illegal_op,
/*  0x35 */ x86emuOp2_illegal_op,
/*  0x36 */ x86emuOp2_illegal_op,
/*  0x37 */ x86emuOp2_illegal_op,
/*  0x38 */ x86emuOp2_illegal_op,
/*  0x39 */ x86emuOp2_illegal_op,
/*  0x3a */ x86emuOp2_illegal_op,
/*  0x3b */ x86emuOp2_illegal_op,
/*  0x3c */ x86emuOp2_illegal_op,
/*  0x3d */ x86emuOp2_illegal_op,
/*  0x3e */ x86emuOp2_illegal_op,
/*  0x3f */ x86emuOp2_illegal_op,

/*  0x40 */ x86emuOp2_illegal_op,
/*  0x41 */ x86emuOp2_illegal_op,
/*  0x42 */ x86emuOp2_illegal_op,
/*  0x43 */ x86emuOp2_illegal_op,
/*  0x44 */ x86emuOp2_illegal_op,
/*  0x45 */ x86emuOp2_illegal_op,
/*  0x46 */ x86emuOp2_illegal_op,
/*  0x47 */ x86emuOp2_illegal_op,
/*  0x48 */ x86emuOp2_illegal_op,
/*  0x49 */ x86emuOp2_illegal_op,
/*  0x4a */ x86emuOp2_illegal_op,
/*  0x4b */ x86emuOp2_illegal_op,
/*  0x4c */ x86emuOp2_illegal_op,
/*  0x4d */ x86emuOp2_illegal_op,
/*  0x4e */ x86emuOp2_illegal_op,
/*  0x4f */ x86emuOp2_illegal_op,

/*  0x50 */ x86emuOp2_illegal_op,
/*  0x51 */ x86emuOp2_illegal_op,
/*  0x52 */ x86emuOp2_illegal_op,
/*  0x53 */ x86emuOp2_illegal_op,
/*  0x54 */ x86emuOp2_illegal_op,
/*  0x55 */ x86emuOp2_illegal_op,
/*  0x56 */ x86emuOp2_illegal_op,
/*  0x57 */ x86emuOp2_illegal_op,
/*  0x58 */ x86emuOp2_illegal_op,
/*  0x59 */ x86emuOp2_illegal_op,
/*  0x5a */ x86emuOp2_illegal_op,
/*  0x5b */ x86emuOp2_illegal_op,
/*  0x5c */ x86emuOp2_illegal_op,
/*  0x5d */ x86emuOp2_illegal_op,
/*  0x5e */ x86emuOp2_illegal_op,
/*  0x5f */ x86emuOp2_illegal_op,

/*  0x60 */ x86emuOp2_illegal_op,
/*  0x61 */ x86emuOp2_illegal_op,
/*  0x62 */ x86emuOp2_illegal_op,
/*  0x63 */ x86emuOp2_illegal_op,
/*  0x64 */ x86emuOp2_illegal_op,
/*  0x65 */ x86emuOp2_illegal_op,
/*  0x66 */ x86emuOp2_illegal_op,
/*  0x67 */ x86emuOp2_illegal_op,
/*  0x68 */ x86emuOp2_illegal_op,
/*  0x69 */ x86emuOp2_illegal_op,
/*  0x6a */ x86emuOp2_illegal_op,
/*  0x6b */ x86emuOp2_illegal_op,
/*  0x6c */ x86emuOp2_illegal_op,
/*  0x6d */ x86emuOp2_illegal_op,
/*  0x6e */ x86emuOp2_illegal_op,
/*  0x6f */ x86emuOp2_illegal_op,

/*  0x70 */ x86emuOp2_illegal_op,
/*  0x71 */ x86emuOp2_illegal_op,
/*  0x72 */ x86emuOp2_illegal_op,
/*  0x73 */ x86emuOp2_illegal_op,
/*  0x74 */ x86emuOp2_illegal_op,
/*  0x75 */ x86emuOp2_illegal_op,
/*  0x76 */ x86emuOp2_illegal_op,
/*  0x77 */ x86emuOp2_illegal_op,
/*  0x78 */ x86emuOp2_illegal_op,
/*  0x79 */ x86emuOp2_illegal_op,
/*  0x7a */ x86emuOp2_illegal_op,
/*  0x7b */ x86emuOp2_illegal_op,
/*  0x7c */ x86emuOp2_illegal_op,
/*  0x7d */ x86emuOp2_illegal_op,
/*  0x7e */ x86emuOp2_illegal_op,
/*  0x7f */ x86emuOp2_illegal_op,

/*  0x80 */ x86emuOp2_long_jump,
/*  0x81 */ x86emuOp2_long_jump,
/*  0x82 */ x86emuOp2_long_jump,
/*  0x83 */ x86emuOp2_long_jump,
/*  0x84 */ x86emuOp2_long_jump,
/*  0x85 */ x86emuOp2_long_jump,
/*  0x86 */ x86emuOp2_long_jump,
/*  0x87 */ x86emuOp2_long_jump,
/*  0x88 */ x86emuOp2_long_jump,
/*  0x89 */ x86emuOp2_long_jump,
/*  0x8a */ x86emuOp2_long_jump,
/*  0x8b */ x86emuOp2_long_jump,
/*  0x8c */ x86emuOp2_long_jump,
/*  0x8d */ x86emuOp2_long_jump,
/*  0x8e */ x86emuOp2_long_jump,
/*  0x8f */ x86emuOp2_long_jump,

/*  0x90 */ x86emuOp2_set_byte,
/*  0x91 */ x86emuOp2_set_byte,
/*  0x92 */ x86emuOp2_set_byte,
/*  0x93 */ x86emuOp2_set_byte,
/*  0x94 */ x86emuOp2_set_byte,
/*  0x95 */ x86emuOp2_set_byte,
/*  0x96 */ x86emuOp2_set_byte,
/*  0x97 */ x86emuOp2_set_byte,
/*  0x98 */ x86emuOp2_set_byte,
/*  0x99 */ x86emuOp2_set_byte,
/*  0x9a */ x86emuOp2_set_byte,
/*  0x9b */ x86emuOp2_set_byte,
/*  0x9c */ x86emuOp2_set_byte,
/*  0x9d */ x86emuOp2_set_byte,
/*  0x9e */ x86emuOp2_set_byte,
/*  0x9f */ x86emuOp2_set_byte,

/*  0xa0 */ x86emuOp2_push_FS,
/*  0xa1 */ x86emuOp2_pop_FS,
/*  0xa2 */ x86emuOp2_illegal_op,  /* CPUID */
/*  0xa3 */ x86emuOp2_bt_R,
/*  0xa4 */ x86emuOp2_shld_IMM,
/*  0xa5 */ x86emuOp2_shld_CL,
/*  0xa6 */ x86emuOp2_illegal_op,
/*  0xa7 */ x86emuOp2_illegal_op,
/*  0xa8 */ x86emuOp2_push_GS,
/*  0xa9 */ x86emuOp2_pop_GS,
/*  0xaa */ x86emuOp2_illegal_op,
/*  0xab */ x86emuOp2_bts_R,
/*  0xac */ x86emuOp2_shrd_IMM,
/*  0xad */ x86emuOp2_shrd_CL,
/*  0xae */ x86emuOp2_illegal_op,
/*  0xaf */ x86emuOp2_imul_R_RM,

/*  0xb0 */ x86emuOp2_illegal_op,  /* TODO: cmpxchg */
/*  0xb1 */ x86emuOp2_illegal_op,  /* TODO: cmpxchg */
/*  0xb2 */ x86emuOp2_lss_R_IMM,
/*  0xb3 */ x86emuOp2_btr_R,
/*  0xb4 */ x86emuOp2_lfs_R_IMM,
/*  0xb5 */ x86emuOp2_lgs_R_IMM,
/*  0xb6 */ x86emuOp2_movzx_byte_R_RM,
/*  0xb7 */ x86emuOp2_movzx_word_R_RM,
/*  0xb8 */ x86emuOp2_illegal_op,
/*  0xb9 */ x86emuOp2_illegal_op,
/*  0xba */ x86emuOp2_btX_I,
/*  0xbb */ x86emuOp2_btc_R,
/*  0xbc */ x86emuOp2_bsf,
/*  0xbd */ x86emuOp2_bsr,
/*  0xbe */ x86emuOp2_movsx_byte_R_RM,
/*  0xbf */ x86emuOp2_movsx_word_R_RM,

/*  0xc0 */ x86emuOp2_illegal_op,  /* TODO: xadd */
/*  0xc1 */ x86emuOp2_illegal_op,  /* TODO: xadd */
/*  0xc2 */ x86emuOp2_illegal_op,
/*  0xc3 */ x86emuOp2_illegal_op,
/*  0xc4 */ x86emuOp2_illegal_op,
/*  0xc5 */ x86emuOp2_illegal_op,
/*  0xc6 */ x86emuOp2_illegal_op,
/*  0xc7 */ x86emuOp2_illegal_op,
/*  0xc8 */ x86emuOp2_bswap_R,
/*  0xc9 */ x86emuOp2_bswap_R,
/*  0xca */ x86emuOp2_bswap_R,
/*  0xcb */ x86emuOp2_bswap_R,
/*  0xcc */ x86emuOp2_bswap_R,
/*  0xcd */ x86emuOp2_bswap_R,
/*  0xce */ x86emuOp2_bswap_R,
/*  0xcf */ x86emuOp2_bswap_R,

/*  0xd0 */ x86emuOp2_illegal_op,
/*  0xd1 */ x86emuOp2_illegal_op,
/*  0xd2 */ x86emuOp2_illegal_op,
/*  0xd3 */ x86emuOp2_illegal_op,
/*  0xd4 */ x86emuOp2_illegal_op,
/*  0xd5 */ x86emuOp2_illegal_op,
/*  0xd6 */ x86emuOp2_illegal_op,
/*  0xd7 */ x86emuOp2_illegal_op,
/*  0xd8 */ x86emuOp2_illegal_op,
/*  0xd9 */ x86emuOp2_illegal_op,
/*  0xda */ x86emuOp2_illegal_op,
/*  0xdb */ x86emuOp2_illegal_op,
/*  0xdc */ x86emuOp2_illegal_op,
/*  0xdd */ x86emuOp2_illegal_op,
/*  0xde */ x86emuOp2_illegal_op,
/*  0xdf */ x86emuOp2_illegal_op,

/*  0xe0 */ x86emuOp2_illegal_op,
/*  0xe1 */ x86emuOp2_illegal_op,
/*  0xe2 */ x86emuOp2_illegal_op,
/*  0xe3 */ x86emuOp2_illegal_op,
/*  0xe4 */ x86emuOp2_illegal_op,
/*  0xe5 */ x86emuOp2_illegal_op,
/*  0xe6 */ x86emuOp2_illegal_op,
/*  0xe7 */ x86emuOp2_illegal_op,
/*  0xe8 */ x86emuOp2_illegal_op,
/*  0xe9 */ x86emuOp2_illegal_op,
/*  0xea */ x86emuOp2_illegal_op,
/*  0xeb */ x86emuOp2_illegal_op,
/*  0xec */ x86emuOp2_illegal_op,
/*  0xed */ x86emuOp2_illegal_op,
/*  0xee */ x86emuOp2_illegal_op,
/*  0xef */ x86emuOp2_illegal_op,

/*  0xf0 */ x86emuOp2_illegal_op,
/*  0xf1 */ x86emuOp2_illegal_op,
/*  0xf2 */ x86emuOp2_illegal_op,
/*  0xf3 */ x86emuOp2_illegal_op,
/*  0xf4 */ x86emuOp2_illegal_op,
/*  0xf5 */ x86emuOp2_illegal_op,
/*  0xf6 */ x86emuOp2_illegal_op,
/*  0xf7 */ x86emuOp2_illegal_op,
/*  0xf8 */ x86emuOp2_illegal_op,
/*  0xf9 */ x86emuOp2_illegal_op,
/*  0xfa */ x86emuOp2_illegal_op,
/*  0xfb */ x86emuOp2_illegal_op,
/*  0xfc */ x86emuOp2_illegal_op,
/*  0xfd */ x86emuOp2_illegal_op,
/*  0xfe */ x86emuOp2_illegal_op,
/*  0xff */ x86emuOp2_illegal_op,
};
