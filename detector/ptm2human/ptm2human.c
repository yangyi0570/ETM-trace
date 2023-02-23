/*
 * vim:ts=4:sw=4:expandtab
 *
 * ptm2human.c: Entry
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
#include "ptm2human.h"

int ptm2human(char* trace_data, unsigned int data_size)
{
    int longindex, c, ret, pkttype = -1;
    char *endptr;
    long val;
    unsigned int trcidr12 = 0, trcidr13 = 0;
    //stream是一个以字符数组形式保存了跟踪数据文件内容的buffer
    struct stream stream;

    memset(&stream, 0, sizeof(struct stream));
    pkttype = PKT_TYPE_ETMV4;
    //tracepkts = etmv4pkts
    //synchronization = etmv4_synchronization
    decode_etmv4(); 

    if (pkttype == -1) {
        pkttype = PKT_TYPE_PTM;
        decode_ptm();
    }

    /* validate context ID size */
    switch (CONTEXTID_SIZE(&(stream.tracer))) {
    case 0:
    case 1:
    case 2:
    case 4:
        break;
    default:
        printf("Invalid context ID size %d\n", CONTEXTID_SIZE(&(stream.tracer)));
        return EXIT_FAILURE;
        break;
    }

    /* validate CONDTYPE in trcidr0 */
    if (CONDTYPE(&(stream.tracer)) > 2) {
        printf("Invalid CONDTYPE in TRCIDR0: %d (should be either 0 or 1)\n", CONDTYPE(&(stream.tracer)));
        return EXIT_FAILURE;
    }

    /* validate trcidr12 and trcidr13 */
    if (trcidr12 < trcidr13) {
        printf("Invalid TRCIDR12/TRCIDR13: TRCIDR12 (%d) < TRCIDR13 (%d)\n", trcidr12, trcidr13);
        return EXIT_FAILURE;
    } else {
        COND_KEY_MAX_INCR(&(stream.tracer)) = trcidr12 - trcidr13;
    }

    stream.buff = trace_data;
    stream.buff_len = data_size;

    ret = decode_etb_stream(&stream);

    if (ret) {
        return EXIT_FAILURE;
    } else {
        return EXIT_SUCCESS;
    }
}
