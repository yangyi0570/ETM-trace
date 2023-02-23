/*
 * stream.h: header of trace stream
 */
#ifndef _STREAM_H
#define _STREAM_H

enum
{
    READING = 0,
    SYNCING,
    INSYNC,
    DECODING,
    DECODED,
};

struct stream
{
    char *buff;
    unsigned int buff_len;
    int state;
    union
    {
        struct ptm_tracer ptm;
        struct etmv4_tracer etmv4;
    } tracer;
};

extern int decode_stream(struct stream *stream);
extern int decode_etb_stream(struct stream *etb_stream, struct stream *multiple_stream);

#endif
