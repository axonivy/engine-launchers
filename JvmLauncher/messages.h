 // ***** Sample.mc *****
 // This is the header section.
 // The following are message definitions.
//
//  Values are 32 bit values layed out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +---+-+-+-----------------------+-------------------------------+
//  |Sev|C|R|     Facility          |               Code            |
//  +---+-+-+-----------------------+-------------------------------+
//
//  where
//
//      Sev - is the severity code
//
//          00 - Success
//          01 - Informational
//          10 - Warning
//          11 - Error
//
//      C - is the Customer code flag
//
//      R - is a reserved bit
//
//      Facility - is the facility code
//
//      Code - is the facility's status code
//
//
// Define the facility codes
//


//
// Define the severity codes
//


//
// MessageId: COULD_NOT_LOAD_JVM
//
// MessageText:
//
//  %1.
//
#define COULD_NOT_LOAD_JVM               ((DWORD)0x00000001L)

//
// MessageId: COULD_NOT_INIT_JVM
//
// MessageText:
//
//  %1.
//
#define COULD_NOT_INIT_JVM               ((DWORD)0x00000002L)

//
// MessageId: ALREADY_CREATED_JVM
//
// MessageText:
//
//  %1.
//
#define ALREADY_CREATED_JVM              ((DWORD)0x00000003L)

//
// MessageId: COULD_NOT_LOAD_JAVA_CLASS
//
// MessageText:
//
//  %1.
//
#define COULD_NOT_LOAD_JAVA_CLASS        ((DWORD)0x00000004L)

//
// MessageId: COULD_NOT_FIND_METHOD
//
// MessageText:
//
//  %1.
//
#define COULD_NOT_FIND_METHOD            ((DWORD)0x00000005L)

//
// MessageId: JVM_WAS_NOT_CREATED
//
// MessageText:
//
//  %1.
//
#define JVM_WAS_NOT_CREATED              ((DWORD)0x00000006L)

//
// MessageId: COULD_NOT_CONVERT_PARAMS
//
// MessageText:
//
//  %1.
//
#define COULD_NOT_CONVERT_PARAMS         ((DWORD)0x00000007L)

//
// MessageId: EXCEPTION_IN_MAIN_METHOD
//
// MessageText:
//
//  %1.
//
#define EXCEPTION_IN_MAIN_METHOD         ((DWORD)0x00000008L)

//
// MessageId: COULD_NOT_START_SERVICE_DISPATCHER
//
// MessageText:
//
//  %1.
//
#define COULD_NOT_START_SERVICE_DISPATCHER ((DWORD)0x00000009L)

//
// MessageId: COULD_NOT_OPEN_SERVICE_CTRL_MANAGER
//
// MessageText:
//
//  %1.
//
#define COULD_NOT_OPEN_SERVICE_CTRL_MANAGER ((DWORD)0x0000000AL)

//
// MessageId: COULD_NOT_CREATE_SERVICE
//
// MessageText:
//
//  %1.
//
#define COULD_NOT_CREATE_SERVICE         ((DWORD)0x0000000BL)

//
// MessageId: COULD_NOT_OPEN_SERVICE
//
// MessageText:
//
//  %1.
//
#define COULD_NOT_OPEN_SERVICE           ((DWORD)0x0000000CL)

//
// MessageId: COULD_NOT_DELETE_SERVICE
//
// MessageText:
//
//  %1.
//
#define COULD_NOT_DELETE_SERVICE         ((DWORD)0x0000000DL)

//
// MessageId: COULD_NOT_START_SERVICE
//
// MessageText:
//
//  %1.
//
#define COULD_NOT_START_SERVICE          ((DWORD)0x0000000EL)

//
// MessageId: COULD_NOT_STOP_SERVICE
//
// MessageText:
//
//  %1.
//
#define COULD_NOT_STOP_SERVICE           ((DWORD)0x0000000FL)

//
// MessageId: COULD_NOT_QUERY_SERVICE_STATUS
//
// MessageText:
//
//  %1.
//
#define COULD_NOT_QUERY_SERVICE_STATUS   ((DWORD)0x00000010L)

//
// MessageId: ERROR_SERVICE_HAS_NOT_STARTED
//
// MessageText:
//
//  %1.
//
#define ERROR_SERVICE_HAS_NOT_STARTED    ((DWORD)0x00000011L)

//
// MessageId: ERROR_SERVICE_HAS_NOT_STOPPED
//
// MessageText:
//
//  %1.
//
#define ERROR_SERVICE_HAS_NOT_STOPPED    ((DWORD)0x00000012L)

//
// MessageId: COULD_NOT_CREATE_JAVA_SERVICE_OBJECT
//
// MessageText:
//
//  %1.
//
#define COULD_NOT_CREATE_JAVA_SERVICE_OBJECT ((DWORD)0x00000013L)

//
// MessageId: EXCEPTION_IN_JAVA_MAIN_SERVICE_METHOD
//
// MessageText:
//
//  %1.
//
#define EXCEPTION_IN_JAVA_MAIN_SERVICE_METHOD ((DWORD)0x00000014L)

//
// MessageId: COULD_NOT_CREATE_EVENT_FOR_JAVA_THREAD
//
// MessageText:
//
//  %1.
//
#define COULD_NOT_CREATE_EVENT_FOR_JAVA_THREAD ((DWORD)0x00000015L)

//
// MessageId: COULD_NOT_CREATE_JAVA_SERVICE_STARTUP_THREAD
//
// MessageText:
//
//  %1.
//
#define COULD_NOT_CREATE_JAVA_SERVICE_STARTUP_THREAD ((DWORD)0x00000016L)

//
// MessageId: TIMEOUT_JAVA_SERVICE_STARTUP_THREAD
//
// MessageText:
//
//  %1.
//
#define TIMEOUT_JAVA_SERVICE_STARTUP_THREAD ((DWORD)0x00000017L)

//
// MessageId: WRONG_LAUNCH_CONFIGURATION_FILE_FORMAT
//
// MessageText:
//
//  %1.
//
#define WRONG_LAUNCH_CONFIGURATION_FILE_FORMAT ((DWORD)0x00000018L)

//
// MessageId: COULD_NOT_DESTROY_JVM
//
// MessageText:
//
//  %1.
//
#define COULD_NOT_DESTROY_JVM            ((DWORD)0x00000019L)

//
// MessageId: COULD_NOT_DETACH_THREAD_FROM_JVM
//
// MessageText:
//
//  %1.
//
#define COULD_NOT_DETACH_THREAD_FROM_JVM ((DWORD)0x0000001AL)

//
// MessageId: COULD_NOT_ATTACH_THREAD_TO_JVM
//
// MessageText:
//
//  %1.
//
#define COULD_NOT_ATTACH_THREAD_TO_JVM   ((DWORD)0x0000001BL)

//
// MessageId: COULD_NOT_EVALUATE_APPLICATION_DIRECTORY
//
// MessageText:
//
//  %1.
//
#define COULD_NOT_EVALUATE_APPLICATION_DIRECTORY ((DWORD)0x0000001CL)

//
// MessageId: COULD_NOT_REGISTER_SERVICE_CONTROL_HANDLER
//
// MessageText:
//
//  %1.
//
#define COULD_NOT_REGISTER_SERVICE_CONTROL_HANDLER ((DWORD)0x0000001DL)

//
// MessageId: COULD_NOT_SET_SERVICE_STATUS
//
// MessageText:
//
//  %1.
//
#define COULD_NOT_SET_SERVICE_STATUS     ((DWORD)0x0000001EL)

//
// MessageId: COULD_NOT_SET_CONSOLE_CTRL_HANDLER
//
// MessageText:
//
//  %1.
//
#define COULD_NOT_SET_CONSOLE_CTRL_HANDLER ((DWORD)0x0000001FL)

//
// MessageId: EXCEPTION_IN_JAVA_SERVICE_CONTROL_METHOD
//
// MessageText:
//
//  %1.
//
#define EXCEPTION_IN_JAVA_SERVICE_CONTROL_METHOD ((DWORD)0x00000020L)

//
// MessageId: COULD_NOT_ENUMERATE_WINDOWS
//
// MessageText:
//
//  %1.
//
#define COULD_NOT_ENUMERATE_WINDOWS      ((DWORD)0x00000021L)

//
// MessageId: COULD_NOT_MAP_VIEW_OF_VIEW
//
// MessageText:
//
//  %1.
//
#define COULD_NOT_MAP_VIEW_OF_VIEW       ((DWORD)0x00000022L)

//
// MessageId: COULD_NOT_CREATE_FILE_MAPPING
//
// MessageText:
//
//  %1.
//
#define COULD_NOT_CREATE_FILE_MAPPING    ((DWORD)0x00000023L)

