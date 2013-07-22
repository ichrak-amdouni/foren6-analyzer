#include <pcap/pcap.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <expat.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>

#include "sniffer_packet_parser.h"
#include "descriptor_poll.h"
#include "packet_parsers/parser_register.h"

//Define that to use the new command line format of newer tshark versions
//#define USE_NEW_TSHARK

static bool sniffer_parser_reset_requested = false;

static pcap_t* pd = NULL;
static pcap_dumper_t* pdumper = NULL;
static pcap_t* pd_out = NULL;
static pcap_dumper_t* pdumper_out = NULL;
static FILE* pcap_output = NULL;
static struct timeval start_time;

static int tshark_pid = 0;
static int pipe_tshark_stdin = 0;	//We will write packets here
static int pipe_tshark_stdout = 0;	//We will read dissected packets from here
static pthread_mutex_t new_packet_mutex;

static int packet_count;

static XML_Parser dissected_packet_parser;	//parse output of tshark

static void process_events(int fd, void* data);
static void sniffer_parser_reset();
static bool spawn_piped_process(const char* command, char* const arguments[], int *pid, int *process_input_pipe, int *process_output_pipe);
static void tshark_exited();

static void parse_xml_start_element(void *data, const char *el, const char **attr);
static void parse_xml_end_element(void *data, const char *el);

//Initialize sniffer parser
void sniffer_parser_init() {
	struct sigaction sigchld_action = {
		.sa_handler = SIG_DFL,
		.sa_flags = SA_NOCLDWAIT
	};
	/* prevent zombie child */
	sigaction(SIGCHLD, &sigchld_action, NULL);
	pthread_mutex_init(&new_packet_mutex, NULL);

	parser_register_all();

	sniffer_parser_reset();

	gettimeofday(&start_time, NULL);
}

//Give data to parse to parser
void sniffer_parser_parse_data(const unsigned char* data, int len) {
	struct pcap_pkthdr pkt_hdr;
	struct timeval pkt_time;

	gettimeofday(&pkt_time, NULL);
	if(pkt_time.tv_usec < start_time.tv_usec) {
		pkt_hdr.ts.tv_sec = pkt_time.tv_sec - start_time.tv_sec - 1;
		pkt_hdr.ts.tv_usec = pkt_time.tv_usec + 1000000 - start_time.tv_usec;
	} else {
		pkt_hdr.ts.tv_sec = pkt_time.tv_sec - start_time.tv_sec;
		pkt_hdr.ts.tv_usec = pkt_time.tv_usec - start_time.tv_usec;
	}

	pkt_hdr.caplen = len;
	pkt_hdr.len = len + 2;	//FCS is not captured (2 bytes)
	pthread_mutex_lock(&new_packet_mutex);
	pcap_dump((u_char *)pdumper, &pkt_hdr, data);
	pcap_dump_flush(pdumper);
	pcap_dump((u_char *)pdumper_out, &pkt_hdr, data);
	pcap_dump_flush(pdumper_out);
	fflush(pcap_output);
	pthread_mutex_unlock(&new_packet_mutex);
	//fflush(stdout);
	//fprintf(stderr, "New packet captured\n");
	//write(STDOUT_FILENO, data, len);
}

int sniffer_parser_get_packet_count() {
	return packet_count;
}


static void process_events(int fd, void* data) {
	char buffer[512];
	int nbread;

	if(sniffer_parser_reset_requested) {
		sniffer_parser_reset();
		sniffer_parser_reset_requested = false;
	}

	while(1) {
		nbread = read(pipe_tshark_stdout, buffer, 512);
		if(nbread <= 0) break;


		if(!XML_Parse(dissected_packet_parser, buffer, nbread, false)) {
			buffer[nbread] = 0;
			fprintf(stderr, "Bad XML input: %s\n%s\n",
				buffer,
				XML_ErrorString(XML_GetErrorCode(dissected_packet_parser)));
		}
	}
}

static void parse_xml_start_element(void *data, const char *el, const char **attr) {
	if(!strcmp(el, "packet")) {
		parser_begin_packet();
		packet_count++;
	} else if(!strcmp(el, "field")) {
		/* Parse packet fields */
		int i;
		const char *nameStr = NULL;
		const char *showStr = NULL;
		const char *valueStr = NULL;
		int64_t valueInt = 0;

		for(i = 0; attr[i]; i += 2) {
			if(!strcmp(attr[i], "name"))
				nameStr = attr[i+1];
			else if(!strcmp(attr[i], "show"))
				showStr = attr[i+1];
			else if(!strcmp(attr[i], "value"))
				valueStr = attr[i+1];
		}

		if(valueStr)
			valueInt = strtoll(valueStr, NULL, 16);

		if(nameStr != NULL && showStr != NULL)
			parser_parse_field(nameStr, showStr, valueStr, valueInt);
	}
}

static void parse_xml_end_element(void *data, const char *el) {
	if(!strcmp(el, "packet"))
		parser_end_packet();
}

/*
 * Reset tshark and parser
 */
static void sniffer_parser_reset() {
	if(tshark_pid)
		kill(tshark_pid, SIGKILL);	//Kill old tshark process to avoid multiple spawned processes at the same time

/*
 Closed by pcap_dump_close(pdumper);
	if(pcap_output)
		fclose(pcap_output);
	if(pipe_tshark_stdin)
		close(pipe_tshark_stdin);
*/
	if(pipe_tshark_stdout)
		close(pipe_tshark_stdout);

	if(pd)
		pcap_close(pd);
	if(pdumper)
		pcap_dump_close(pdumper);
	if(pd_out)
		pcap_close(pd_out);
	if(pdumper_out)
		pcap_dump_close(pdumper_out);

	if(dissected_packet_parser)
		XML_ParserFree(dissected_packet_parser);

	tshark_pid = 0;
	pcap_output = NULL;
	pd = NULL;
	pdumper = NULL;
	pd_out = NULL;
	pdumper_out = NULL;
	pipe_tshark_stdin = 0;
	pipe_tshark_stdout = 0;

	packet_count = 0;

#ifdef USE_NEW_TSHARK
	if(spawn_piped_process("/usr/bin/tshark", (char* const[]){"tshark", "-i", "-", "-V", "-T", "pdml", "-2", "-R", "ipv6", "-l", NULL}, &tshark_pid, &pipe_tshark_stdin, &pipe_tshark_stdout) == false) {
#else
	if(spawn_piped_process("/usr/bin/tshark", (char* const[]){"tshark", "-i", "-", "-V", "-T", "pdml", "-R", "ipv6", "-l", NULL}, &tshark_pid, &pipe_tshark_stdin, &pipe_tshark_stdout) == false) {
#endif
		perror("Can't spawn tshark process");
		return;
	}

	pcap_output = fdopen(pipe_tshark_stdin, "w");
	if(pcap_output == NULL) {
		fprintf(stderr, "pipe %d: ", pipe_tshark_stdin);
		perror("Can't open tshark stdin pipe with fopen");
		return;
	}

//	pcap_output = stdout;

	pd = pcap_open_dead(DLT_IEEE802_15_4, 255);
	pdumper = pcap_dump_fopen(pd, pcap_output);

	pd_out = pcap_open_dead(DLT_IEEE802_15_4, 255);
	pdumper_out = pcap_dump_open(pd_out, "out.pcap");

	dissected_packet_parser = XML_ParserCreate(NULL);
	XML_SetElementHandler(dissected_packet_parser, &parse_xml_start_element, &parse_xml_end_element);

	signal(SIGPIPE, &tshark_exited);

	desc_poll_add(pipe_tshark_stdout, &process_events, NULL);
}

static bool spawn_piped_process(const char* command, char* const arguments[], int *pid, int *process_input_pipe, int *process_output_pipe) {
	int child_pid;
	int stdin_pipe[2];
	int stdout_pipe[2];
	static const int PIPE_READ = 0;
	static const int PIPE_WRITE = 1;

	if(pipe(stdin_pipe))
		return false;

	if(pipe(stdout_pipe)) {
		close(stdin_pipe[PIPE_READ]);
		close(stdin_pipe[PIPE_WRITE]);

		return false;
	}

	child_pid = fork();
	if(child_pid == 0) {
		int result;

		//In child process
		dup2(stdin_pipe[PIPE_READ], STDIN_FILENO);
		dup2(stdout_pipe[PIPE_WRITE], STDOUT_FILENO);

		// all these are for use by parent only
		close(stdin_pipe[PIPE_READ]);
		close(stdin_pipe[PIPE_WRITE]);
		close(stdout_pipe[PIPE_READ]);
		close(stdout_pipe[PIPE_WRITE]);

		result = execv(command, arguments);
		perror("Failed to spawn process");

		exit(result);
	} else if(child_pid > 0) {
		close(stdin_pipe[PIPE_READ]);
		close(stdout_pipe[PIPE_WRITE]);

		fcntl(stdout_pipe[PIPE_READ], F_SETFL, fcntl(stdout_pipe[PIPE_READ], F_GETFL, 0) | O_NONBLOCK);

		if(pid) *pid = child_pid;
		if(process_input_pipe) *process_input_pipe = stdin_pipe[PIPE_WRITE];
		if(process_output_pipe) *process_output_pipe = stdout_pipe[PIPE_READ];

		return true;
	} else {
		close(stdin_pipe[PIPE_READ]);
		close(stdin_pipe[PIPE_WRITE]);
		close(stdout_pipe[PIPE_READ]);
		close(stdout_pipe[PIPE_WRITE]);

		perror("Failed to fork");

		return false;
	}
}

/*
 * tshark exited, so restart it (maybe a crash ?)
 * called when SIGPIPE
 */
static void tshark_exited() {
	/* Prevent spawn flood */
	signal(SIGPIPE, SIG_IGN);
	sniffer_parser_reset_requested = true;
	fprintf(stderr, "tshark exited, parser reset requested\n");
}
