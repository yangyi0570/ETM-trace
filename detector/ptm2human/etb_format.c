/*
 * etb_format.c: Decode ETB
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "tracer.h"
#include "stream.h"

#define ETB_PACKET_SIZE 16
#define NULL_TRACE_SOURCE 0

/**
 * 创建一个stream如下：
 * stream.buff指向新开辟的一块大小等同于parent中buff的空间，同时内容memset为0
 * stream.buff_len = 0
 * stream.stat和stream.tracer与parent中的相同
 */
int init_stream(struct stream *stream, struct stream *parent)
{
    if (!stream) {
        printf("Invalid stream pointer\n");
        return -1;
    }

    memcpy(stream, parent, sizeof(struct stream));

    stream->buff_len  = 0;

    stream->buff = malloc(parent->buff_len);
    if (!(stream->buff)) {
        printf("Fail to allocate memory \n");
        return -1;
    }

    memset((void *)stream->buff, 0, parent->buff_len);

    return 0;
}

int decode_etb_stream(struct stream *etb_stream, struct stream *multiple_stream)
{
    struct stream *stream;
    int ret = 0;
    int nr_stream, pkt_idx, byte_idx, id, cur_id, pre_id, nr_new, i, trace_stop = 0;
    unsigned char c, end, tmp;

    if (!etb_stream) {
        printf("Invalid stream pointer\n");
        return -1;
    }

    /* create the first stream */
    cur_id = -1;
    pre_id = -1;
    nr_stream = 1;
    stream = malloc(sizeof(struct stream));
    if (!stream) {
        printf("Fail to allocate stream\n");
        return -1;
    }
    if (init_stream(stream, etb_stream)) {
        ret = -1;
        goto exit_decode_etb_stream;
    }

    for (pkt_idx = 0; pkt_idx < etb_stream->buff_len; pkt_idx += ETB_PACKET_SIZE) {//一个包16字节
        if (trace_stop) {   //跟踪停止的标志
            break;
        }
        end = etb_stream->buff[pkt_idx + ETB_PACKET_SIZE - 1];  //end指向当前处理的包的最后一个字节
        for (byte_idx = 0; byte_idx < (ETB_PACKET_SIZE - 1); byte_idx++) {  //包内字节遍历
            c = etb_stream->buff[pkt_idx + byte_idx];   //c为当前指向的byte
            if (byte_idx & 1) { //包内的奇数个byte，对应于data，判断前一个字节的原因在于要确定该字节的data属于哪个id。
                /* data byte */
                tmp = etb_stream->buff[pkt_idx + byte_idx - 1];
                if ((tmp & 1) &&    /* previous byte is an ID byte */   \
                        end & (1 << (byte_idx / 2))) {
                    /* data corresponds to the previous ID */
                    if (pre_id < 0) {
                        /* drop the byte since there is no ID byte yet */
                        continue;
                    }
                    stream[pre_id].buff[stream[pre_id].buff_len] = c;
                    stream[pre_id].buff_len = stream[pre_id].buff_len + 1;
                } else {
                    /* data corresponds to the new ID */
                    if (cur_id < 0) {
                        /* drop the byte since there is no ID byte yet */
                        continue;
                    }
                    stream[cur_id].buff[stream[cur_id].buff_len] = c;
                    stream[cur_id].buff_len = stream[cur_id].buff_len + 1;
                }
            } else {
                if (c & 1) {    //包内的第偶数个byte，且该byte最后一位为1
                    /* ID byte */
                    id = (c >> 1) & 0x7f;
                    if (id == NULL_TRACE_SOURCE) {
                        printf("Found a NULL_TRACE_SOURCE ID in the ETB data packet\n");
                        trace_stop = 1;
                        break;
                    } else {
                        pre_id = cur_id;
                        cur_id = id - 1;
                    }

                    if (cur_id >= nr_stream) {
                        /* create new streams */
                        nr_new = cur_id - nr_stream + 1;//有多少需要新创建的stream
                        nr_stream = cur_id + 1; //有多少个stream
                        stream = realloc(stream, sizeof(struct stream) * nr_stream);
                        if (!stream) {
                            printf("Fail to re-allocate stream\n");
                            ret = -1;
                            goto exit_decode_etb_stream;
                        }
                        for (i = (nr_stream - nr_new); i < nr_stream; i++) {
                            if (init_stream(&(stream[i]), etb_stream)) {
                                printf("Fail to init stream %d\n", i);
                                ret = -1;
                                goto exit_decode_etb_stream;
                            }
                        }
                    }
                } else {
                    /* data byte */
                    c |= (end & (1 << (byte_idx / 2)))? 1: 0;
                    if (cur_id < 0) {
                        /* drop the byte since there is no ID byte yet */
                        continue;
                    }
                    stream[cur_id].buff[stream[cur_id].buff_len] = c; //向指定id的stream.buff中写入数据字节
                    stream[cur_id].buff_len = stream[cur_id].buff_len + 1;
                }
            }
        }
    }

    multiple_stream = stream;
    return ret;

exit_decode_etb_stream:
    free(stream);
    return ret;
}
