#include "parser_register.h"

#include <stdlib.h>
#include <stdbool.h>
#include "../utlist.h"

#include "rpl_parser.h"

typedef struct parser_el {
	parser_t parser;
	struct parser_el *next;
} parser_el_t, *parser_list_t;

parser_list_t parsers;

static void parser_add(parser_t parser);

void parser_register_all() {
	parsers = NULL;
	
	parser_add(rpl_parser_register());
}

void parser_begin_packet() {
	parser_el_t *parser_element;
	
	LL_FOREACH(parsers, parser_element)
		parser_element->parser.begin_packet();
}

void parser_parse_field(const char *nameStr, const char *showStr, const char *valueStr, int64_t valueInt) {
	parser_el_t *parser_element;
	
	LL_FOREACH(parsers, parser_element)
		parser_element->parser.parse_field(nameStr, showStr, valueStr, valueInt);
}

void parser_end_packet() {
	parser_el_t *parser_element;
	
	LL_FOREACH(parsers, parser_element)
		parser_element->parser.end_packet();
}

static void parser_add(parser_t parser) {
	parser_el_t *parser_element;
	
	parser_element = (parser_el_t*) malloc(sizeof(parser_el_t));
	parser_element->parser = parser;
	LL_PREPEND(parsers, parser_element);
}
