/*
 * vim:ts=4:sw=4:expandtab
 *
 * tracer-etmv4.c: Core of ETMv4 tracer
 * Copyright (C) 2013  Chih-Chyuan Hwang (hwangcc@csie.nctu.edu.tw)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#include <stdio.h>
#include "tracer.h"
#include "stream.h"

static const char *cond_result_token_apsr[] = 
{
    "C flag set",
    "N flag set",
    "Z and C flags set",
    "N and C flags set",
    "unknown",
    "unknown",
    "unknown",
    "No flag set",
    "unknown",
    "unknown",
    "unknown",
    "Z flag set",
    "unknown",
    "unknown",
    "unknown",
    "unknown"
};

static const char *cond_result_token_pass_fail[] =
{
    "failed the condition code check",
    "passed the condition code check",
    "don't know the result of the condition code check"
};

static const char *exp_name[32] = { "PE reset", "Debug halt", "Call", "Trap",
                                    "System error", NULL, "Inst debug", "Data debug",
                                    NULL, NULL, "Alignment", "Inst fault",
                                    "Data fault", NULL, "IRQ", "FIQ" };

void tracer_trace_info(void *t, unsigned int plctl, unsigned int info,\
                       unsigned int key, unsigned int spec, unsigned int cyct)
{
    struct etmv4_tracer *tracer = (struct etmv4_tracer *)t;

    RESET_ADDRESS_REGISTER(tracer);

    TRACE_INFO(tracer) = (plctl & 1)? info: 0;
    P0_KEY(tracer) = (plctl & 2)? key: 0;
    CURR_SPEC_DEPTH(tracer) = (plctl & 4)? spec: 0;
    CC_THRESHOLD(tracer) = (plctl & 8)? cyct: 0;

    printf("TraceInfo - %s,\n", (TRACE_INFO(tracer) & 0x01)? "Cycle count enabled": "Cycle count disabled");
    printf("            %s,\n", (TRACE_INFO(tracer) & 0x0E)? "Tracing of conditional non-branch instruction enabled": "Tracing of conditional non-branch instruction disabled");
    printf("            %s,\n", (TRACE_INFO(tracer) & 0x10)? "Explicit tracing of load instructions": "No explicit tracing of load instructions");
    printf("            %s,\n", (TRACE_INFO(tracer) & 0x20)? "Explicit tracing of store instructions": "No explicit tracing of store instructions");
    printf("            p0_key = 0x%X,\n", P0_KEY(tracer));
    printf("            curr_spec_depth = %d,\n", CURR_SPEC_DEPTH(tracer));
    printf("            cc_threshold = 0x%X\n", CC_THRESHOLD(tracer));
}

void tracer_trace_on(void *t)
{
    printf("TraceOn - A discontinuity in the trace stream\n");
}

void tracer_discard(void *t)
{
    struct etmv4_tracer *tracer = (struct etmv4_tracer *)t;

    printf("Discard\n");

    tracer_cond_flush(tracer);

    CURR_SPEC_DEPTH(tracer) = 0;
}

void tracer_overflow(void *t)
{
    struct etmv4_tracer *tracer = (struct etmv4_tracer *)t;

    printf("Discard\n");

    tracer_cond_flush(tracer);

    CURR_SPEC_DEPTH(tracer) = 0;
}

void tracer_ts(void *t, unsigned long long timestamp, int have_cc, unsigned int count,  \
               int nr_replace)
{
    struct etmv4_tracer *tracer = (struct etmv4_tracer *)t;

    if (timestamp) {
        TIMESTAMP(tracer) &= ~((1LL << nr_replace) - 1);
        TIMESTAMP(tracer) |= timestamp;
    }

    printf("Timestamp - %lld\n", TIMESTAMP(tracer));
    if (have_cc) {
        printf("            (number of cycles between the most recent Cycle Count element %d)\n", count);
    }
}

void tracer_exception(void *t, int type)
{
    struct etmv4_tracer *tracer = (struct etmv4_tracer *)t;

    printf("Exception - exception type %s, address 0x%016llx\n", (type < 32 && exp_name[type])? exp_name[type]: "Reserved", ADDRESS_REGISTER(tracer)[0].address);

    tracer_cond_flush(tracer);

    /*
     * If p0_key_max is zero, it implies that the target CPU uses no P0 right-hand keys.
     * If so, there is no need to update p0_key.
     */
    if (P0_KEY_MAX(tracer)) {
        P0_KEY(tracer)++;
        P0_KEY(tracer) %= P0_KEY_MAX(tracer);
    }

    CURR_SPEC_DEPTH(tracer)++;
    if (!MAX_SPEC_DEPTH(tracer) || (CURR_SPEC_DEPTH(tracer) > MAX_SPEC_DEPTH(tracer))) {
        tracer_commit(tracer, 1);
    }
}

void tracer_exception_return(void *t)
{
    printf("Exception return\n");

    /* FIXME: for ARMv6-M and ARMv7-M PEs, exception_return is a P0 element */
}

void tracer_cc(void *t, int unknown, unsigned int count)
{
    struct etmv4_tracer *tracer = (struct etmv4_tracer *)t;
    unsigned long long cc;

    if (unknown) {
        printf("Cycle count - unknown\n");
    } else {
        cc = (unsigned long long)count + (unsigned long long)CC_THRESHOLD(tracer);
        printf("Cycle count - %lld\n", cc);
    }
}

void tracer_commit(void *t, unsigned int commit)
{
    struct etmv4_tracer *tracer = (struct etmv4_tracer *)t;

    printf("Commit - %d\n", commit);

    CURR_SPEC_DEPTH(tracer) -= commit;
}

void tracer_cancel(void *t, int mispredict, unsigned int cancel)
{
    struct etmv4_tracer *tracer = (struct etmv4_tracer *)t;

    printf("Cancel - %d\n", cancel);

    CURR_SPEC_DEPTH(tracer) -= cancel;

    /*
     * If p0_key_max is zero, it implies that the target CPU uses no P0 right-hand keys.
     * If so, there is no need to update p0_key.
     */
    if (P0_KEY_MAX(tracer)) {
        P0_KEY(tracer) -= cancel;
        P0_KEY(tracer) %= P0_KEY_MAX(tracer);
    }

    if (mispredict) {
        tracer_mispredict(tracer, 0);
        tracer_cond_flush(tracer);
    }
}

void tracer_mispredict(void *t, int param)
{
    struct etmv4_tracer *tracer = (struct etmv4_tracer *)t;

    switch (param) {
    case 0:
        break;

    case 1:
        tracer_atom(tracer, ATOM_TYPE_E);
        break;

    case 2:
        tracer_atom(tracer, ATOM_TYPE_E);
        tracer_atom(tracer, ATOM_TYPE_E);
        break;

    case 3:
        tracer_atom(tracer, ATOM_TYPE_N);
        break;

    default:
        printf("Invalid param (%d)\n", param);
        break;
    }

    printf("Mispredict\n");
}

static int __is_cond_key_special(struct etmv4_tracer *tracer, unsigned int key)
{
    return (key >= COND_KEY_MAX_INCR(tracer))? 1: 0;
}

void tracer_cond_inst(void *t, int format, unsigned int param1, unsigned int param2)
{
    struct etmv4_tracer *tracer = (struct etmv4_tracer *)t;
    unsigned int key;
    int ci, i, z, num;

    if (COND_KEY_MAX_INCR(tracer) == 0) {
        printf("cond_key_max_incr MUST NOT be zero for conditional instruction elements. Set it via input arguments --trcidr12 and --trcidr13\n");
        return ;
    }

    switch (format) {
    case 1:
        key = param1;
        if (__is_cond_key_special(tracer, key)) {
            COND_C_KEY(tracer)++;
            COND_C_KEY(tracer) %= COND_KEY_MAX_INCR(tracer);
        } else {
            COND_C_KEY(tracer) = key;
        }
        printf("Conditional instruction - C key = %d\n", key);
        break;

    case 2:
        ci = param1;
        if (ci == 0) {
            COND_C_KEY(tracer)++;
            COND_C_KEY(tracer) %= COND_KEY_MAX_INCR(tracer);
            printf("Conditional instruction - C key = %d\n", COND_C_KEY(tracer));
        } else if (ci == 1) {
            printf("Conditional instruction - C key = %d\n", COND_C_KEY(tracer));
        } else if (ci == 2) {
            COND_C_KEY(tracer)++;
            COND_C_KEY(tracer) %= COND_KEY_MAX_INCR(tracer);
            printf("Conditional instruction - C key = %d\n", COND_C_KEY(tracer));
            printf("Conditional instruction - C key = %d\n", COND_C_KEY(tracer));
        } else {
            printf("Invalid CI (%d)\n", ci);
        }
        break;

    case 3:
        z = param1;
        num = param2;
        for (i = 0; i < num; i++) {
            COND_C_KEY(tracer)++;
            COND_C_KEY(tracer) %= COND_KEY_MAX_INCR(tracer);
            printf("Conditional instruction - C key = %d\n", COND_C_KEY(tracer));
        }
        if (z) {
            printf("Conditional instruction - C key = %d\n", COND_C_KEY(tracer));
        }
        break;

    default:
        printf("Invalid format (%d)\n", format);
        break;
    }
}

void tracer_cond_flush(void *t)
{
    printf("Conditional flush\n");
}

/*
 * __interpret_tokens: Interpret tokens for conditional result elements
 * @tracer: pointer to the etmv4_tracer structure
 * @tokens: tokens to interpret
 * @pos: start position in tokens
 * Return the next position for the next token, or 0 for Null (no R element indicated)
 */
static int __interpret_tokens(struct etmv4_tracer *tracer, unsigned int tokens, int pos)
{
    unsigned char token;

    if (pos % 2) {
        printf("Invalid pos\n");
        return pos + 1;
    }

    if (CONDTYPE(tracer)) {
        token = (tokens >> pos) & 0x0F;
        if ((token & 0x03) == 0x03) {
            /* 2-bit tokens */
            return pos + 2;
        } else {
            /* 4-bit otkens */
            if (token == 0x0F) {
                /* NULL, no R element indicated */
                return 0;
            } else {
                return pos + 4;
            }
        }
    } else {
        token = (tokens >> pos) & 0x03;
        if (token == 3) {
            /* NULL, no R element indicated */
            return 0;
        } else {
            return pos + 2;
        }
    }
}

void tracer_cond_result(void *t, int format, unsigned int param1, \
                        unsigned int param2, unsigned int param3)
{
    struct etmv4_tracer *tracer = (struct etmv4_tracer *)t;
    unsigned int key, ci, result, k, tokens, token;
    int pos, next_pos;
    const int MAX_TOKENS_POS = 12;

    if (COND_KEY_MAX_INCR(tracer) == 0) {
        printf("cond_key_max_incr MUST NOT be zero for conditional instruction elements. Set it via input arguments --trcidr12 and --trcidr13\n");
        return ;
    }

    switch (format) {
    case 1:
        key = param1;
        ci = param2;
        result = param3;
        if (__is_cond_key_special(tracer, key)) {
            COND_R_KEY(tracer)++;
            COND_R_KEY(tracer) %= COND_KEY_MAX_INCR(tracer);
        } else {
            COND_R_KEY(tracer) = key;
        }
        if (ci) {
            do {
                tracer_cond_inst(tracer, 3, 0, 1);
            } while (COND_C_KEY(tracer) == COND_R_KEY(tracer));
        }
        if (CONDTYPE(tracer)) {
            printf("Conditional result - R key = %d, APSR_V = %d, APSR_C = %d, APSR_Z = %d, APSR_N = %d\n", \
                    key, result & 0x01, result & 0x02, result & 0x04, result & 0x08);
        } else {
            printf("Conditional result - R key = %d, %s the condition code check\n", \
                    key, (result)? "passed": "failed");
        }
        break;

    case 2:
        k = param1 & 0x01;
        token = param2 & 0x03;
        COND_R_KEY(tracer) += 1 + k;
        COND_R_KEY(tracer) %= COND_KEY_MAX_INCR(tracer);
        do {
            tracer_cond_inst(tracer, 3, 0, 1);
        } while (COND_C_KEY(tracer) == COND_R_KEY(tracer));
        if (CONDTYPE(tracer)) {
            printf("Conditional result - R key = %d, APSR indication: %s\n", \
                    COND_R_KEY(tracer), cond_result_token_apsr[token]);
        } else {
            printf("Conditional result - R key = %d, %s\n", \
                    COND_R_KEY(tracer), cond_result_token_pass_fail[token]);
        }
        break;

    case 3:
        tokens = param1 & 0x0FFF;
        pos = 0;
        do {
            next_pos = __interpret_tokens(tracer, tokens, pos);
            if (next_pos) {
                token = (tokens & ((1 << next_pos) - 1)) >> pos;
                if (CONDTYPE(tracer)) {
                    printf("Conditional result - R key = %d, APSR indication: %s\n", \
                            COND_R_KEY(tracer), cond_result_token_apsr[token]);
                } else {
                    printf("Conditional result - R key = %d, %s\n", \
                            COND_R_KEY(tracer), cond_result_token_pass_fail[token]);
                }
                pos = next_pos;
            }
        } while (next_pos != 0 || pos < MAX_TOKENS_POS);
        break;

    case 4:
        token = param1 & 0x03;
        COND_R_KEY(tracer)--;
        COND_R_KEY(tracer) %= COND_KEY_MAX_INCR(tracer);
        if (CONDTYPE(tracer)) {
            printf("Conditional result - R key = %d, APSR indication: %s\n", \
                    COND_R_KEY(tracer), cond_result_token_apsr[token]);
        } else {
            printf("Conditional result - R key = %d, %s\n", \
                    COND_R_KEY(tracer), cond_result_token_pass_fail[token]);
        }
        break;

    default:
        printf("Invalid format (%d)\n", format);
        break;
    }
}

void tracer_context(void *t, int p, int el, int sf, int ns, \
                    int v, unsigned int vmid,   \
                    int c, unsigned int contextid)
{
    struct etmv4_tracer *tracer = (struct etmv4_tracer *)t;

    if (p) {
        EX_LEVEL(tracer) = el;
        SIXTY_FOUR_BIT(tracer) = sf;
        SECURITY(tracer) = !ns;
        if (v) {
            VMID(tracer) = vmid;
        }
        if (c) {
            CONTEXT_ID(tracer) = contextid;
        }
    }

    printf("Context - Context ID = 0x%X,\n", CONTEXT_ID(tracer));
    printf("          VMID = 0x%X,\n", VMID(tracer));
    printf("          Exception level = EL%d,\n", EX_LEVEL(tracer));
    printf("          Security = %s,\n", (SECURITY(tracer))? "S": "NS");
    printf("          %d-bit instruction\n", (SIXTY_FOUR_BIT(tracer))? 64: 32);
}

void tracer_address(void *t)
{
    struct etmv4_tracer *tracer = (struct etmv4_tracer *)t;
    unsigned long long address = ADDRESS_REGISTER(tracer)[0].address;
    int IS = ADDRESS_REGISTER(tracer)[0].IS;

    if (SIXTY_FOUR_BIT(tracer)) {
        printf("Address - Instruction address 0x%016llx, Instruction set Aarch64\n", address);
    } else {
        if (IS) {
            printf("Address - Instruction address 0x%016llx, Instruction set Aarch32 (ARM)\n", address);
        } else {
            printf("Address - Instruction address 0x%016llx, Instruction set Aarch32 (Thumb)\n", address);
        }
    }
}

void tracer_atom(void *t, int type)
{
    struct etmv4_tracer *tracer = (struct etmv4_tracer *)t;

    if (type == ATOM_TYPE_E) {
        printf("ATOM - E\n");
    } else if (type == ATOM_TYPE_N) {
        printf("ATOM - N\n");
    } else {
        printf("Invalid ATOM type (%d)\n", type);
        return ;
    }

    /*
     * If p0_key_max is zero, it implies that the target CPU uses no P0 right-hand keys.
     * If so, there is no need to update p0_key.
     */
    if (P0_KEY_MAX(tracer)) {
        P0_KEY(tracer)++;
        P0_KEY(tracer) %= P0_KEY_MAX(tracer);
    }

    CURR_SPEC_DEPTH(tracer)++;
    if (!MAX_SPEC_DEPTH(tracer) || (CURR_SPEC_DEPTH(tracer) > MAX_SPEC_DEPTH(tracer))) {
        tracer_commit(tracer, 1);
    }
}

void tracer_q(void *t, unsigned int count)
{
    struct etmv4_tracer *tracer = (struct etmv4_tracer *)t;

    if (count) {
        printf("Q - %d of instructions\n", count);
    } else {
        printf("Q - UNKNOWN of instructions\n");
    }

    /*
     * If p0_key_max is zero, it implies that the target CPU uses no P0 right-hand keys.
     * If so, there is no need to update p0_key.
     */
    if (P0_KEY_MAX(tracer)) {
        P0_KEY(tracer)++;
        P0_KEY(tracer) %= P0_KEY_MAX(tracer);
    }

    CURR_SPEC_DEPTH(tracer)++;
    if (!MAX_SPEC_DEPTH(tracer) || (CURR_SPEC_DEPTH(tracer) > MAX_SPEC_DEPTH(tracer))) {
        tracer_commit(tracer, 1);
    }
}


void tracer_event(void *t, unsigned char e_val)
{
    struct etmv4_tracer *tracer = (struct etmv4_tracer *)t;
    unsigned char mask = 1;
    for(int i = 0; i < 4; i++)
    {
        if(mask & e_val)
            printf("Event - %d\n", i + 1);    //如果第i个事件发生，则输出
        mask = mask << 1;
    }
    // printf("Event - %x", e_val);
}
