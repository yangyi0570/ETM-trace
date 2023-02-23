/*
 * vim:ts=4:sw=4:expandtab
 *
 * ptm.c: Decoding functions of PTM packets
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
#include "pktproto.h"

DEF_TRACEPKT(async, 0xff, 0x00);
DEF_TRACEPKT(isync, 0xff, 0x08);
DEF_TRACEPKT(atom, 0x81, 0x80);
DEF_TRACEPKT(branch_addr, 0x01, 0x01);
DEF_TRACEPKT(waypoint_update, 0xff, 0x72);
DEF_TRACEPKT(trigger, 0xff, 0x0c);
DEF_TRACEPKT(contextid, 0xff, 0x6e);
DEF_TRACEPKT(vmid, 0xff, 0x3c);
DEF_TRACEPKT(timestamp, 0xfb, 0x42);
DEF_TRACEPKT(exception_return, 0xff, 0x76);
DEF_TRACEPKT(ignore, 0xff, 0x66);

DECL_DECODE_FN(async)
{
    int index;

    /* continue until reach the end packet with the binary value b10000000 */
    for (index = 1; index < stream->buff_len; index++) {
        if (pkt[index] == 0x00) {
            continue;
        } else if (pkt[index] == (unsigned char)0x80) {
            printf("[a-sync]\n");
            index++;
            break;
        } else {
            printf("Invalid a-sync packet\n");
            index = -1;
            break;
        }
    }
    if (index == stream->buff_len) {
        printf("Invalid a-sync packet\n");
        index = -1;
    }

    return index;
}

DECL_DECODE_FN(isync)
{
    int i, index, c_bit, reason;
    unsigned int addr = 0, info, cyc_cnt = 0, contextid = 0;
 
    for (i = 0, index = 1; i < 4; i++, index++) {
        addr |= (unsigned int)(pkt[index]) << (8 * i);
    }

    info = pkt[index++];
    reason = (info & 0x60) >> 5;

    /* 
     * The cycle count is present only when cycle-acculate tracing is enabled 
     * AND the reason code is not b00. 
     */
    if (IS_CYC_ACC(&(stream->tracer)) && reason) {
        cyc_cnt = (unsigned int)(pkt[index] & 0x3c) >> 2;
        c_bit = (pkt[index++] & 0x40) >> 6;
        printf("cyc_cnt = 0x%x, c_bit = %d\n", cyc_cnt, c_bit); 
        if (c_bit) {
            for (i = 1; i < 5; i++) {
                cyc_cnt |= (unsigned int)(pkt[index] & 0x7f) << (4 + 7 * (i - 1));
                c_bit = (pkt[index++] & 0x80)? 1: 0;
                printf("cyc_cnt = 0x%x, c_bit = %d\n", cyc_cnt, c_bit); 
                if (!c_bit) {
                    break;
                }
            }
        }
    }

    switch (CONTEXTID_SIZE(&(stream->tracer))) {
    case 1:
        contextid = (unsigned int)(pkt[index++]);
        break;

    case 2:
        for (i = 0; i < 2; i++) {
            contextid |= (unsigned int)(pkt[index++]) << (8 * i);
        }
        break;

    case 4:
        for (i = 0; i < 4; i++) {
            contextid |= (unsigned int)(pkt[index++]) << (8 * i);
        }
        break;

    default:
        break;
    }

    if (stream->state >= DECODING) {
        printf("[i-sync] addr = 0x%x (%s), ", addr & ~0x1, (addr & 0x1)? "thumb": "arm");
        printf("info = |reason %d|NS %d|AltIS %d|Hyp %d|, ",  \
                    reason, \
                    (info & 0x08)? 1: 0,    \
                    (info & 0x04)? 1: 0,    \
                    (info & 0x02)? 1: 0);
        if (IS_CYC_ACC(&(stream->tracer))) {
            printf("cycle count = 0x%x, ", cyc_cnt);
        }
        if (CONTEXTID_SIZE(&(stream->tracer))) {
            printf("context id = 0x%x, ", contextid);
        }
        printf("\n");

        tracer_sync(&(stream->tracer), addr & ~1, (addr & 1)? THUMB_STATE: ARM_STATE,   \
                    info, cyc_cnt, contextid);
    }

    return index;
}

DECL_DECODE_FN(atom)
{
    int i, c_bit, F_bit, index, cnt;
    unsigned int cyc_cnt;
    static const char F_bit_mask = 0x02, F_bit_shift = 1;

    if (IS_CYC_ACC(&(stream->tracer))) {
        index = 0;
        cyc_cnt = (unsigned int)(pkt[index] & 0x3c) >> 2;
        c_bit = (pkt[index] & 0x40) >> 6;
        F_bit = (pkt[index++] & F_bit_mask) >> F_bit_shift;
        printf("cyc_cnt = 0x%x, c_bit = %d\n", cyc_cnt, c_bit); 
        if (c_bit) {
            for (i = 1; i < 5; i++) {
                cyc_cnt |= (unsigned int)(pkt[index] & 0x7f) << (4 + 7 * (i - 1));
                c_bit = (pkt[index++] & 0x80)? 1: 0;
                printf("cyc_cnt = 0x%x, c_bit = %d\n", cyc_cnt, c_bit); 
                if (!c_bit) {
                    break;
                }
            } 
        }

        printf("[%s atom] cycle count = 0x%x\n", (F_bit)? "N": "E", cyc_cnt);

        return index;
    } else {
        cnt = 5;
        for (i = 0x40; i > 0x02; i >>= 1) {
            if (pkt[0] & i ){
                break;
            }
            cnt--;
        }
        if (!cnt){
             printf("Invalid atom packet");
             return 1;
        } else {
             printf("Find %d atoms\n", cnt);
        }

        for (i = 1; i < (cnt + 1); i++) {
            if (pkt[0] & (1 << i)) {
                printf("[N atom]\n");
            } else {
                printf("[E atom]\n");
            }
        }

        return 1;
    }
}

DECL_DECODE_FN(branch_addr)
{
    int index, full_addr, addr_size, inst_state, have_exp = 0, NS = 0, Hyp = 0, i, c_bit;
    unsigned int addr, exp = 0, cyc_cnt = 0;

    index = 0;
    addr = 0;
    full_addr = 0;
    addr_size = 0;
    inst_state = NOT_CHANGE;
    do {
        if (index == 0) {
            addr_size = 6;
            addr = (unsigned int)(pkt[index] & 0x7e) >> 1;
            if (!(pkt[index++] & 0x80)) {
                break;
            }
        } else if (index >= 1 && index <= 3) {
            addr_size = 6 + (7 * index) - 1;
            addr |= (unsigned int)(pkt[index] & 0x7f) << (6 + 7 * (index - 1));
            if (!(pkt[index++] & 0x80)) {
                break;
            }
        } else if (index == 4) {
            full_addr = 1;
            if (pkt[index] & 0x20) {
                /* Jazelle state */
                addr_size += 5;
                inst_state = JAZELLE_STATE;
                addr |= (unsigned int)(pkt[index] & 0x1f) << 27;
            } else if (pkt[index] & 0x10) {
                /* Thumb state */
                addr_size += 4;
                inst_state = THUMB_STATE;
                addr |= (unsigned int)(pkt[index] & 0x0f) << 27;
                addr <<= 1;
            } else {
                /* ARM state */
                addr_size += 3;
                inst_state = ARM_STATE;
                addr |= (unsigned int)(pkt[index] & 0x07) << 27;
                addr <<= 2;
            }
            if (!(pkt[index++] & 0x40)) {
                break;
            } else {
                have_exp = 1;
                NS = (pkt[index] & 0x01)? 1: 0;
                exp = (pkt[index] & 0x1E) >> 1;
                if (pkt[index++] & 0x80) {
                    Hyp = (pkt[index] & 0x20)? 1: 0;
                    exp |= (pkt[index] & 0x1F) << 4;
                }
            }
        }
    } while (index < 5);

    if (IS_CYC_ACC(&(stream->tracer))) {
        cyc_cnt = (unsigned int)(pkt[index] & 0x3c) >> 2;
        c_bit = (pkt[index++] & 0x40) >> 6;
        printf("cyc_cnt = 0x%x, c_bit = %d\n", cyc_cnt, c_bit); 
        if (c_bit) {
            for (i = 1; i < 5; i++) {
                cyc_cnt |= (unsigned int)(pkt[index] & 0x7f) << (4 + 7 * (i - 1));
                c_bit = (pkt[index++] & 0x80)? 1: 0;
                printf("cyc_cnt = 0x%x, c_bit = %d\n", cyc_cnt, c_bit); 
                if (!c_bit) {
                    break;
                }
            }
        }
    }

    printf("[branch address] ");
    if (full_addr) {
        printf("addr = 0x%x, ", addr);
    } else {
        printf("addr change = 0x%x * n(n=4 for ARM state, n=2 for Thumb state), ", addr);
        printf("addr_size = %d, ", addr_size);
    }
    if (have_exp) {
        printf("info = |exception %d|NS %d|Hyp %d|, ", exp, NS, Hyp);
    }
    if (IS_CYC_ACC(&(stream->tracer))) {
        printf("cycle count = 0x%x, ", cyc_cnt);
    }
    printf("\n");

    tracer_branch(&(stream->tracer), addr, full_addr? MAX_NR_ADDR_BIT: addr_size, inst_state,    \
                    exp, NS, Hyp, cyc_cnt);

    return index;
}

DECL_DECODE_FN(waypoint_update)
{
    int index, full_addr, addr_size, inst_state, AltS = -1;
    unsigned int addr;

    index = 1;
    addr = 0;
    full_addr = 0;
    addr_size = 0;
    inst_state = NOT_CHANGE;
    do {
        if (index == 1) {
            addr_size = 6;
            addr = (unsigned int)(pkt[index] & 0x7e) >> 1;
            if (!(pkt[index++] & 0x80)) {
                break;
            }
        } else if (index >= 2 && index <= 4) {
            addr_size = 6 + 7 * (index - 1) - 1;
            addr |= (unsigned int)(pkt[index] & 0x7f) << (6 + 7 * (index - 2));
            if (!(pkt[index++] & 0x80)) {
                break;
            }
        } else if (index == 5) {
            full_addr = 1;
            if (pkt[index] & 0x10) {
                /* Thumb state */
                addr_size += 4;
                inst_state = THUMB_STATE;
                addr |= (unsigned int)(pkt[index] & 0x0f) << 27;
                addr <<= 1;
                if (pkt[index++] & 0x40) {
                    AltS = (pkt[index] & 0x40)? 1: 0;
                    index++;
                    break;
                }
            } else {
                /* ARM state */
                addr_size += 4;
                inst_state = ARM_STATE;
                addr |= (unsigned int)(pkt[index] & 0x07) << 27;
                addr <<= 2;
                if (pkt[index++] & 0x40) {
                    index++;
                    break;
                }
            }
        }
    } while (index < 7);

    printf("[waypoint update] ");
    if (full_addr) {
        printf("addr = 0x%x, ", addr);
    } else {
        printf("addr change = 0x%x * n(n=4 for ARM state and n=2 for Thumb state) ", addr);
    }
    if (AltS != -1) {
        printf("AltS = %d, ", AltS);
    }
    printf("\n");

    tracer_waypoint(&(stream->tracer), addr, full_addr? MAX_NR_ADDR_BIT: addr_size,    \
                        inst_state, AltS);

    return index;
}

DECL_DECODE_FN(trigger)
{
    printf("[trigger]\n");

    return 1;
}

DECL_DECODE_FN(contextid)
{
    unsigned int contextid = 0;
    int i, index = 1;

    switch (CONTEXTID_SIZE(&(stream->tracer))) {
    case 1:
        contextid = (unsigned int)(pkt[index++]);
        break;

    case 2:
        for (i = 0; i < 2; i++) {
            contextid |= (unsigned int)(pkt[index++]) << (8 * i);
        }
        break;

    case 4:
        for (i = 0; i < 4; i++) {
            contextid |= (unsigned int)(pkt[index++]) << (8 * i);
        }
        break;

    default:
        break;
    }

    printf("[context ID] context ID = 0x%x\n", contextid);

    tracer_contextid(&(stream->tracer), contextid);

    return index;
}

DECL_DECODE_FN(vmid)
{
    printf("[VMID] VMID = 0x%02x\n", pkt[1]);

    tracer_vmid(&(stream->tracer), pkt[1]);

    return 2;
}

DECL_DECODE_FN(timestamp)
{
    unsigned long long timestamp;
    int index, c_bit, i;
    unsigned int cyc_cnt = 0;

    for (index = 1, timestamp = 0; index < 9; index++) {
        timestamp |= (unsigned long long)(pkt[index] & 0x7f) << (7 * (index - 1));
        if (!(pkt[index] & 0x80)) {
            index++;
            break;
        }
    }
    if (index == 9) {
        timestamp |= (unsigned long long)(pkt[index]) << 56;
        index++;
    }

    if (IS_CYC_ACC(&(stream->tracer))) {
        cyc_cnt = (unsigned int)(pkt[index] & 0x3c) >> 2;
        c_bit = (pkt[index++] & 0x40) >> 6;
        printf("cyc_cnt = 0x%x, c_bit = %d\n", cyc_cnt, c_bit); 
        if (c_bit) {
            for (i = 1; i < 5; i++) {
                cyc_cnt |= (unsigned int)(pkt[index] & 0x7f) << (4 + 7 * (i - 1));
                c_bit = (pkt[index++] & 0x80)? 1: 0;
                printf("cyc_cnt = 0x%x, c_bit = %d\n", cyc_cnt, c_bit); 
                if (!c_bit) {
                    break;
                }
            }
        }
    }

    printf("[timestamp] timestamp = 0x%llx ", timestamp);
    if (IS_CYC_ACC(&(stream->tracer))) {
        printf("cycle count = 0x%x, ", cyc_cnt);
    }
    printf("\n");

    tracer_timestamp(&(stream->tracer), timestamp, cyc_cnt);

    return index;
}

DECL_DECODE_FN(exception_return)
{
    printf("[exception return]\n");

    tracer_exception_ret(&(stream->tracer));

    return 1;
}

DECL_DECODE_FN(ignore)
{
    printf("[ignore]\n");

    return 1;
}

struct tracepkt *ptmpkts[] =
{
    &PKT_NAME(async),
    &PKT_NAME(isync),
    &PKT_NAME(atom),
    &PKT_NAME(branch_addr),
    &PKT_NAME(waypoint_update),
    &PKT_NAME(trigger),
    &PKT_NAME(contextid),
    &PKT_NAME(vmid),
    &PKT_NAME(timestamp),
    &PKT_NAME(exception_return),
    &PKT_NAME(ignore),
    NULL,
};

int ptm_synchronization(struct stream *stream)
{
    int i, p;
    unsigned char c;

    for (i = 0; i < stream->buff_len; i++) {
        c = stream->buff[i];
        if ((c & PKT_NAME(isync).mask) == PKT_NAME(isync).val) {
            p = DECODE_FUNC_NAME(isync)((const unsigned char *)&(stream->buff[i]), stream);
            if (p > 0) {
                /* SYNCING -> INSYNC */
                stream->state++;
                return i;
            }
        }
    }

    return -1;
}

void decode_ptm(void)
{
    tracepkts = ptmpkts;
    synchronization = ptm_synchronization;
}
