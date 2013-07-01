/* 
 * File:   sniffer_interface_read.h
 * Author: am
 *
 * Created on June 12, 2013, 5:17 PM
 */

#ifndef SNIFFER_INTERFACE_READ_H
#define	SNIFFER_INTERFACE_READ_H

//Read incoming data and pass them to sniffer_packet_parser

//Initialize the interface reader. interface_name is the name of the interface to read (implementation defined)
void sniffer_interface_init(const char* interface_name);

#endif	/* SNIFFER_INTERFACE_READ_H */

