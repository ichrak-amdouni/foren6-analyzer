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
 *         Parser Registration
 * \author
 *         Foren6 Team <foren6@cetic.be>
 *         http://cetic.github.io/foren6/credits.html
 */

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
