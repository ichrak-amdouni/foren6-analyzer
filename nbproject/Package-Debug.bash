#!/bin/bash -x

#
# Generated - do not edit!
#

# Macros
TOP=`pwd`
CND_PLATFORM=GNU-Linux-x86
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build
CND_DLIB_EXT=so
NBTMPDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}/tmp-packaging
TMPDIRNAME=tmp-packaging
OUTPUT_PATH=${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libpacket_parser.${CND_DLIB_EXT}
OUTPUT_BASENAME=libpacket_parser.${CND_DLIB_EXT}
PACKAGE_TOP_DIR=libpacketparser.so/

# Functions
function checkReturnCode
{
    rc=$?
    if [ $rc != 0 ]
    then
        exit $rc
    fi
}
function makeDirectory
# $1 directory path
# $2 permission (optional)
{
    mkdir -p "$1"
    checkReturnCode
    if [ "$2" != "" ]
    then
      chmod $2 "$1"
      checkReturnCode
    fi
}
function copyFileToTmpDir
# $1 from-file path
# $2 to-file path
# $3 permission
{
    cp "$1" "$2"
    checkReturnCode
    if [ "$3" != "" ]
    then
        chmod $3 "$2"
        checkReturnCode
    fi
}

# Setup
cd "${TOP}"
mkdir -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/package
rm -rf ${NBTMPDIR}
mkdir -p ${NBTMPDIR}

# Copy files and create directories and links
cd "${TOP}"
makeDirectory "${NBTMPDIR}/libpacketparser.so/bin"
copyFileToTmpDir "${OUTPUT_PATH}" "${NBTMPDIR}/${PACKAGE_TOP_DIR}bin/${OUTPUT_BASENAME}" 0755

cd "${TOP}"
makeDirectory "${NBTMPDIR}/libpacketparser.so"
copyFileToTmpDir "utarray.h" "${NBTMPDIR}/${PACKAGE_TOP_DIR}utarray.h" 0644

cd "${TOP}"
makeDirectory "${NBTMPDIR}/libpacketparser.so"
copyFileToTmpDir "uthash.h" "${NBTMPDIR}/${PACKAGE_TOP_DIR}uthash.h" 0644

cd "${TOP}"
makeDirectory "${NBTMPDIR}/libpacketparser.so"
copyFileToTmpDir "utlist.h" "${NBTMPDIR}/${PACKAGE_TOP_DIR}utlist.h" 0644

cd "${TOP}"
makeDirectory "${NBTMPDIR}/libpacketparser.so"
copyFileToTmpDir "utstring.h" "${NBTMPDIR}/${PACKAGE_TOP_DIR}utstring.h" 0644

cd "${TOP}"
makeDirectory "${NBTMPDIR}/libpacketparser.so/data_collector"
copyFileToTmpDir "data_collector/rpl_collector.h" "${NBTMPDIR}/${PACKAGE_TOP_DIR}data_collector/rpl_collector.h" 0644

cd "${TOP}"
makeDirectory "${NBTMPDIR}/libpacketparser.so/data_collector"
copyFileToTmpDir "data_collector/rpl_event_callbacks.h" "${NBTMPDIR}/${PACKAGE_TOP_DIR}data_collector/rpl_event_callbacks.h" 0644

cd "${TOP}"
makeDirectory "${NBTMPDIR}/libpacketparser.so/data_info"
copyFileToTmpDir "data_info/route.h" "${NBTMPDIR}/${PACKAGE_TOP_DIR}data_info/route.h" 0644

cd "${TOP}"
makeDirectory "${NBTMPDIR}/libpacketparser.so/data_info"
copyFileToTmpDir "data_info/dodag.h" "${NBTMPDIR}/${PACKAGE_TOP_DIR}data_info/dodag.h" 0644

cd "${TOP}"
makeDirectory "${NBTMPDIR}/libpacketparser.so/data_info"
copyFileToTmpDir "data_info/metric.h" "${NBTMPDIR}/${PACKAGE_TOP_DIR}data_info/metric.h" 0644

cd "${TOP}"
makeDirectory "${NBTMPDIR}/libpacketparser.so/data_info"
copyFileToTmpDir "data_info/link.h" "${NBTMPDIR}/${PACKAGE_TOP_DIR}data_info/link.h" 0644

cd "${TOP}"
makeDirectory "${NBTMPDIR}/libpacketparser.so/data_info"
copyFileToTmpDir "data_info/rpl_instance.h" "${NBTMPDIR}/${PACKAGE_TOP_DIR}data_info/rpl_instance.h" 0644

cd "${TOP}"
makeDirectory "${NBTMPDIR}/libpacketparser.so/data_info"
copyFileToTmpDir "data_info/node.h" "${NBTMPDIR}/${PACKAGE_TOP_DIR}data_info/node.h" 0644

cd "${TOP}"
makeDirectory "${NBTMPDIR}/libpacketparser.so/data_info"
copyFileToTmpDir "data_info/address.h" "${NBTMPDIR}/${PACKAGE_TOP_DIR}data_info/address.h" 0644

cd "${TOP}"
makeDirectory "${NBTMPDIR}/libpacketparser.so"
copyFileToTmpDir "rpl_packet_parser.h" "${NBTMPDIR}/${PACKAGE_TOP_DIR}rpl_packet_parser.h" 0644

cd "${TOP}"
makeDirectory "${NBTMPDIR}/libpacketparser.so"
copyFileToTmpDir "sniffer_packet_parser.h" "${NBTMPDIR}/${PACKAGE_TOP_DIR}sniffer_packet_parser.h" 0644


# Generate tar file
cd "${TOP}"
rm -f ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/package/libpacketparser.so.tar
cd ${NBTMPDIR}
tar -vcf ../../../../${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/package/libpacketparser.so.tar *
checkReturnCode

# Cleanup
cd "${TOP}"
rm -rf ${NBTMPDIR}
