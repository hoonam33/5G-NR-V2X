// ~/NmpntLibrary/NmpntClient/NmpntExtern.h
#pragma once
#ifdef _WIN32
	#define LIB_INTERFACE		EXTERN_C __declspec(dllexport)
#else
	#define LIB_INTERFACE		extern "C"
#endif

// Callback Functions
typedef void(*TableCB)(char *mount);						// Source Table
typedef void(*NtripCB)(void *owner, char *data, int size);	// Receive Data
typedef void(*StateCB)(void *owner, int state); 			// Status Event

typedef struct {
	unsigned int				major : 16;	// 2 byte
	unsigned int				minor : 8;	// 1 byte
	unsigned int				patch : 8;	// 1 byte
} LibVersion; // 4 bytes
	
#define NTRIP_DATASIZE			2048

// Client Status Code
#define NTRIP_ONAIR				 1	// 서비스 정상
#define NTRIP_OFFAIR			 0	// 서비스 정지
#define NTRIP_SOCKET_ERROR		-1	// 소켓 오류
#define NTRIP_CONNECT_ERROR		-2	// 연결 오류
#define NTRIP_TIMEOUT_ERROR		-3	// 수신 불량
#define NTRIP_CONNECT_CLOSE		-4	// 연결 끊김
#define NTRIP_MOUNT_ERROR		-5	// 마운트명 오류
#define NTRIP_USER_ERROR		-6	// 사용자정보 오류
#define NTRIP_VERSION_ERROR		-7	// 버전 오류
#define NTRIP_CASTER_ERROR		-8	// 캐스터 오류

// Client Use Purpose
#define NTRIP_PURPOSE_ETC		0	// 기타
#define NTRIP_PURPOSE_MES		1	// 측량
#define NTRIP_PURPOSE_GIS		2	// FGIS/GIS
#define NTRIP_PURPOSE_RND		3	// 연구개발
#define NTRIP_PURPOSE_ENV		4	// 환경조사
#define NTRIP_PURPOSE_CON		5	// 건설
#define NTRIP_PURPOSE_TRF		6	// 교통
#define NTRIP_PURPOSE_ANF		7	// 농/수산업
#define NTRIP_PURPOSE_COM		8	// 통신
#define NTRIP_PURPOSE_EXP		9	// 원격탐사
#define NTRIP_PURPOSE_TMP		10	// 기상
#define NTRIP_PURPOSE_EDU		11	// 교육
#define NTRIP_PURPOSE_AIR		12	// 항공

class NmpntClient;
LIB_INTERFACE LibVersion NmpntVersion();
LIB_INTERFACE int NmpntSourceTable(TableCB table);
LIB_INTERFACE NmpntClient *NmpntConstruct();
LIB_INTERFACE void NmpntDestruct(NmpntClient *nmpnt);
LIB_INTERFACE void NmpntStart(NmpntClient *nmpnt, void *owner, char *mount, int purpose, NtripCB ntrip, StateCB state);
LIB_INTERFACE void NmpntStop(NmpntClient *nmpnt);
LIB_INTERFACE int NmpntStatus(NmpntClient *nmpnt);

