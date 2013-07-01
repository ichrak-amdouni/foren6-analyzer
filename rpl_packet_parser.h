/* 
 * File:   rpl_packet_parser.h
 * Author: am
 *
 * Created on June 26, 2013, 2:54 PM
 */

#ifndef RPL_PACKET_PARSER_H
#define	RPL_PACKET_PARSER_H

#include "data_collector/rpl_collector.h"
#include "data_collector/rpl_event_callbacks.h"

#ifdef	__cplusplus
extern "C" {
#endif

	void rpl_tool_init(const char *name, const char *target);
	void rpl_tool_start();
	void rpl_tool_stop();
	rpl_collector_t *rpl_tool_get_collected_data();
	void rpl_tool_set_callbacks(rpl_event_callbacks_t *callbacks);


#ifdef	__cplusplus
}
#endif

#endif	/* RPL_PACKET_PARSER_H */

