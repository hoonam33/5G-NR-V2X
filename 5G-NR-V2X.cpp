#include <sstream>
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include "sys/types.h"
#include "netinet/in.h"
#include <iostream>
#include <vector>
#include "thread"
#include "string"
#include <pqxx/pqxx>
#include "/home/kiapi/catkin_ws/src/ublox_f9p/avb/libavb/libavb.h"
#include <chrono>
#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <stdio.h>
#include <cstring>
#include "../include/NmpntExtern.h"
#include <sys/select.h>

int sock;
std::vector<char> bytes01;
std::vector<char> bytes03;

char rx[256];
int i=1;

uint16_t magic_num = 61937;
uint16_t ver = 0;
uint8_t e_action = 0;
uint8_t e_payload_type = 0;
uint8_t reserved1[2];
uint32_t psid = htonl(5271);
uint8_t reserved2[4];

uint16_t magic_num_tx = htons(62194);
uint8_t e_v2x_comm_type = 3;
uint8_t elements_indicator = 1;
int8_t tx_power = 20;
uint8_t e_signer_id = 0;
uint8_t e_priority = 0;
uint8_t channel_load = 0;
uint8_t reserved1_tx;
uint64_t expiry_time = 0;
uint32_t transmitter_profile_id = htonl(100);
uint32_t peer_l2id = htonl(0);
uint8_t reserved3[8];
uint16_t length = htons(35);
uint8_t data[35];

#pragma pack(push, 1)
struct basic
{
	uint16_t device_id;
	uint8_t device_type;
	uint64_t timestamp;
	int32_t latitude;
	int32_t longitude;
	uint16_t heading;
	uint16_t speed;
	char link_id[12];
};
#pragma pack(pop)
basic basic_payload;

std::string id;

	uint16_t device_id_rx;
	uint8_t device_type_rx;
	uint64_t timestamp_rx;
	int32_t latitude_rx;
	int32_t longitude_rx;
	uint16_t heading_rx;
	uint16_t speed_rx;
	char link_id_rx[12];

  int serial_port;
  uint16_t heading_pre;

  NmpntClient *__client = NULL;
  char __mountpoint[32] = "SEJU_RTCM32";

  static void ntrip(void *self, char *data, int size){

    write(serial_port, data, size);
    printf("%s : ntrip : %d : %s\n", __mountpoint, size, data);
  }

  static void state(void *self, int status)
{
	printf("%s : ", __mountpoint);
	switch (status)
	{
	case NTRIP_ONAIR:
		printf("서비스를 시작하였습니다.\n");
		break;
	case NTRIP_OFFAIR:
		printf("서비스를 정지하였습니다.\n");
		break;
	case NTRIP_SOCKET_ERROR:
		printf("네트워크 오류입니다.\n");
		break;
	case NTRIP_CONNECT_ERROR:
		printf("네트워크 상태를 확인하십시요.\n");
		break;
	case NTRIP_TIMEOUT_ERROR:
		printf("네트워크 상태가 좋지 않습니다.\n");
		break;
	case NTRIP_CONNECT_CLOSE:
		printf("네트워크 연결이 끊겼습니다.\n");
		break;
	case NTRIP_MOUNT_ERROR:
		printf("지원하지 않는 마운트명입니다.\n");
		break;
	case NTRIP_USER_ERROR:
		printf("사용자정보가 틀립니다.\n");
		break;
	case NTRIP_VERSION_ERROR:
		printf("지원하지 않는 버전입니다.\n");
		break;
	case NTRIP_CASTER_ERROR:
		printf("정상적인 캐스터가 아닙니다.\n");
		break;
	default:
		printf("알 수 없는 오류입니다.\n");
		break;
	}
}

template <typename T>
void to_bytes(const T& object, std::vector<char>& bytes01) {
    const char* begin = reinterpret_cast<const char*>(std::addressof(object));
    const char* end = begin + sizeof(T);
    bytes01.insert(bytes01.end(), begin, end);
}

template <typename Y>
void to_bytes1(const Y& object, std::vector<char>& bytes03) {
    const char* begin = reinterpret_cast<const char*>(std::addressof(object));
    const char* end = begin + sizeof(Y);
    bytes03.insert(bytes03.end(), begin, end);
}
//crc-32
   
static uint32_t crc32_tab[] = {
    0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
    0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
    0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
    0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
    0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9,
    0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
    0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c,
    0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
    0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
    0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
    0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190, 0x01db7106,
    0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
    0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,
    0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
    0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
    0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
    0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7,
    0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
    0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa,
    0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
    0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
    0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
    0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84,
    0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
    0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
    0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
    0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
    0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
    0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55,
    0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
    0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28,
    0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
    0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
    0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
    0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
    0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
    0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69,
    0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
    0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
    0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
    0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693,
    0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
    0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d};

static uint32_t crc32(uint32_t crc_in, const uint8_t* buf, int size) {
  const uint8_t* p = buf;
  uint32_t crc;
  crc = crc_in ^ ~0U;
  while (size--)
    crc = crc32_tab[(crc ^ *p++) & 0xFF] ^ (crc >> 8);
  return crc ^ ~0U;
}
uint32_t avb_crc32(const uint8_t* buf, size_t size) {
  return crc32(0, buf, size);
}

void realtime_send(std::vector<std::string>& fields)
{
  auto current_time = std::chrono::system_clock::now();
  auto timestamp_n = std::chrono::duration_cast<std::chrono::milliseconds>(current_time.time_since_epoch()).count();

  float heading_tf;
  uint16_t heading_RT;
  std::string convert_lat;
  std::string convert_lat_dd;
  std::string convert_lat_mm;
  std::string convert_lon;
  std::string convert_lon_dd;
  std::string convert_lon_mm;

  double lat_tf = std::stof(fields[3]);
  double lon_tf = std::stof(fields[5]);
  double speed_tf = std::stof(fields[7]);
  if(fields[8].empty() == true){
    heading_RT = heading_pre;
  }
  else{
    heading_tf = std::stof(fields[8]);
  }

  lat_tf *=10000000;
  convert_lat = std::to_string(lat_tf);
  convert_lat_dd = convert_lat.substr(0,2);
  convert_lat_mm = convert_lat.substr(2,9);
  convert_lat_mm = std::to_string(std::stoi(convert_lat_mm)/60);
  lat_tf = std::stoi(convert_lat_dd + convert_lat_mm);

  lon_tf *=10000000;
  convert_lon = std::to_string(lon_tf);
  convert_lon_dd = convert_lon.substr(0,3);
  convert_lon_mm = convert_lon.substr(3,10);
  convert_lon_mm = std::to_string(std::stoi(convert_lon_mm)/60);
  lon_tf = std::stoi(convert_lon_dd + convert_lon_mm);

  heading_tf = heading_tf*80;

  speed_tf = (speed_tf)*0.514447;
  speed_tf = (speed_tf)*50;

   
  double latitude_raw;
  double longitude_raw;
  
  latitude_raw = (lat_tf)*0.0000001;  
  longitude_raw = (lon_tf)*0.0000001;
  
  pqxx::connection connection("dbname=kiapimap user=postgres password=1 hostaddr=192.168.11.133 port=5433");
  pqxx::work txn(connection);

    std::string query = "SELECT id " 
						"FROM a2_link "
                        "ORDER BY ST_Distance("
                        "geometry_wgs84, "
                        "ST_SetSRID(ST_Point(" + std::to_string(longitude_raw) + ", " + std::to_string(latitude_raw) + "), 4326)"
                        ") ASC "
                        "LIMIT 1;";

    pqxx::result r = txn.exec(query);
   
    for (auto row: r) {
        //std::cout << "ID: " << row["id"].as<std::string>() << std::endl;
        id = row["id"].as<std::string>();
    }

	//std::cout << id.size() << id << basic_payload.link_id << std::endl;
   
  memset(&basic_payload, 0, sizeof(basic_payload));
  
  basic_payload.device_id = 11;
  basic_payload.device_type = 1;
  basic_payload.timestamp = timestamp_n;
  basic_payload.latitude = lat_tf;
  basic_payload.longitude = lon_tf;
  if(fields[8].empty() == false){
    basic_payload.heading = heading_tf;
  }
  basic_payload.speed = speed_tf;
  

  heading_pre = basic_payload.heading;
  
    strncpy(basic_payload.link_id, id.c_str(), sizeof(basic_payload.link_id));
    //basic_payload.link_id[sizeof(basic_payload.link_id) - 1] = '\0';
  
  
  memcpy(data, &basic_payload, sizeof(basic_payload));
  
   for(i;i<2;i++){
   to_bytes(magic_num, bytes01);
   to_bytes(ver, bytes01);
   to_bytes(e_action, bytes01);
   to_bytes(e_payload_type, bytes01);
   to_bytes(reserved1, bytes01);
   to_bytes(psid, bytes01);
   to_bytes(reserved2, bytes01);
  
   
    send(sock , bytes01.data(), bytes01.size(), 0);
															}

  to_bytes1(magic_num_tx, bytes03);
  to_bytes1(ver, bytes03);
  to_bytes1(psid, bytes03);
  to_bytes1(e_v2x_comm_type, bytes03);
  to_bytes1(e_payload_type, bytes03);
  to_bytes1(elements_indicator, bytes03);
  to_bytes1(tx_power, bytes03);
  to_bytes1(e_signer_id, bytes03);
  to_bytes1(e_priority, bytes03);
  to_bytes1(channel_load, bytes03);
  to_bytes1(reserved1_tx, bytes03);
  to_bytes1(expiry_time, bytes03);
  to_bytes1(transmitter_profile_id, bytes03);
  to_bytes1(peer_l2id, bytes03);
  to_bytes1(reserved2, bytes03);
  to_bytes1(reserved3, bytes03);
  
 uint32_t crc_value = avb_crc32(data, sizeof(data));
 
  to_bytes1(crc_value, bytes03);
  to_bytes1(length, bytes03);
  to_bytes1(data, bytes03);
  
  send(sock , bytes03.data(), bytes03.size(), 0);

  bytes03.clear();
  
 
  // Send the message over TCP
	
	
}

void receive_data(int sock) {
    
    while (true) {
        int bytesRead = recv(sock, rx, sizeof(rx), 0);
        if (bytesRead < 0) {
            perror("recv failed");
            break;
        } else if (bytesRead == 0) {
            printf("connection closed");
            break;
        } else {
            // buffer에 저장된 데이터를 사용합니다.
             //printf("Received: %s\n", EPD0102);
        }
				
			memcpy(&device_id_rx, &rx[38], sizeof(device_id_rx));
			memcpy(&device_type_rx, &rx[40], sizeof(device_type_rx));
			memcpy(&timestamp_rx, &rx[41], sizeof(timestamp_rx));
			memcpy(&latitude_rx, &rx[49], sizeof(latitude_rx));
			memcpy(&longitude_rx, &rx[53], sizeof(longitude_rx));
			memcpy(&heading_rx, &rx[57], sizeof(heading_rx));
			memcpy(&speed_rx, &rx[59], sizeof(speed_rx));
			memcpy(&link_id_rx, &rx[61], sizeof(link_id_rx));

		printf("device_id : %d\n", device_id_rx);
		printf("device_type : %d\n", device_type_rx);
		printf("timestamp : %lu\n", timestamp_rx);
		printf("latitude : %lf\n", latitude_rx*0.0000001);
		printf("longitude : %lf\n", longitude_rx*0.0000001);
		printf("heading : %lf\n", heading_rx*0.0125);
		printf("speed : %lf km/h \n", speed_rx*0.02*3.6);
		printf("link_id : %s\n\n", link_id_rx);
    
    }
}

void serial_conect(){
 std::string buffer;
 std::string realtime_m;
 std::string realtime_m1;
 std::vector<std::string> fields;

 serial_port = open("/dev/ttyACM0", O_RDWR);

  fd_set read_fds;
  struct timeval timeout;
  int max_fd = serial_port + 1;


 struct termios tty;
 tcgetattr(serial_port, &tty);

 cfsetispeed(&tty, B38400);
 cfsetospeed(&tty, B38400);
 tty.c_cflag &= ~PARENB;
 tty.c_cflag &= ~CSTOPB;
 tty.c_cflag |= CS8;

 tcsetattr(serial_port, TCSANOW, &tty);

 while(1){

  FD_ZERO(&read_fds);
  FD_SET(serial_port, &read_fds);

  timeout.tv_sec = 1;
  timeout.tv_usec = 0;

if (select(max_fd, &read_fds, NULL, NULL, &timeout) > 0) {
            // 데이터 읽기
  char buf[1024];
  int num_bytes = read(serial_port, buf, sizeof(buf));

    if (num_bytes < 0) {
      std::cerr << "Error reading: " << strerror(errno) << std::endl;
        break;
       } else if (num_bytes > 0) {
      // 읽은 데이터를 std::string buffer에 추가
     buffer.append(buf, num_bytes);
            }
}

 /* buffer.resize(2048);
  int num_bytes = read(serial_port, &buffer[0], buffer.size());

  if(num_bytes < 0){
    std::cerr << "Error reading: " << strerror(errno) << std::endl;
  }*/

  size_t RMC_PATH = buffer.find("$GNRMC");
  realtime_m = buffer.substr(RMC_PATH);

  std::istringstream iss(realtime_m);

  while(std::getline(iss, realtime_m1, ',')){
    fields.push_back(realtime_m1);
  }

  realtime_send(fields);

  fields.clear();

  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
 }

}

void ntrip_client(){

  __client = NmpntConstruct();
  NmpntStart(__client, NULL, __mountpoint, NTRIP_PURPOSE_RND, &ntrip, &state);
  getchar();

}

int main(int argc, char **argv)
{
	
	struct sockaddr_in server, client;

  //Create socket
  sock = socket(AF_INET , SOCK_STREAM , 0);
  if (sock == -1)
  {
      printf("Could not create socket");
      return 1;
  }
  puts("Socket created");
  

  server.sin_addr.s_addr = inet_addr("192.168.1.11");  // Server IP
  server.sin_family = AF_INET;
  server.sin_port = htons(47347);  // Server Port

  //Connect to remote server
  if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
  {
      perror("connect failed. Error");
      return 1;
  }
  
   
	std::thread recvThread(receive_data, sock);
    recvThread.detach();
  //std::thread ntripThread(ntrip_client);
    //ntripThread.detach();
    
  serial_conect();


  return 0;
}
