/*
 * File:   rpl_packet_parser.h
 * Author: am
 *
 * Created on June 26, 2013, 2:54 PM
 */

#ifndef RPL_PACKET_PARSER_H
#define	RPL_PACKET_PARSER_H

#include "data_info/rpl_data.h"
#include "data_collector/rpl_event_callbacks.h"
#include "interface_reader/interfaces_mgr.h"

#ifdef	__cplusplus
extern "C" {
#endif

typedef struct analyzer_callbacks {
    void (*onErrorEvent)(char const * errorMessage);
} analyzer_callbacks_t;

	/**
	 * Initialize the analyzer.
     * @param name the name of a registered interface reader, or the shared object file that provide the needed interface
     * @param target a target where to read data from the sniffer like /dev/ttyUSB0 for the telos interface
     */
	void rpl_tool_init();

	void rpl_tool_cleanup();

	interface_t *rpl_tool_get_interface(const char *name);

	/**
	 * Set callback to call when events are triggered (like node creation)
     * @param callbacks
     */
	void rpl_tool_set_callbacks(rpl_event_callbacks_t *callbacks);

    /**
     * Set callback to call when analyzer events are triggered (like error reporting)
     * @param callbacks
     */
	void rpl_tool_set_analyzer_callbacks(analyzer_callbacks_t * callbacks);

	void rpl_tool_report_error(char const *  error_message);

#ifdef	__cplusplus
}
#endif

#endif	/* RPL_PACKET_PARSER_H */

