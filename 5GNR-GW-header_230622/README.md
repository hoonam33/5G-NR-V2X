#5G NR V2X GW Sample Code
 5G NR V2X 과제를 위해 제공 되는 5G NR V2X OBU GW Sample Code
 자세한 Data Format 및 처리 절차는 Chem_5GNRV2X_OBU_GW_IF 문사를 참조하세요.

## Reference Standard
SAE J2945/1_201603/202004 On-Board System Requirements for V2V Safety Communications
SAE J2735_201603 Dedicated Short Range Communications (DSRC) Message Set Dictionary 
ITSK-00100-3:2021v2  C-ITS 규격 - 제3부:C2X 정보연계 C-ITS Specification Part3.C2X Information Linking
IEEE 1609.3 Networking services

## Files
### inc Directory
#### v2x_defs.h
#### v2x_ext_type.g

### src Directory
#### sample.c
*main file

### requirements


## Usage
Usage: %s [OPTIONS]
  -o payload_type  : Raw, EncodedbyJ2735, eITSK00130, eKETI, eETRI (default : Raw)
  -p psid          : <decimal number> (default : 32)
  -y comm_type     : DSRC, LTEV2X, 5GNRV2X (default : 5GNRV2X)
  -t tx_port       : tx port dbm (default : 20)
  -s signer_id     : UNSECURED, CERTIFICATE, DIGEST, ALTERNATE (default : UNSECURED)
  -r priority      : 0~7 (default : 0)
  -c tx_count      : total tx count (default : 100)
  -d tx_delay      : msec delay (default : 100)
  -m total_time    : total exec second (default : 10)
  -h help       : help message
  
### Build
  gcc -o <실행파일명> sample.c -lpthread
### Run
  ./sample -c 1000000 -m 100000 
