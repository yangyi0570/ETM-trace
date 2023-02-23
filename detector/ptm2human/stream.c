/*
 * vim:ts=4:sw=4:expandtab
 *
 * stream.c: synchronize trace stream and decode it
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

/**
 * tracepkt是包类型信息
 * synchronization是同步函数，用于寻找A-Sync包
 * 在调用ptm2human时指定etmv4还是ptm，调用decode_etmv4或decode_ptm函数将值写入
 */
struct tracepkt **tracepkts;
sync_func synchronization;  

int decode_stream(struct stream *stream)
{
    // cur为当前指向的字节位置
    int cur, i, ret;

    if (!stream) {
        printf("Invalid struct stream pointer\n");
        return -1;
    }
    if (stream->state == READING) {
        /* READING -> SYNCING */
        stream->state++;
    } else {
        printf("Stream state is not correct\n");
        return -1;
    }

    printf("Syncing the trace stream...\n");
    cur = synchronization(stream);  //返回async包的字节位置
    if (cur < 0) {
        printf("Cannot find any synchronization packet\n");
        return -1;
    } else {
        printf("Trace starts from offset %d\n", cur);
    }

    printf("Decoding the trace stream...\n");
    /* INSYNC -> DECODING */
    stream->state++;
    for (; cur < stream->buff_len; ) {
        unsigned char c = stream->buff[cur];    //当前指向的字节内容

        printf("Got a packet header 0x%02x at offset %d\n", c, cur);

        for (i = 0; tracepkts[i]; i++) {        //遍历tracepkts搜索匹配的包类型
            if ((c & tracepkts[i]->mask) == tracepkts[i]->val) {
                printf("Get a packet of type %s\n", tracepkts[i]->name);
                break;
            }
        }
        if (!tracepkts[i]) {    //没找到对应的包类型
            printf("Cannot recognize a packet header 0x%02x\n", c);
            printf("Proceed on guesswork\n");
            cur++;
            continue;
        }

        ret = tracepkts[i]->decode((const unsigned char *)&(stream->buff[cur]), stream);    //ret为当前包的大小
        if (ret <= 0) {
            printf("Cannot decode a packet of type %s at offset %d\n", tracepkts[i]->name, cur);
            printf("Proceed on guesswork\n");
            cur++;
        } else {
            cur += ret;
        }
    }

    printf("Complete decode of the trace stream\n");

    return 0;
}
