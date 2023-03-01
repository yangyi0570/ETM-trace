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
    struct stream stream, *multiple_stream = NULL;

    memset(&stream, 0, sizeof(struct stream));
    pkttype = PKT_TYPE_ETMV4;
    decode_etmv4(); 

    COND_KEY_MAX_INCR(&(stream.tracer)) = trcidr12 - trcidr13;

    stream.buff = trace_data;
    stream.buff_len = data_size;

    // 如果跟踪数据最后结尾的0xffffffff，并且在一个stream里传入多段，应该在传入时把多次跟踪截断。
    int nr_stream = decode_etb_stream(&stream, multiple_stream);
    if(nr_stream == -1) return -1;
    printf("decode_etb_stream finished.\n");
    
    for (int i = 0; i < nr_stream; i++) {
        // printf("There are %d bytes in the stream %d\n", multiple_stream[i].buff_len, i);
        // if (multiple_stream[i].buff_len != 0) {
        //     printf("Decode trace stream of ID %d\n", i);
        //     decode_stream(&(multiple_stream[i]));
        // } else {
        //     printf("There is no valid data in the stream of ID %d\n", i);
        // }
        free(multiple_stream[i].buff);
    }
    free(multiple_stream);
    printf("decode_stream finished.\n");

    free(stream.buff);

    return 0;
}
