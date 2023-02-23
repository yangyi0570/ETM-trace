#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <getopt.h>
#include <stdint.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include "tracer.h"
#include "stream.h"
#include "pktproto.h"

int ptm2human(char* trace_data, unsigned int data_size);