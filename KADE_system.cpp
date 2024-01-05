#include <sstream>
#include <fstream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "sys/types.h"
#include "netinet/in.h"
#include <iostream>
#include <vector>
#include "thread"
#include "string"
#include <pqxx/pqxx>
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

char rx[256];
uint16_t heading_pre;
std::string id;
int sock;
uint8_t data[35];
int serial_port;

NmpntClient *__client = NULL;
char __mountpoint[32] = "SEJU_RTCM32";

#pragma pack(push, 1)
struct basic
{
    char msgID[1];
    uint16_t deviceID;
    uint8_t deviceType;
    uint8_t commType;
    char hwVersion[1];
    char swVersion[1];
    uint32_t msgLength;
    uint8_t vehicleType;
    char vehicleNumber[11];
    uint8_t avLevel;
    uint16_t driverID;
    uint8_t drivingMode;
    uint8_t drivingRegion;
    uint64_t txTimestamp;
    char txSeqNum;
    bool receiveFlag;
    int latitude;
    int longitude;
    int elevation;
    char linkID[13];
    uint16_t heading;
    uint16_t speed;
    char optDataFlag;
    char stdMsgCount;
    uint16_t obuDataSize;
};
#pragma pack(pop)
basic basic_payload;

void config_check()
{
    std::ifstream config;
    std::string part;
    std::vector<std::string> set_part;

    // ipfile.open("./server_ip.txt");
    // ipfile_c.open("./client_ip.txt");
    config.open("./config");

    while (getline(config, part))
    {
        size_t firstnumber = part.find("\"");
        if (firstnumber != std::string::npos)
        {
            size_t secondnumber = part.find("\"", firstnumber + 1);
            if (secondnumber != std::string::npos)
            {
                std::string ips = part.substr(firstnumber + 1, secondnumber - firstnumber - 1);

                set_part.push_back(ips);
            }
        }
    }

    std::strcpy(basic_payload.msgID, set_part[0].c_str());
    basic_payload.deviceID = std::stoi(set_part[1]);
    basic_payload.deviceType = std::stoi(set_part[2]);
    basic_payload.commType = std::stoi(set_part[3]);
    std::strcpy(basic_payload.hwVersion, set_part[4].c_str());
    std::strcpy(basic_payload.swVersion, set_part[5].c_str());
    basic_payload.msgLength = std::stoi(set_part[6]);
    basic_payload.vehicleType = std::stoi(set_part[7]);
    std::strcpy(basic_payload.vehicleNumber, set_part[8].c_str());
    basic_payload.avLevel = std::stoi(set_part[9]);
    basic_payload.driverID = std::stoi(set_part[10]);
    basic_payload.drivingMode = std::stoi(set_part[11]);
    basic_payload.drivingRegion = std::stoi(set_part[12]);
}

static void ntrip(void *self, char *data, int size)                       // ntrip callback
{
    write(serial_port, data, size);
    printf("%s : ntrip : %d : %s\n", __mountpoint, size, data);
}

static void state(void *self, int status)                                 // state callback
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

void ntrip_client()      // ntrip 클라이언트, 추후 NmpntState 추가 요
{
    __client = NmpntConstruct();
    NmpntStart(__client, NULL, __mountpoint, NTRIP_PURPOSE_RND, &ntrip, &state);
    getchar();
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
				
    }
}

void packet_send(basic basic_payload)
{
    send(sock, &basic_payload, sizeof(basic_payload), 0); // 최종 패킷으로 변경
}

void GPS_packet(std::vector<std::string> &fields_RMC, std::vector<std::string> &fields_GGA)                                  //실시간 서버 전송
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

    config_check();

    double lat_tf = std::stof(fields_RMC[3]);
    double lon_tf = std::stof(fields_RMC[5]);
    double ele_tf = std::stof(fields_GGA[10]);
    double speed_tf = std::stof(fields_RMC[7]);
    if (fields_RMC[8].empty() == true)
    {
        heading_RT = heading_pre;
    }
    else
    {
        heading_tf = std::stof(fields_RMC[8]);
    }

    lat_tf *= 10000000;
    convert_lat = std::to_string(lat_tf);
    convert_lat_dd = convert_lat.substr(0, 2);
    convert_lat_mm = convert_lat.substr(2, 9);
    convert_lat_mm = std::to_string(std::stoi(convert_lat_mm) / 60);
    lat_tf = std::stoi(convert_lat_dd + convert_lat_mm);

    lon_tf *= 10000000;
    convert_lon = std::to_string(lon_tf);
    convert_lon_dd = convert_lon.substr(0, 3);
    convert_lon_mm = convert_lon.substr(3, 10);
    convert_lon_mm = std::to_string(std::stoi(convert_lon_mm) / 60);
    lon_tf = std::stoi(convert_lon_dd + convert_lon_mm);


    ele_tf = ele_tf * 10;

    heading_tf = heading_tf * 80;

    speed_tf = (speed_tf) * 0.514447;
    speed_tf = (speed_tf) * 50;

    double latitude_raw;
    double longitude_raw;

    latitude_raw = (lat_tf) * 0.0000001;
    longitude_raw = (lon_tf) * 0.0000001;

    pqxx::connection connection("dbname=kiapimap user=postgres password=1 hostaddr=192.168.11.133 port=5433");
    pqxx::work txn(connection);

    std::string query = "SELECT id "
                        "FROM a2_link "
                        "ORDER BY ST_Distance("
                        "geometry_wgs84, "
                        "ST_SetSRID(ST_Point(" +
                        std::to_string(longitude_raw) + ", " + std::to_string(latitude_raw) + "), 4326)"
                                                                                              ") ASC "
                                                                                              "LIMIT 1;";

    pqxx::result r = txn.exec(query);

    for (auto row : r)
    {
        // std::cout << "ID: " << row["id"].as<std::string>() << std::endl;
        id = row["id"].as<std::string>();
    }

    // std::cout << id.size() << id << basic_payload.linkID << std::endl;

    memset(&basic_payload, 0, sizeof(basic_payload));

    
    basic_payload.txTimestamp = timestamp_n;
    basic_payload.latitude = lat_tf;
    basic_payload.longitude = lon_tf;
    basic_payload.elevation = ele_tf;
    if (fields_RMC[8].empty() == false)
    {
        basic_payload.heading = heading_tf;
    }
    basic_payload.speed = speed_tf;

    heading_pre = basic_payload.heading;

    strncpy(basic_payload.linkID, id.c_str(), sizeof(basic_payload.linkID));

    packet_send(basic_payload);
}

void serial_connect()                                 // GPS에서 데이터 파싱     
{ 
    
    std::string buffer;
    size_t RMC_PATH;
    size_t GGA_PATH;
    std::string realtime_m;
    std::string realtime_m1;
    std::string realtime_RMC;
    std::string realtime_GGA;
    std::vector<std::string> fields_RMC;
    std::vector<std::string> fields_GGA;

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

    while (1)
    {

        FD_ZERO(&read_fds);
        FD_SET(serial_port, &read_fds);

        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        if (select(max_fd, &read_fds, NULL, NULL, &timeout) > 0)
        {

            char buf[1024];
            int num_bytes = read(serial_port, buf, sizeof(buf));

            if (num_bytes < 0)
            {
                std::cerr << "Error reading: " << strerror(errno) << std::endl;
                break;
            }
            else if (num_bytes > 0)
            {
                buffer.append(buf, num_bytes);
            }
        }
        while (1)
        {
            if (buffer.find("$GNRMC") != std::string::npos)
            {
                RMC_PATH = buffer.find("$GNRMC");
                break;
            }
        }
        realtime_m = buffer.substr(RMC_PATH);

        while (1)
        {
            if (buffer.find("$GNGGA") != std::string::npos)
            {
                GGA_PATH = buffer.find("$GNGGA");
                break;
            }
        }
        realtime_m1 = buffer.substr(GGA_PATH);

        std::istringstream iss(realtime_m);
        std::istringstream iss1(realtime_m1);

        while (std::getline(iss, realtime_RMC, ','))
        {
            fields_RMC.push_back(realtime_RMC);
        }

        while (std::getline(iss1, realtime_GGA, ','))
        {
            fields_GGA.push_back(realtime_GGA);
        }

        GPS_packet(fields_RMC, fields_GGA);

        fields_RMC.clear();
        fields_GGA.clear();

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

void OBU_eth()
{
    const int port = 12001;     // 서버 포트 설정
    int server_fd, new_socket;  // 소켓 파일 디스크립터
    struct sockaddr_in address; // 주소 정보
    int opt = 1;
    int addrlen = sizeof(address);

    // 소켓 생성
    if ((server_fd = socket(PF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // 소켓 옵션 설정
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("192.168.70.10");
    address.sin_port = htons(port);

    // 바인딩
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // 리스닝
    if (listen(server_fd, 5) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    
    while (true)
    {
        printf("Waiting for OBU...\n");
        // 연결 수락
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        std::cout << "OBU connection" << std::endl;

        char buffer[1024] = {0};

        // 데이터 송수신
        while (true)
        {
            ssize_t valread = read(new_socket, buffer, 1024);

            //spat 데이터 처리 로직

            packet_send(basic_payload);

            if (valread > 0)                                       
            {
                std::cout << "Data received: " << buffer << std::endl;
                write(new_socket, buffer, valread);
            }
            else if (valread == 0)
            {
                std::cout << "OBU disconnected" << std::endl;
                break;
            }
            else
            {
                perror("recv");
                break;
            }
            memset(buffer, 0, 1024);
        }
    }
}

int main(int argc, char **argv)
{

    struct sockaddr_in server;

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        printf("Could not create socket");
        return 1;
    }
    puts("Socket created");

    server.sin_addr.s_addr = inet_addr("192.168.11.133"); // Server IP
    server.sin_family = AF_INET;
    server.sin_port = htons(8889); // Server Port

    // Connect to remote server
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return 1;
    }

    std::thread recvThread(receive_data, sock);
    recvThread.detach();

    std::thread OBU_ethThread(OBU_eth);
    OBU_ethThread.detach();

    serial_connect();

    return 0;
}