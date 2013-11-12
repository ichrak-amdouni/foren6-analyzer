/*
 * Copyright (c) 2013, CETIC.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/**
 * \file
 *         PCAP file reader, common for all input interfaces.
 * \author
 *         Foren6 Team <foren6@cetic.be>
 *         http://cetic.github.io/foren6/credits.html
 */

#include "pcap_reader.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

struct pcap_file {
    FILE *file;
    int *packet_pos_infile;
    int packet_count;
    int array_size;
};

typedef struct pcap_hdr_s {
    uint32_t magic_number;      /* magic number */
    uint16_t version_major;     /* major version number */
    uint16_t version_minor;     /* minor version number */
    int32_t thiszone;           /* GMT to local correction */
    uint32_t sigfigs;           /* accuracy of timestamps */
    uint32_t snaplen;           /* max length of captured packets, in octets */
    uint32_t network;           /* data link type */
} pcap_hdr_t;

typedef struct pcaprec_hdr_s {
    uint32_t ts_sec;            /* timestamp seconds */
    uint32_t ts_usec;           /* timestamp microseconds */
    uint32_t incl_len;          /* number of octets of packet saved in file */
    uint32_t orig_len;          /* actual length of packet */
} pcaprec_hdr_t;

pcap_file_t
pcap_parser_open(const char *filename)
{
    FILE *fileptr;
    pcaprec_hdr_t header;
    struct pcap_file *file;

    fileptr = fopen(filename, "rb");
    if(fileptr == NULL)
        return NULL;

    file = malloc(sizeof(struct pcap_file));
    file->file = fileptr;

    fseek(file->file, sizeof(pcap_hdr_t), SEEK_SET);

    file->packet_count = 0;
    file->array_size = 0;
    file->packet_pos_infile = NULL;

    while(fread(&header, 1, sizeof(header), file->file) == sizeof(header)) {
        if(file->packet_count >= file->array_size) {
            if(file->array_size == 0)
                file->array_size = 1;
            else
                file->array_size *= 2;
            file->packet_pos_infile =
                realloc(file->packet_pos_infile,
                        file->array_size * sizeof(int));
        }
        file->packet_pos_infile[file->packet_count] =
            ftell(file->file) - sizeof(header);
        file->packet_count++;

        fseek(file->file, header.incl_len, SEEK_CUR);
    }

    return file;
}

int
pcap_parser_get(pcap_file_t file, int packet_id, void *packet_data, int *len)
{
    struct pcap_file *currentFile = file;
    pcaprec_hdr_t header;

    fseek(currentFile->file, currentFile->packet_pos_infile[packet_id],
          SEEK_SET);
    fread(&header, 1, sizeof(header), currentFile->file);

    if(packet_data == NULL || *len > header.incl_len)
        *len = header.incl_len;

    if(packet_data)
        *len = fread(packet_data, 1, *len, currentFile->file);

    return 0;
}


void
pcap_parser_close(pcap_file_t file)
{
    struct pcap_file *currentFile = file;

    fclose(currentFile->file);

    free(currentFile->packet_pos_infile);
}
