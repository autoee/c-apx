#ifndef APX_ERROR_H
#define APX_ERROR_H

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include <stdint.h>

//////////////////////////////////////////////////////////////////////////////
// CONSTANTS AND DATA TYPES
//////////////////////////////////////////////////////////////////////////////
#define APX_GENERIC_ERROR             -1
#define APX_NO_ERROR                  0
#define APX_INVALID_ARGUMENT_ERROR    1
#define APX_MEM_ERROR                 2
#define APX_PARSE_ERROR               3
#define APX_DATA_SIGNATURE_ERROR      4
#define APX_PORT_SIGNATURE_ERROR      5
#define APX_VALUE_ERROR               6
#define APX_LENGTH_ERROR              7
#define APX_ELEMENT_TYPE_ERROR        8
#define APX_DV_TYPE_ERROR             9
#define APX_UNSUPPORTED_ERROR         10
#define APX_NOT_IMPLEMENTED_ERROR     11
#define APX_NOT_FOUND_ERROR           12
#define APX_UNMATCHED_BRACE_ERROR     13
#define APX_UNMATCHED_BRACKET_ERROR   14
#define APX_UNMATCHED_STRING_ERROR    15
#define APX_INVALID_TYPE_REF_ERROR    16
#define APX_EXPECTED_BRACKET_ERROR    17
#define APX_INVALID_ATTRIBUTE_ERROR   18
#define APX_TOO_MANY_NODES_ERROR      19
#define APX_NODE_MISSING_ERROR        20
#define APX_NODE_ALREADY_EXISTS_ERROR 21
#define APX_MISSING_BUFFER_ERROR      22
#define APX_MISSING_FILE_ERROR        23
#define APX_NAME_MISSING_ERROR        24
#define APX_NAME_TOO_LONG_ERROR       25
#define APX_THREAD_CREATE_ERROR       26
#define APX_THREAD_JOIN_ERROR         27
#define APX_THREAD_JOIN_TIMEOUT_ERROR 28
#define APX_FILE_TOO_LARGE_ERROR      29
#define APX_MSG_TOO_LARGE_ERROR       30
#define APX_CONNECTION_ERROR          31
#define APX_TRANSMIT_ERROR            32
#define APX_NULL_PTR_ERROR            33
#define APX_BUFFER_BOUNDARY_ERROR     34
#define APX_BUFFER_FULL_ERROR         35
#define APX_QUEUE_FULL_ERROR          36
#define APX_DATA_NOT_PROCESSED_ERROR  37
#define APX_PACK_ERROR                38
#define APX_UNPACK_ERROR              39
#define APX_READ_ERROR                40
#define APX_INVALID_MSG_ERROR         41
#define APX_UNEXPECTED_DATA_ERROR     42
#define APX_INVALID_PROGRAM_ERROR     43
#define APX_INVALID_STATE_ERROR       44
#define APX_INVALID_INSTRUCTION_ERROR 45
#define APX_FILE_NOT_FOUND_ERROR      46
#define APX_FILE_ALREADY_EXISTS_ERROR 47
#define APX_MISSING_KEY_ERROR           48
#define APX_INVALID_OPEN_HANDLER_ERROR  49
#define APX_INVALID_WRITE_HANDLER_ERROR 50
#define APX_INVALID_WRITE_ERROR         51
#define APX_INVALID_FILE_ERROR          52
#define APX_INIT_VALUE_ERROR            53
#define APX_INVALID_ADDRESS_ERROR       54
#define APX_FILE_NOT_OPEN_ERROR         55
#define APX_BUSY_ERROR                  56
#define APX_DATA_NOT_COMPLETE_ERROR     57
#define APX_NOT_CONNECTED_ERROR         58
#define APX_INVALID_NAME_ERROR          59
#define APX_INVALID_PORT_HANDLE_ERROR   60

#define RMF_APX_NO_ERROR                    500
#define RMF_APX_INVALID_ARGUMENT_ERROR      (RMF_APX_NO_ERROR+APX_INVALID_ARGUMENT_ERROR)
#define RMF_APX_MEM_ERROR                   (RMF_APX_NO_ERROR+APX_MEM_ERROR)
#define RMF_APX_PARSE_ERROR                 (RMF_APX_NO_ERROR+APX_PARSE_ERROR)
#define RMF_APX_DATA_SIGNATURE_ERROR        (RMF_APX_NO_ERROR+APX_DATA_SIGNATURE_ERROR)
#define RMF_APX_PORT_SIGNATURE_ERROR        (RMF_APX_NO_ERROR+APX_PORT_SIGNATURE_ERROR)
#define RMF_APX_VALUE_ERROR                 (RMF_APX_NO_ERROR+APX_VALUE_ERROR)
#define RMF_APX_LENGTH_ERROR                (RMF_APX_NO_ERROR+APX_LENGTH_ERROR)
#define RMF_APX_ELEMENT_TYPE_ERROR          (RMF_APX_NO_ERROR+APX_LENGTH_ERROR)
#define RMF_APX_DV_TYPE_ERROR               (RMF_APX_NO_ERROR+APX_DV_TYPE_ERROR)
#define RMF_APX_UNSUPPORTED_ERROR           (RMF_APX_NO_ERROR+APX_UNSUPPORTED_ERROR)
#define RMF_APX_NOT_IMPLEMENTED_ERROR       (RMF_APX_NO_ERROR+APX_NOT_IMPLEMENTED_ERROR)
#define RMF_APX_NOT_FOUND_ERROR             (RMF_APX_NO_ERROR+APX_NOT_FOUND_ERROR)
#define RMF_APX_UNMATCHED_BRACE_ERROR      512
#define RMF_APX_UNMATCHED_BRACKET_ERROR    513
#define RMF_APX_UNMATCHED_STRING_ERROR     514
#define RMF_APX_INVALID_TYPE_REF_ERROR     515
#define RMF_APX_EXPECTED_BRACKET_ERROR     516
#define RMF_APX_INVALID_ATTRIBUTE_ERROR    517
#define RMF_APX_TOO_MANY_NODES_ERROR       518
#define RMF_APX_NODE_MISSING_ERROR         519
#define RMF_APX_NODE_ALREADY_EXISTS_ERROR  520
#define RMF_APX_MISSING_BUFFER_ERROR       521
#define RMF_APX_MISSING_FILE_ERROR         522
#define RMF_APX_NAME_MISSING_ERROR          (RMF_APX_NO_ERROR+APX_NAME_MISSING_ERROR
#define RMF_APX_NAME_TOO_LONG_ERROR         (RMF_APX_NO_ERROR+APX_NAME_TOO_LONG_ERROR)
#define RMF_APX_THREAD_CREATE_ERROR         (RMF_APX_NO_ERROR+APX_THREAD_CREATE_ERROR)
#define RMF_APX_THREAD_JOIN_ERROR           (RMF_APX_NO_ERROR+APX_THREAD_JOIN_ERROR)
#define RMF_APX_FILE_TOO_LARGE_ERROR        (RMF_APX_NO_ERROR+APX_FILE_TOO_LARGE_ERROR)
#define RMF_APX_MSG_TOO_LARGE_ERROR         (RMF_APX_NO_ERROR+APX_MSG_TOO_LARGE_ERROR)
                                            //Connection error cannot be sent remotely
#define RMF_APX_TRANSMIT_ERROR              (RMF_APX_NO_ERROR+APX_TRANSMIT_ERROR)
#define RMF_APX_NULL_PTR_ERROR              (RMF_APX_NO_ERROR+APX_NULL_PTR_ERROR)
#define RMF_APX_BUFFER_BOUNDARY_ERROR       (RMF_APX_NO_ERROR+APX_BUFFER_BOUNDARY_ERROR)
#define RMF_APX_BUFFER_FULL_ERROR           (RMF_APX_NO_ERROR+APX_BUFFER_FULL_ERROR)
#define RMF_APX_QUEUE_FULL_ERROR            (RMF_APX_NO_ERROR+APX_QUEUE_FULL_ERROR)
#define RMF_APX_DATA_NOT_PROCESSED_ERROR    (RMF_APX_NO_ERROR+APX_DATA_NOT_PROCESSED_ERROR)
#define RMF_APX_PACK_ERROR                  (RMF_APX_NO_ERROR+APX_PACK_ERROR)
#define RMF_APX_UNPACK_ERROR                (RMF_APX_NO_ERROR+APX_UNPACK_ERROR)
#define RMF_APX_READ_ERROR                  (RMF_APX_NO_ERROR+APX_READ_ERROR)
#define RMF_APX_INVALID_MSG_ERROR           (RMF_APX_NO_ERROR+APX_INVALID_MSG_ERROR)
#define RMF_APX_UNEXPECTED_DATA_ERROR       (RMF_APX_NO_ERROR+APX_UNEXPECTED_DATA_ERROR)
#define RMF_APX_INVALID_PROGRAM_ERROR       (RMF_APX_NO_ERROR+APX_INVALID_PROGRAM_ERROR)
#define RMF_APX_INVALID_STATE_ERROR         (RMF_APX_NO_ERROR+APX_INVALID_STATE_ERROR)
#define RMF_APX_INVALID_INSTRUCTION_ERROR   (RMF_APX_NO_ERROR+APX_INVALID_INSTRUCTION_ERROR)
#define RMF_APX_FILE_ALREADY_EXISTS_ERROR   (RMF_APX_NO_ERROR+APX_FILE_ALREADY_EXISTS_ERROR)
#define RMF_APX_FILE_NOT_FOUND_ERROR        (RMF_APX_NO_ERROR+APX_FILE_NOT_FOUND_ERROR)
#define RMF_APX_INVALID_OPEN_HANDLER_ERROR  (RMF_APX_NO_ERROR+APX_INVALID_OPEN_HANDLER_ERROR)
#define RMF_APX_INVALID_WRITE_HANDLER_ERROR (RMF_APX_NO_ERROR+APX_INVALID_WRITE_HANDLER_ERROR)
#define RMF_APX_INVALID_WRITE_ERROR         (RMF_APX_NO_ERROR+RMF_APX_INVALID_WRITE_ERROR)
#define RMF_APX_INIT_VALUE_ERROR            (RMF_APX_NO_ERROR+APX_INIT_VALUE_ERROR)
#define RMF_APX_INVALID_ADDRESS_ERROR       (RMF_APX_NO_ERROR+APX_INVALID_ADDRESS_ERROR)
#define RMF_APX_FILE_NOT_OPEN_ERROR         (RMF_APX_NO_ERROR+APX_FILE_NOT_OPEN_ERROR)
#define RMF_APX_BUSY_ERROR                  (RMF_APX_NO_ERROR+APX_BUSY_ERROR)
#define RMF_APX_DATA_NOT_COMPLETE_ERROR     (RMF_APX_NO_ERROR+APX_DATA_NOT_COMPLETE_ERROR)
#define RMF_APX_INVALID_NAME_ERROR          (RMF_APX_NO_ERROR+APX_INVALID_NAME_ERROR)
#define RMF_APX_INVALID_PORT_HANDLE_ERROR   (RMF_APX_NO_ERROR+APX_INVALID_PORT_HANDLE_ERROR)



typedef int32_t apx_error_t;

//////////////////////////////////////////////////////////////////////////////
// GLOBAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////

#endif //APX_ERROR_H
