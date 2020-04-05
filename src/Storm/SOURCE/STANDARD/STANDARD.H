/****************************************************************************
*
*  GLOBAL VARIABLES
*
***/

extern HINSTANCE global_instance;

/****************************************************************************
*
*  EXPORTED STRUCTURES
*
***/

extern DWORD     ipx_id;
extern LPCSTR    ipx_desc;
extern LPCSTR    ipx_req;
extern SNETCAPS  ipx_caps;
extern SNETSPI   ipx_spi;

extern DWORD     modem_id;
extern LPCSTR    modem_desc;
extern LPCSTR    modem_req;
extern SNETCAPS  modem_caps;
extern SNETSPI   modem_spi;

extern DWORD     serial_id;
extern LPCSTR    serial_desc;
extern LPCSTR    serial_req;
extern SNETCAPS  serial_caps;
extern SNETSPI   serial_spi;

extern DWORD     null_id;
extern LPCSTR    null_desc;
extern LPCSTR    null_req;
extern SNETCAPS  null_caps;
extern SNETSPI   null_spi;

/****************************************************************************
*
*  COMMON PERFORMANCE MONITORING FUNCTIONS
*
***/

#define  PERF_PKTSENT      0
#define  PERF_PKTRECV      1
#define  PERF_BYTESSENT    2
#define  PERF_BYTESRECV    3
#define  PERFNUM           4

void PerfAdd (DWORD id, DWORD value);
BOOL PerfGetPerformanceData (DWORD          counterid,
                             DWORD         *countervalue,
                             LARGE_INTEGER *measurementtime,
                             LARGE_INTEGER *measurementfreq);
void PerfIncrement (DWORD id);
void PerfReset ();
