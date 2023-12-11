//=============================================================================
// This confidential and proprietary software may be used only as authorized by
// a licensing agreement from CHEMTRONICS Limited.
//
// COPYRIGHT (c) CHEMTRONICS. ALL RIGHTS RESERVED.
//
// The entire notice above must be reproduced on all authorized copies and
// copies may only be made to the extent permitted by a licensing agreement
// from CHEMTRONICS Limited.
//
// Module: sampel_external.c
// Description: 5G NR V2X OBU GW Sample Code for Test
// Update History
// [16/05/2023 jongsik.kim] create
//=============================================================================
/* include */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <pthread.h>

/* include for v2x */
#include "v2x_defs.h"
#include "v2x_ext_type.h"
#include "db_v2x.h"

/////////////////////////////////////////////////////////////////////////////////////////
/* MACRO - for modify */

#define SAMPLE_V2X_API_VER 0x0001
#define SAMPLE_V2X_IP_ADDR "192.168.1.11"
#define SAMPLE_V2X_PORT_ADDR 47347

#define SAMPLE_V2X_MSG_LEN 100

/////////////////////////////////////////////////////////////////////////////////////////
/* MACRO - for sample */

/* Usage */
#define SAMPLE_USAGE_STR "Usage: %s [OPTIONS]\n"                                                                  \
						 "  -o payload_type  : Raw, EncodedbyJ2735, eITSK00130, eKETI, eETRI (default : Raw)\n"   \
						 "  -p psid          : <decimal number> (default : 32)\n"                                 \
						 "  -y comm_type     : DSRC, LTEV2X, 5GNRV2X (default : 5GNRV2X)\n"                       \
						 "  -t tx_port       : tx port dbm (default : 20)\n"                                      \
						 "  -s signer_id     : UNSECURED, CERTIFICATE, DIGEST, ALTERNATE (default : UNSECURED)\n" \
						 "  -r priority      : 0~7 (default : 0)\n"                                               \
						 "  -c tx_count      : total tx count (default : 100)\n"                                  \
						 "  -d tx_delay      : msec delay (default : 100)\n"                                      \
						 "  -m total_time    : total exec second (default : 10)\n"                                \
						 "  -h help\n"

/////////////////////////////////////////////////////////////////////////////////////////
/* Global Variable Value */
V2xAction_t e_action_g = eV2xAction_ADD;
V2xPayloadType_t e_payload_type_g = eRaw;
V2xPsid_t psid_g = 5271;
V2XCommType_t e_comm_type_g = eV2XCommType_5GNRV2X;
V2xPowerDbm_t tx_power_g = 20;
V2xSignerId_t e_signer_id_g = eV2xSignerId_UNSECURED;
V2xMsgPriority_t e_priority_g = eV2xPriority_CV2X_PPPP_0;
uint32_t tx_cnt_g = 100;
uint32_t tx_delay_g = 100;
V2xFrequency_t freq_g = 5900;
V2xDataRate_t e_data_rate_g = eV2xDataRate_6MBPS;
V2xTimeSlot_t e_time_slot_g = eV2xTimeSlot_Continuous;
uint8_t peer_mac_addr_g[MAC_EUI48_LEN] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
uint32_t transmitter_profile_id_g = 100;
uint32_t peer_l2id_g = 0;

uint32_t delay_time_sec_g = 10;

int sock_g = -1;

/////////////////////////////////////////////////////////////////////////////////////////
/* Functions */

void print_usage(char *program_name_str)
{
	printf(SAMPLE_USAGE_STR, program_name_str);
}

/* function : Parsing Options */
int parsing_option(int argc, char *argv[])
{
	int res = -1; // failure

	char payload_type_str[256] = {0};
	char comm_type_str[256] = {0};
	char signer_id_str[256] = {0};

	uint8_t payload_type_flg = 0;
	uint8_t comm_type_flg = 0;
	uint8_t signer_id_flg = 0;

	int option;
	while ((option = getopt(argc, argv, "i:o:p:y:t:s:r:c:d:m:h")) != -1)
	{
		switch (option)
		{
		case 'o':
			payload_type_flg = 1;
			strcpy(payload_type_str, optarg);
			printf("Payload type: %s\n", optarg);
			break;
		case 'p':
			psid_g = (V2xPsid_t)strtoul(optarg, NULL, 0);
			printf("PSID: %d\n", psid_g);
			break;
		case 'y':
			comm_type_flg = 1;
			strcpy(comm_type_str, optarg);
			printf("Communication type: %s\n", optarg);
			break;
		case 't':
			tx_power_g = (V2xPowerDbm_t)strtoul(optarg, NULL, 0);
			printf("TX Power: %d\n", tx_power_g);
			break;
		case 's':
			signer_id_flg = 1;
			strcpy(signer_id_str, optarg);
			printf("Signer ID: %s\n", optarg);
			break;
		case 'r':
			e_priority_g = (V2xMsgPriority_t)strtoul(optarg, NULL, 0);
			printf("Msg Priority: %d\n", e_priority_g);
			break;
		case 'c':
			tx_cnt_g = (uint32_t)strtoul(optarg, NULL, 0);
			printf("TX count: %u\n", tx_cnt_g);
			break;
		case 'd':
			tx_delay_g = (uint32_t)strtoul(optarg, NULL, 0);
			printf("TX delay: %u\n", tx_delay_g);
			break;
		case 'm':
			delay_time_sec_g = (uint32_t)strtoul(optarg, NULL, 0);
			printf("Delay time(sec): %u\n", delay_time_sec_g);
			break;
		case 'h':
			print_usage(argv[0]);
			exit(EXIT_SUCCESS);
		default:
			print_usage(argv[0]);
			exit(EXIT_FAILURE);
		}
	}

	if (payload_type_flg)
	{
		if (strcmp(payload_type_str, "Raw") == 0)
		{
			e_payload_type_g = eRaw;
		}
		else if (strcmp(payload_type_str, "EncodedbyJ2735") == 0)
		{
			e_payload_type_g = eEncodedbyJ2735;
		}
		else if (strcmp(payload_type_str, "ITSK00130") == 0)
		{
			e_payload_type_g = eITSK00130;
		}
		else if (strcmp(payload_type_str, "KETI") == 0)
		{
			e_payload_type_g = eKETI;
		}
		else if (strcmp(payload_type_str, "ETRI") == 0)
		{
			e_payload_type_g = eETRI;
		}
		else
		{
			fprintf(stderr, "payload type default set : %d\n", e_payload_type_g);
		}
	}

	if (comm_type_flg)
	{
		if (strcmp(comm_type_str, "DSRC") == 0)
		{
			e_comm_type_g = eV2XCommType_DSRC;
		}
		else if (strcmp(comm_type_str, "LTEV2X") == 0)
		{
			e_comm_type_g = eV2XCommType_LTEV2X;
		}
		else if (strcmp(comm_type_str, "5GNRV2X") == 0)
		{
			e_comm_type_g = eV2XCommType_5GNRV2X;
		}
		else
		{
			fprintf(stderr, "Communication type default set : %d\n", e_comm_type_g);
		}
	}

	if (signer_id_flg)
	{
		if (strcmp(signer_id_str, "UNSECURED") == 0)
		{
			e_signer_id_g = eV2xSignerId_UNSECURED;
		}
		else if (strcmp(signer_id_str, "CERTIFICATE") == 0)
		{
			e_signer_id_g = eV2xSignerId_CERTIFICATE;
		}
		else if (strcmp(signer_id_str, "DIGEST") == 0)
		{
			e_signer_id_g = eV2xSignerId_DIGEST;
		}
		else if (strcmp(signer_id_str, "ALTERNATE") == 0)
		{
			e_signer_id_g = eV2xSignerId_ALTERNATE;
		}
		else
		{
			fprintf(stderr, "Communication type default set : %d\n", e_signer_id_g);
		}
	}

	res = 0; // success
	return res;
}

int connect_v2x_socket(void)
{
	int res = -1; // failure

	// Create the socket
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0)
	{
		perror("socket() failed");
		res = sock;
		return res;
	}

	// Connect to the server
	struct sockaddr_in server_addr =
		{
			.sin_family = AF_INET,
			.sin_addr.s_addr = inet_addr(SAMPLE_V2X_IP_ADDR),
			.sin_port = htons(SAMPLE_V2X_PORT_ADDR)};
	if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
	{
		perror("connect() failed");
		return res;
	}
#if 1
	// Change to NON-BLOCK socket
	int flags = fcntl(sock, F_GETFL, 0);
	if (flags == -1)
	{
		perror("fcntl F_GETFL failed");
		return res;
	}

	flags |= O_NONBLOCK;
	if (fcntl(sock, F_SETFL, flags) == -1)
	{
		perror("fcntl F_SETFL failed");
		return res;
	}
#endif
	printf("connect socket success\n");

	sock_g = sock;

	res = 0;
	return res;
}

void close_v2x_socket(void)
{
	// Close the socket
	if (sock_g >= 0)
	{
		close(sock_g);
	}
}

/* function : WSR Send/Response processing */
int v2x_wsr_cmd_process(void)
{
	int res = -1; // failure

	// Prepare the Ext_WSReq_t structure
	Ext_WSReq_t ws_req;
	memset(&ws_req, 0, sizeof(ws_req));
	ws_req.magic_num = htons(MAGIC_WSREQ);
	ws_req.ver = htons(SAMPLE_V2X_API_VER);
	ws_req.e_action = eV2xAction_ADD;
	ws_req.e_payload_type = e_payload_type_g;
	ws_req.psid = htonl(psid_g);

	printf("\nWSM Service REQ>>\n"
		   "  magic_num        : 0x%04X\n"
		   "  ver              : 0x%04X\n"
		   "  e_action         : %d\n"
		   "  e_payload_type   : %d\n"
		   "  psid             : %u\n",
		   ntohs(ws_req.magic_num),
		   ntohs(ws_req.ver),
		   ws_req.e_action,
		   ws_req.e_payload_type,
		   ntohl(ws_req.psid));

	// Send the request
	ssize_t n = send(sock_g, &ws_req, sizeof(ws_req), 0);
	if (n < 0)
	{
		perror("send() failed");
		return res;
	}
	else if (n != sizeof(ws_req))
	{
		fprintf(stderr, "send() sent a different number of bytes than expected\n");
		return res;
	}

	// Wait for the response
	Ext_WSResp_t ws_resp;
	memset(&ws_resp, 0, sizeof(ws_resp));
	n = -1;
	while (n <= 0)
	{
		n = recv(sock_g, &ws_resp, sizeof(ws_resp), 0);
		if (n < 0)
		{
			if (errno != EAGAIN && errno != EWOULDBLOCK)
			{
				perror("recv() failed");
				break;
			}
		}
		else if (n == 0)
		{
			fprintf(stderr, "recv() connection closed by peer\n");
		}
		else if (n != sizeof(ws_resp))
		{
			fprintf(stderr, "recv() received a different number of bytes than expected\n");
		}

		usleep(1000);
	}

	// Print the response
	printf("\nWSM Service RESP>>\n"
		   "  magic_num      : 0x%04X\n"
		   "  ver            : 0x%04X\n"
		   "  e_action       : %d\n"
		   "  is_confirmed   : %d\n"
		   "  psid           : %u\n",
		   ntohs(ws_resp.magic_num),
		   ntohs(ws_resp.ver),
		   ws_resp.e_action,
		   ws_resp.is_confirmed,
		   ntohl(ws_resp.psid));

	res = 0;
	return res;
}

/* function : V2X TX processing */
void *v2x_tx_cmd_process(void *arg)
{
	(void)arg;

	// Prepare the Ext_WSReq_t structure
	int db_v2x_tmp_size = sizeof(DB_V2X_T) + SAMPLE_V2X_MSG_LEN;
	int v2x_tx_pdu_size = sizeof(Ext_V2X_TxPDU_t) + db_v2x_tmp_size;

	Ext_V2X_TxPDU_t *v2x_tx_pdu_p = NULL;
	DB_V2X_T *db_v2x_tmp_p = NULL;

	v2x_tx_pdu_p = malloc(v2x_tx_pdu_size);
	memset(v2x_tx_pdu_p, 0, sizeof(Ext_V2X_TxPDU_t));

	v2x_tx_pdu_p->ver = htons(SAMPLE_V2X_API_VER);
	v2x_tx_pdu_p->e_payload_type = e_payload_type_g;
	v2x_tx_pdu_p->psid = htonl(psid_g);
	v2x_tx_pdu_p->tx_power = tx_power_g;
	v2x_tx_pdu_p->e_signer_id = e_signer_id_g;
	v2x_tx_pdu_p->e_priority = e_priority_g;

	if (e_comm_type_g == eV2XCommType_LTEV2X || e_comm_type_g == eV2XCommType_5GNRV2X)
	{
		v2x_tx_pdu_p->magic_num = htons(MAGIC_CV2X_TX_PDU);
		v2x_tx_pdu_p->u.config_cv2x.transmitter_profile_id = htonl(transmitter_profile_id_g);
		v2x_tx_pdu_p->u.config_cv2x.peer_l2id = htonl(peer_l2id_g);
	}
	else if (e_comm_type_g == eV2XCommType_DSRC)
	{
		v2x_tx_pdu_p->magic_num = htons(MAGIC_DSRC_TX_PDU);
		v2x_tx_pdu_p->u.config_wave.freq = htons(freq_g);
		v2x_tx_pdu_p->u.config_wave.e_data_rate = htons(e_data_rate_g);
		v2x_tx_pdu_p->u.config_wave.e_time_slot = e_time_slot_g;
		memcpy(v2x_tx_pdu_p->u.config_wave.peer_mac_addr, peer_mac_addr_g, MAC_EUI48_LEN);
	}

	// Payload = KETI Format
	v2x_tx_pdu_p->v2x_msg.length = htons(db_v2x_tmp_size);

	db_v2x_tmp_p = malloc(db_v2x_tmp_size);
	memset(db_v2x_tmp_p, 0, db_v2x_tmp_size);

	db_v2x_tmp_p->eDeviceType = DB_V2X_DEVICE_TYPE_OBU;
	db_v2x_tmp_p->eTeleCommType = DB_V2X_TELECOMM_TYPE_5G_PC5;
	db_v2x_tmp_p->unDeviceId = 0;
	db_v2x_tmp_p->ulTimeStamp = 0ULL;
	db_v2x_tmp_p->eServiceId = DB_V2X_SERVICE_ID_PLATOONING;
	db_v2x_tmp_p->eActionType = DB_V2X_ACTION_TYPE_REQUEST;
	db_v2x_tmp_p->eRegionId = DB_V2X_REGION_ID_SEOUL;
	db_v2x_tmp_p->ePayloadType = DB_V2X_PAYLOAD_TYPE_SAE_J2735_BSM;
	db_v2x_tmp_p->eCommId = DB_V2X_COMM_ID_V2V;
	db_v2x_tmp_p->usDbVer = 0;
	db_v2x_tmp_p->usHwVer = 0;
	db_v2x_tmp_p->usSwVer = 0;
	db_v2x_tmp_p->ulPayloadLength = SAMPLE_V2X_MSG_LEN;
	db_v2x_tmp_p->ulPayloadCrc32 = 0;

	memcpy(v2x_tx_pdu_p->v2x_msg.data, db_v2x_tmp_p, db_v2x_tmp_size);

	printf("\nV2X TX PDU>>\n"
		   "  magic_num        : 0x%04X\n"
		   "  ver              : 0x%04X\n"
		   "  e_payload_type   : %d\n"
		   "  psid             : %u\n"
		   "  tx_power         : %d\n"
		   "  e_signer_id      : %d\n"
		   "  e_priority       : %d\n",
		   ntohs(v2x_tx_pdu_p->magic_num),
		   ntohs(v2x_tx_pdu_p->ver),
		   v2x_tx_pdu_p->e_payload_type,
		   ntohl(v2x_tx_pdu_p->psid),
		   v2x_tx_pdu_p->tx_power,
		   v2x_tx_pdu_p->e_signer_id,
		   v2x_tx_pdu_p->e_priority);

	if (e_comm_type_g == eV2XCommType_LTEV2X || e_comm_type_g == eV2XCommType_5GNRV2X)
	{
		printf("  u.config_cv2x.transmitter_profile_id : %u\n"
			   "  u.config_cv2x.peer_l2id              : %u\n",
			   ntohl(v2x_tx_pdu_p->u.config_cv2x.transmitter_profile_id),
			   ntohl(v2x_tx_pdu_p->u.config_cv2x.peer_l2id));
	}
	else if (e_comm_type_g == eV2XCommType_DSRC)
	{
		printf("  u.config_wave.freq                  : %d\n"
			   "  u.config_wave.e_data_rate           : %d\n"
			   "  u.config_wave.e_time_slot           : %d\n"
			   "  u.config_wave.peer_mac_addr         : %s\n",
			   ntohs(v2x_tx_pdu_p->u.config_wave.freq),
			   ntohs(v2x_tx_pdu_p->u.config_wave.e_data_rate),
			   v2x_tx_pdu_p->u.config_wave.e_time_slot,
			   v2x_tx_pdu_p->u.config_wave.peer_mac_addr);
	}

	uint32_t i;
	ssize_t n;
	for (i = 0; i < tx_cnt_g; i++)
	{
		// Send the request
		n = send(sock_g, v2x_tx_pdu_p, v2x_tx_pdu_size, 0);
		if (n < 0)
		{
			perror("send() failed");
			break;
		}
		else if (n != v2x_tx_pdu_size)
		{
			fprintf(stderr, "send() sent a different number of bytes than expected\n");
			break;
		}
		else
		{
			printf(" tx send success(%ld bytes) : [%u/%u]\n", n, i + 1, tx_cnt_g);
		}

		usleep((1000 * tx_delay_g));
	}

	free(v2x_tx_pdu_p);
	free(db_v2x_tmp_p);

	return NULL;
}

/* function : V2X RX processing */
void *v2x_rx_cmd_process(void *arg)
{
	(void)arg;

	uint8_t buf[4096] = {0};
	int n = -1;
	time_t start_time = time(NULL);

	while (1)
	{
		time_t current_time = time(NULL);
		if (current_time - start_time >= delay_time_sec_g)
		{
			break;
		}

		n = recv(sock_g, buf, sizeof(buf), 0);
		if (n < 0)
		{
			if (errno != EAGAIN && errno != EWOULDBLOCK)
			{
				perror("recv() failed");
				break;
			}
			else
			{
				usleep(10000);
				continue;
			}
		}
		else if (n == 0)
		{
			fprintf(stderr, "recv() connection closed by peer\n");
			break;
		}
		else
		{
			// fprintf(stderr, "recv() success : len[%u]\n",n);
			printf("recv() success : len[%u]\n", n);
		}
	}

	return NULL;
}

/* function : Process Commands */
int process_commands(void)
{
	int res = -1; // failure
	pthread_t tx_thread;
	pthread_t rx_thread;
	void *tx_thread_ret;
	void *rx_thread_ret;

	v2x_wsr_cmd_process();

	pthread_create(&tx_thread, NULL, v2x_tx_cmd_process, NULL);
	pthread_create(&rx_thread, NULL, v2x_rx_cmd_process, NULL);

	pthread_join(tx_thread, &tx_thread_ret);
	pthread_join(rx_thread, &rx_thread_ret);

	return res;
}

/* function : Main(Entry point of this program) */
int main(int argc, char *argv[])
{
	int res;

	do
	{
		// Connect V2X Socket
		if ((res = connect_v2x_socket()) < 0)
		{
			break;
		}

		// Parsing Options
		if ((res = parsing_option(argc, argv)) < 0)
		{
			break;
		}

		// Process Commands
		if ((res = process_commands()) < 0)
		{
			break;
		}
	} while (0);

	// Close V2X Socket
	close_v2x_socket();

	return res;
}

/////////////////////////////////////////////////////////////////////////////////////////
