#ifndef PCAP_READER_H
#define PCAP_READER_H


#ifdef	__cplusplus
extern "C" {
#endif

typedef void* pcap_file_t;

pcap_file_t pcap_parser_open(const char *filename);
int pcap_parser_get(pcap_file_t file, int packet_id, void *packet_data, int* len);
void pcap_parser_close(pcap_file_t file);


#ifdef	__cplusplus
}
#endif
#endif // PCAP_READER_H
