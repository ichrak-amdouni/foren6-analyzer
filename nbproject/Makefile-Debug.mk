#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_DLIB_EXT=so
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/1968008247/circular_buffer.o \
	${OBJECTDIR}/_ext/1968008247/descriptor_poll.o \
	${OBJECTDIR}/_ext/730068611/rpl_parser.o \
	${OBJECTDIR}/_ext/1968008247/property.o \
	${OBJECTDIR}/data_collector/rpl_collector.o \
	${OBJECTDIR}/data_collector/rpl_event_callbacks.o \
	${OBJECTDIR}/data_info/address.o \
	${OBJECTDIR}/data_info/dodag.o \
	${OBJECTDIR}/data_info/link.o \
	${OBJECTDIR}/data_info/metric.o \
	${OBJECTDIR}/data_info/node.o \
	${OBJECTDIR}/data_info/route.o \
	${OBJECTDIR}/data_info/rpl_instance.o \
	${OBJECTDIR}/interface_readers/interface_telos.o \
	${OBJECTDIR}/interface_readers/interfaces_register.o \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/packet_parsers/parser_register.o \
	${OBJECTDIR}/rpl_packet_parser.o \
	${OBJECTDIR}/sniffer_packet_parser.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-lpcap -lexpat -lpthread

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libpacket_parser.${CND_DLIB_EXT}

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libpacket_parser.${CND_DLIB_EXT}: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.c} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libpacket_parser.${CND_DLIB_EXT} ${OBJECTFILES} ${LDLIBSOPTIONS} -shared -fPIC

${OBJECTDIR}/_ext/1968008247/circular_buffer.o: /home/am/NetBeansProjects/packet_parser/circular_buffer.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1968008247
	${RM} $@.d
	$(COMPILE.c) -g -Wall -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1968008247/circular_buffer.o /home/am/NetBeansProjects/packet_parser/circular_buffer.c

${OBJECTDIR}/_ext/1968008247/descriptor_poll.o: /home/am/NetBeansProjects/packet_parser/descriptor_poll.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1968008247
	${RM} $@.d
	$(COMPILE.c) -g -Wall -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1968008247/descriptor_poll.o /home/am/NetBeansProjects/packet_parser/descriptor_poll.c

${OBJECTDIR}/_ext/730068611/rpl_parser.o: /home/am/NetBeansProjects/packet_parser/packet_parsers/rpl_parser.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/730068611
	${RM} $@.d
	$(COMPILE.c) -g -Wall -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/730068611/rpl_parser.o /home/am/NetBeansProjects/packet_parser/packet_parsers/rpl_parser.c

${OBJECTDIR}/_ext/1968008247/property.o: /home/am/NetBeansProjects/packet_parser/property.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1968008247
	${RM} $@.d
	$(COMPILE.c) -g -Wall -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1968008247/property.o /home/am/NetBeansProjects/packet_parser/property.c

${OBJECTDIR}/data_collector/rpl_collector.o: data_collector/rpl_collector.c 
	${MKDIR} -p ${OBJECTDIR}/data_collector
	${RM} $@.d
	$(COMPILE.c) -g -Wall -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/data_collector/rpl_collector.o data_collector/rpl_collector.c

${OBJECTDIR}/data_collector/rpl_event_callbacks.o: data_collector/rpl_event_callbacks.c 
	${MKDIR} -p ${OBJECTDIR}/data_collector
	${RM} $@.d
	$(COMPILE.c) -g -Wall -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/data_collector/rpl_event_callbacks.o data_collector/rpl_event_callbacks.c

${OBJECTDIR}/data_info/address.o: data_info/address.c 
	${MKDIR} -p ${OBJECTDIR}/data_info
	${RM} $@.d
	$(COMPILE.c) -g -Wall -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/data_info/address.o data_info/address.c

${OBJECTDIR}/data_info/dodag.o: data_info/dodag.c 
	${MKDIR} -p ${OBJECTDIR}/data_info
	${RM} $@.d
	$(COMPILE.c) -g -Wall -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/data_info/dodag.o data_info/dodag.c

${OBJECTDIR}/data_info/link.o: data_info/link.c 
	${MKDIR} -p ${OBJECTDIR}/data_info
	${RM} $@.d
	$(COMPILE.c) -g -Wall -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/data_info/link.o data_info/link.c

${OBJECTDIR}/data_info/metric.o: data_info/metric.c 
	${MKDIR} -p ${OBJECTDIR}/data_info
	${RM} $@.d
	$(COMPILE.c) -g -Wall -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/data_info/metric.o data_info/metric.c

${OBJECTDIR}/data_info/node.o: data_info/node.c 
	${MKDIR} -p ${OBJECTDIR}/data_info
	${RM} $@.d
	$(COMPILE.c) -g -Wall -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/data_info/node.o data_info/node.c

${OBJECTDIR}/data_info/route.o: data_info/route.c 
	${MKDIR} -p ${OBJECTDIR}/data_info
	${RM} $@.d
	$(COMPILE.c) -g -Wall -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/data_info/route.o data_info/route.c

${OBJECTDIR}/data_info/rpl_instance.o: data_info/rpl_instance.c 
	${MKDIR} -p ${OBJECTDIR}/data_info
	${RM} $@.d
	$(COMPILE.c) -g -Wall -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/data_info/rpl_instance.o data_info/rpl_instance.c

${OBJECTDIR}/interface_readers/interface_telos.o: interface_readers/interface_telos.c 
	${MKDIR} -p ${OBJECTDIR}/interface_readers
	${RM} $@.d
	$(COMPILE.c) -g -Wall -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/interface_readers/interface_telos.o interface_readers/interface_telos.c

${OBJECTDIR}/interface_readers/interfaces_register.o: interface_readers/interfaces_register.c 
	${MKDIR} -p ${OBJECTDIR}/interface_readers
	${RM} $@.d
	$(COMPILE.c) -g -Wall -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/interface_readers/interfaces_register.o interface_readers/interfaces_register.c

${OBJECTDIR}/main.o: main.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -Wall -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/main.o main.c

${OBJECTDIR}/packet_parsers/parser_register.o: packet_parsers/parser_register.c 
	${MKDIR} -p ${OBJECTDIR}/packet_parsers
	${RM} $@.d
	$(COMPILE.c) -g -Wall -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/packet_parsers/parser_register.o packet_parsers/parser_register.c

${OBJECTDIR}/rpl_packet_parser.o: rpl_packet_parser.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -Wall -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/rpl_packet_parser.o rpl_packet_parser.c

${OBJECTDIR}/sniffer_packet_parser.o: sniffer_packet_parser.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -Wall -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/sniffer_packet_parser.o sniffer_packet_parser.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libpacket_parser.${CND_DLIB_EXT}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
