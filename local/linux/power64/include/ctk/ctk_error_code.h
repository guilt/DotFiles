/** @file ctk_error_code.h
   @brief CTK error code.
    This file is automatically generated. */
#define CTK_SUCCESS	0	/**< Success (no error) */
#define CTK_ERROR_GENERIC	1000	/**< Generic error */
#define CTK_ERROR_UNKNOWN	1001	/**< Unknown error */
#define CTK_ERROR_FILE_NOT_FOUND	1002	/**< File not found */
#define CTK_ERROR_THREAD_NOT_FOUND	1003	/**< No such thread */
#define CTK_ERROR_NOT_FOUND	1004	/**< Object not found */
#define CTK_ERROR_BAD_ID	1005	/**< Invalid ID */
#define CTK_ERROR_BAD_FILE	1006	/**< Invalid file */
#define CTK_ERROR_BAD_FORMAT	1007	/**< Bad format */
#define CTK_ERROR_BAD_ADDRESS	1008	/**< Bad address */
#define CTK_ERROR_BAD_OBJECT	1009	/**< Invalid object */
#define CTK_ERROR_BAD_VALUE	1010	/**< Invalid value */
#define CTK_ERROR_NULL_OBJECT	1011	/**< Got a NULL object */
#define CTK_ERROR_BAD_ALIGNMENT	1012	/**< Bad alignment */
#define CTK_ERROR_BAD_ELF	1013	/**< Bad Elf image */
#define CTK_ERROR_BAD_ELF_HEADER	1014	/**< Invalid Elf header */
#define CTK_ERROR_BAD_ELF_MACHTYPE	1015	/**< Invalid Elf machine type */
#define CTK_ERROR_BAD_ELF_TYPE	1016	/**< Invalid Elf object type */
#define CTK_ERROR_NO_ELF_SEGMENT	1017	/**< No Elf segment to load */
#define CTK_ERROR_NOT_EXECUTABLE	1018	/**< Not an executable file */
#define CTK_ERROR_NOT_REGULAR_FILE	1019	/**< Not a regular file */
#define CTK_ERROR_FILE_TOO_LARGE	1020	/**< File is too large */
#define CTK_ERROR_NO_MEMORY	1021	/**< No enough memory */
#define CTK_ERROR_NO_RESOURCE	1022	/**< Not enough resource */
#define CTK_ERROR_BAD_FLAG	1023	/**< Invalid flag */
#define CTK_ERROR_BAD_ARGUMENT	1024	/**< Invalid argument */
#define CTK_ERROR_BAD_REQUEST	1025	/**< Invalid request */
#define CTK_ERROR_OUT_OF_RANGE	1026	/**< Out of range */
#define CTK_ERROR_NO_PERMISSION	1027	/**< Permission denied */
#define CTK_ERROR_BUSY	1028	/**< Resource busy */
#define CTK_ERROR_NOT_INITIALIZED	1029	/**< Not initialized */
#define CTK_ERROR_NOT_SUPPORTED	1030	/**< Not supported */
#define CTK_ERROR_INTERNAL	1031	/**< Internal error */
#define CTK_ERROR_RETRY	1032	/**< Internal error (please retry) */
#define CTK_ERROR_NO_DATA	1033	/**< No data is available (on non-blocking call) */
#define CTK_ERROR_TOO_BIG	1034	/**< The parameter is too big */
#define CTK_ERROR_TOO_SMALL	1035	/**< The parameter is too small */
#define CTK_ERROR_TOO_MANY	1036	/**< Too many objects or items */
#define CTK_ERROR_SPE_NOT_STARTED	1037	/**< The SPE is not started */
#define CTK_ERROR_SPE_NOT_RUNNING	1038	/**< The SPE is not running */
#define CTK_ERROR_SPE_ALREADY_RUNNING	1039	/**< The SPE is already running */
#define CTK_ERROR_BAD_SPE_STATUS	1040	/**< Bad SPE status */
#define CTK_ERROR_ITEM_EXISTS	1041	/**< The item already exists */
#define CTK_ERROR_INVALID_ENTRY_ADDR	1042	/**< The entry address is invalid */
#define CTK_ERROR_BAD_TASK_STATUS	1043	/**< Bad task status */
#define CTK_ERROR_TASK_ALREADY_QUEUED	1044	/**< The task is already queued or running */
#define CTK_ERROR_NO_TASK	1045	/**< No such tasks. */
#define CTK_ERROR_TASK_DETACHED	1046	/**< The task is detached. */
#define CTK_ERROR_BAD_RUNTIME_TYPE	1047	/**< Bad runtime type. */
#define CTK_ERROR_MAX	1047	/**< The largest error code */
extern const char *ctk_errlist[];	/**< CTK error msgs */
extern int ctk_nerr;	/**< Number of CTK error code */