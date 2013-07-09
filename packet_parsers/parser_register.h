/* 
 * File:   parser_register.h
 * Author: am
 *
 * Created on June 18, 2013, 4:40 PM
 */

#ifndef PARSER_REGISTER_H
#define	PARSER_REGISTER_H

#include <stdint.h>

typedef struct parser {
	const char *parser_name;
	void (*begin_packet)();
	void (*parse_field)(const char *nameStr, const char *showStr, const char *valueStr, int64_t valueInt);
	void (*end_packet)();
} parser_t;

void parser_register_all();

void parser_begin_packet();
void parser_parse_field(const char *nameStr, const char *showStr, const char *valueStr, int64_t valueInt);
void parser_end_packet();

#endif	/* PARSER_REGISTER_H */

