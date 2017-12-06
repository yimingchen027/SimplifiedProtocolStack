#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "threadpool.h"
#include "message.h"

#include <ctime>
#include <sys/time.h>
#include <time.h>

using namespace std;
#define BUFLEN 2048


void send_ftp(int protocol_id, Message *msg);
void send_telnet(int protocol_id, Message *msg);
void send_rdp(int protocol_id, Message *msg);
void send_dns(int protocol_id, Message *msg);
void send_tcp(int protocol_id, Message *msg);
void send_udp(int protocol_id, Message *msg);
void send_ip(int protocol_id, Message *msg);
void send_eth(int protocol_id, Message *msg);
void deliver_eth(Message* msg);
void deliver_ip(Message* msg);
void deliver_tcp(Message* msg);
void deliver_udp(Message* msg);
void deliver_ftp(Message* msg);
void deliver_telnet(Message* msg);
void deliver_rdp(Message* msg);
void deliver_dns(Message* msg);

pthread_mutex_t m;
pthread_mutex_t m1;
pthread_mutex_t m_deliver;
pthread_mutex_t m3;
pthread_mutex_t m4;
pthread_mutex_t m5;
pthread_mutex_t m6;

pthread_mutex_t ftpsnd;
pthread_mutex_t rdpsnd;
pthread_mutex_t dnssnd;
pthread_mutex_t telsnd;

int dlv_ftp_count = 0;
int dlv_telnet_count = 0;
int dlv_rdp_count = 0;
int dlv_dns_count = 0;
int pool_count = 0;
/*----------------global states for send messages-------------*/
char *outBuf = new char[1024];
struct sockaddr_in remaddr;	/* remote address */
int fd, i, slen = sizeof(remaddr);
char *server = "127.0.0.1";
int portnum;

//recording start sending time and end receiving time
int startmili=0, endmili=0;

typedef struct
{
	int hlpID;
	char OI[8];
	int dataLength;
}ftpHeader;

typedef struct
{
	int hlpID;
	char OI[8];
	int dataLength;
}telnetHeader;

typedef struct
{
	int hlpID;
	char OI[12];
	int dataLength;
}rdpHeader;

typedef struct
{
	int hlpID;
	char OI[8];
	int dataLength;
}dnsHeader;

typedef struct
{
	int hlpID;
	char OI[4];
	int dataLength;
}tcpHeader;

typedef struct
{
	int hlpID;
	char OI[4];
	int dataLength;
}udpHeader;

typedef struct
{
	int hlpID;
	char OI[12];
	int dataLength;
}ipHeader;

typedef struct
{
	int hlpID;
	char OI[8];
	int dataLength;
}ethHeader;


void thread_pool(void *message)
{
	//cout << "thread pool created! : " << pool_count<< endl;
	pthread_mutex_lock(&m_deliver);
	deliver_eth( (Message*)message);
	pthread_mutex_unlock(&m_deliver);
	pool_count ++;
}

void* receive(void* message)
{
	int recv_count = 0;
	ThreadPool pool(100);
	struct sockaddr_in myaddr;	/* our address */
	struct sockaddr_in remaddr;	/* remote address */
	socklen_t addrlen = sizeof(remaddr);		/* length of addresses */
	int portnum_recv;
	int fd_recv;
	int recvlen;			/* # bytes received */
	char *buf_recv = new char[BUFLEN];
	/*------Create a receive socket-------*/
	if ((fd_recv = socket(AF_INET, SOCK_DGRAM, 0))==-1)
		cout << "failed creating socket" << endl;

	/* bind it to all local addresses and pick any port number */
		memset((char *)&myaddr, 0, sizeof(myaddr));
		myaddr.sin_family = AF_INET;
		myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
		myaddr.sin_port = htons(0);

	if (bind(fd_recv, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
				perror("bind failed");
				exit(1);//!!!!!!!
			}else{
				unsigned int length = sizeof(myaddr);
				if(getsockname(fd_recv, (struct sockaddr *)&myaddr, &length) < 0){
					cout << "getsockname error" << endl;
					return 0;
				}
				pthread_mutex_lock(&m);
				cout << "new assigned server port num is:" << ntohs(myaddr.sin_port) <<endl;
				pthread_mutex_unlock(&m);
				portnum_recv = ntohs(myaddr.sin_port);
			}

	for(;;){
		pthread_mutex_lock(&m);
		cout << "waiting on port:" << portnum_recv << endl;
		pthread_mutex_unlock(&m);
		recvlen = recvfrom(fd_recv, buf_recv, BUFLEN, 0, (struct sockaddr*)&remaddr, &addrlen);
		if(recv > 0){
			pthread_mutex_lock(&m);
			cout << "receiving successfully: "<< recv_count<< endl;
			pthread_mutex_unlock(&m);

			Message* me = new Message(buf_recv,1024);
			deliver_eth(me);
			//if (pool.thread_avail())
			//pool.dispatch_thread(thread_pool,(void*)me);
			recv_count ++;
		}
	}
}

void send(void)
{

	if (sendto(fd, outBuf, 1024, 0, (struct sockaddr *)&remaddr, slen)==-1) {
		perror("sendto");
		exit(1);
	}

}

void* ftp_app(void *message)
{

		int recvlen;			/* # bytes received */

		char *buf = new char[100];
		//buf = "FTP MESSAGE!";

		/*-----sending messages-----*/
		int i=0;
	while (i < 100) {
		if (i == 0) {
			struct timeval start;
			gettimeofday(&start, NULL);
			startmili = (start.tv_sec) * 1000 + (start.tv_usec) / 1000;

		}
		pthread_mutex_lock(&ftpsnd);
		sprintf(buf, "FTP MESSAGE %d!", i);
		Message *m = new Message(buf, 100);
		//memset(outBuf,0, sizeof(outBuf));
		int ftp_hlp = 0;
		send_ftp(ftp_hlp, m);
		//cout << "send ftp" << endl;
		
		i++;
		pthread_mutex_unlock(&ftpsnd);
		unsigned int x = 200000;
		usleep(x);

	}


		while(1);
}

void* telnet_app(void *message)
{
	int recvlen;			/* # bytes received */

			char *buf = new char[100];
			//buf = "TELNET MESSAGE!";

			int i=0;
			/*-----sending messages-----*/
			while(i< 100 )
			{
				pthread_mutex_lock(&telsnd);
				sprintf(buf, "TELNET MESSAGE %d!", i);
				Message *m = new Message(buf,100);
				//memset(outBuf,0, sizeof(outBuf));
				
				int telnet_id = 0;
				send_telnet(telnet_id, m);

				
				i++;
				pthread_mutex_unlock(&telsnd);
				unsigned int x = 200000;
				usleep(x);

			}


			while(1);
}

void* rdp_app(void *message)
{
	int recvlen;			/* # bytes received */

			char *buf = new char[100];
			//buf = "RDP MESSAGE!";

			int i=0;
			/*-----sending messages-----*/
			while(i< 100)
			{
				pthread_mutex_lock(&rdpsnd);
				sprintf(buf, "RDP MESSAGE %d!", i);
				Message *m = new Message(buf,100);
				//memset(outBuf,0, sizeof(outBuf));
				
				int rdp_id = 0;
				send_rdp(rdp_id, m);

				
				i++;
				pthread_mutex_unlock(&rdpsnd);
				unsigned int x = 200000;
				usleep(x);

			}


			while(1);
}

void* dns_app(void *message)
{
	int recvlen;			/* # bytes received */

			char *buf = new char[100];
			//buf = "DNS MESSAGE!";

			int i=0;
			/*-----sending messages-----*/
			while(i< 100 )
			{
				pthread_mutex_lock(&dnssnd);
				sprintf(buf, "DNS MESSAGE %d!", i);
				Message *m = new Message(buf,100);
				//memset(outBuf,0, sizeof(outBuf));
				
				int dns_id = 8;

				send_dns(dns_id, m);

				
				i++;
				pthread_mutex_unlock(&dnssnd);
				unsigned int x = 200000;
				usleep(x);

			}


			while(1);
}

int main()
{
	pthread_mutex_init(&m,NULL);
	pthread_mutex_init(&m1,NULL);
	pthread_mutex_init(&m_deliver,NULL);
	pthread_mutex_init(&m3,NULL);
	pthread_mutex_init(&m4,NULL);
	pthread_mutex_init(&ftpsnd,NULL);
	pthread_mutex_init(&telsnd,NULL);
	pthread_mutex_init(&dnssnd,NULL);
	pthread_mutex_init(&rdpsnd,NULL);

	/*----Creating Receiving Thread----*/
	pthread_t *th1;

	th1 = (pthread_t *) malloc(sizeof(pthread_t));
		if(pthread_create(th1, NULL, receive, NULL)){
			perror("error creating th1 thread!");
			exit(1);
		}

	/*------Create a send socket-------*/
	if ((fd=socket(AF_INET, SOCK_DGRAM, 0))==-1)
		cout << "failed creating socket" << endl;
	pthread_mutex_lock(&m);
	cout << "Please input a port numbuer: ";
	pthread_mutex_unlock(&m);
	cin >> portnum;


	/* now define remaddr, the address to whom we want to send messages */
	/* For convenience, the host address is expressed as a numeric IP address */
	/* that we will convert to a binary format via inet_aton */

	memset((char *) &remaddr, 0, sizeof(remaddr));
	remaddr.sin_family = AF_INET;
	//remaddr.sin_port = htons((unsigned short)atoi(portnum));
	remaddr.sin_port = htons((unsigned short)portnum);
	if (inet_aton(server, &remaddr.sin_addr)==0) {
			fprintf(stderr, "inet_aton() failed\n");
			exit(1);
		}

	/*------establish ftp app--------*/
	pthread_t *ftp;
	ftp = (pthread_t *) malloc(sizeof(pthread_t));
	if(pthread_create(ftp, NULL, ftp_app, NULL)){
				perror("error creating ftp thread!");
				exit(1);
			}

	/*------establish telnet app--------*/
	pthread_t *telnet;
	telnet = (pthread_t *)malloc(sizeof(pthread_t));
	if(pthread_create(telnet, NULL, telnet_app, NULL)){
		perror("error creating telnet thread!");
		exit(1);
	}

	/*------establish RDP app--------*/
	pthread_t *rdp;
	rdp = (pthread_t *)malloc(sizeof(pthread_t));
	if(pthread_create(rdp, NULL, rdp_app, NULL)){
		perror("error creating rdp thread!");
		exit(1);
	}

	/*------establish DNS app--------*/
	pthread_t *dns;
	dns = (pthread_t*)malloc(sizeof(pthread_t));
	if(pthread_create(dns, NULL, dns_app, NULL)){
		perror("error creating dns thread!");
		exit(1);
	}


	while(1);

}

void send_ftp(int protocol_id, Message *msg)
{
	pthread_mutex_lock(&m);
	//cout << "send_ftp" << endl;
	pthread_mutex_unlock(&m);

	ftpHeader * ftp_add = new ftpHeader;
	ftp_add->hlpID = protocol_id;
	ftp_add->dataLength = 100;
	msg->msgAddHdr((char*)ftp_add, sizeof(ftpHeader));
	int tcp_hlp = 5;
	//pthread_mutex_lock(&m3);
	send_tcp(tcp_hlp,msg);
	//pthread_mutex_unlock(&m3);
}


void send_telnet(int protocol_id, Message *msg)
{
	telnetHeader* telnet_add = new telnetHeader;
	telnet_add->hlpID = protocol_id;
	telnet_add->dataLength = 100;
	msg->msgAddHdr((char*)telnet_add, sizeof(telnetHeader));
	int tcp_hlp = 6;
	//pthread_mutex_lock(&m3);
	send_tcp(tcp_hlp,msg);
	//pthread_mutex_unlock(&m3);
}

void send_rdp(int protocol_id, Message *msg)
{
	rdpHeader* rdp_add = new rdpHeader;
	rdp_add->hlpID = protocol_id;
	rdp_add->dataLength = 100;
	msg->msgAddHdr((char*)rdp_add, sizeof(rdpHeader));
	int udp_hlp = 7;
	//pthread_mutex_lock(&m3);
	send_udp(udp_hlp,msg);
	//pthread_mutex_unlock(&m3);
}

void send_dns(int protocol_id, Message *msg)
{
	dnsHeader* dns_add = new dnsHeader;
	dns_add->hlpID = protocol_id;
	dns_add->dataLength = 100;
	msg->msgAddHdr((char*)dns_add, sizeof(dnsHeader));
	int udp_hlp = 8;
	//pthread_mutex_lock(&m3);
	send_udp(udp_hlp,msg);
	//pthread_mutex_unlock(&m3);
}

void send_tcp(int protocol_id, Message *msg)
{
	pthread_mutex_lock(&m);
	//cout << "send_tcp" << endl;
	pthread_mutex_unlock(&m);

	tcpHeader * tcp_add = new tcpHeader;
	tcp_add->hlpID = protocol_id;
	tcp_add->dataLength = 100;
	msg->msgAddHdr((char*)tcp_add, sizeof(tcpHeader));
	int ip_hlp = 3;
	//pthread_mutex_lock(&m3);
	send_ip(ip_hlp, msg);
	//pthread_mutex_unlock(&m3);
}

void send_udp(int protocol_id, Message *msg)
{
	pthread_mutex_lock(&m);
	//cout << "send_udp" << endl;
	pthread_mutex_unlock(&m);

	udpHeader * udp_add = new udpHeader;
	udp_add->hlpID = protocol_id;
	udp_add->dataLength = 100;
	msg->msgAddHdr((char*)udp_add, sizeof(udpHeader));
	int ip_hlp = 4;
	//pthread_mutex_lock(&m3);
	send_ip(ip_hlp, msg);
	//pthread_mutex_unlock(&m3);
}

void send_ip(int protocol_id, Message *msg)
{
	pthread_mutex_lock(&m);
	//cout << "send_ip" << endl;
	pthread_mutex_unlock(&m);

	ipHeader * ip_add = new ipHeader;
	ip_add->hlpID = protocol_id;
	ip_add->dataLength = 100;
	msg->msgAddHdr((char*)ip_add, sizeof(ipHeader));
	int eth_hlp = 2;
	send_eth(eth_hlp, msg);
}

void send_eth(int protocol_id, Message *msg)
{

	pthread_mutex_lock(&m);
	//cout << "send_eth" << endl;
	pthread_mutex_unlock(&m);
	ethHeader * eth_add = new ethHeader;
	eth_add->hlpID = protocol_id;
	eth_add->dataLength = 100;
	msg->msgAddHdr((char*)eth_add, sizeof(ethHeader));
	msg->msgFlat(outBuf);

	if (sendto(fd, outBuf, 1024, 0, (struct sockaddr *)&remaddr, slen)==-1) {
		perror("sendto");
		exit(1);
	}
	memset(outBuf, 0,1024);
}

void deliver_eth(Message* msg)
{
	ethHeader* eth_strip = new ethHeader;
	eth_strip = (ethHeader*)msg->msgStripHdr(sizeof(ethHeader));
	//cout << "ETH ID: "<<eth_strip->hlpID << " "<< msg->msgLen()<<endl;
	deliver_ip(msg);
}

void deliver_ip(Message* msg)
{
	ipHeader* ip_strip = new ipHeader;
	ip_strip = (ipHeader*)msg->msgStripHdr(sizeof(ipHeader));
	//cout << "IP ID: "<<ip_strip->hlpID  << " "<< msg->msgLen()<<endl;

	int HLP = ip_strip->hlpID;
	if(HLP == 3)
		deliver_tcp(msg);
	else if(HLP == 4)
		deliver_udp(msg);
}

void deliver_tcp(Message* msg)
{
	tcpHeader* tcp_strip = new tcpHeader;
	tcp_strip = (tcpHeader*)msg->msgStripHdr(sizeof(tcpHeader));
	//cout << "TCP ID: "<<tcp_strip->hlpID  << " "<< msg->msgLen()<<endl;
	int HLP = tcp_strip->hlpID;
	if(HLP == 5)
		deliver_ftp(msg);
	else if(HLP == 6)
		deliver_telnet(msg);
}

void deliver_udp(Message* msg)
{
	udpHeader* udp_strip = new udpHeader;
	udp_strip = (udpHeader*)msg->msgStripHdr(sizeof(udpHeader));
	//cout << "UDP ID: "<<udp_strip->hlpID  << " "<< msg->msgLen()<<endl;
	int HLP = udp_strip->hlpID;
		if(HLP == 7)
			deliver_rdp(msg);
		else if(HLP == 8)
			deliver_dns(msg);
}

void deliver_ftp(Message* msg)
{
	char* printBuf = new char[100];

	ftpHeader* ftp_strip = new ftpHeader;
	ftp_strip = (ftpHeader*)msg->msgStripHdr(sizeof(ftpHeader));
	msg->msgFlat(printBuf);
	//cout << "FTP ID: "<<ftp_strip->hlpID  << " "<< msg->msgLen()<<endl;
	pthread_mutex_lock(&m);
	cout << "FTP received original message: " << printBuf << endl;
	pthread_mutex_unlock(&m);

	pthread_mutex_lock(&m3);
	dlv_ftp_count++;
	pthread_mutex_unlock(&m3);
	
	if ((strcmp(printBuf, "This is the FTP message 99!") == 0)||(strcmp(printBuf, "FTP MESSAGE 99!") == 0)) {
		struct timeval end;
		gettimeofday(&end, NULL);
		endmili = (end.tv_sec) * 1000 + (end.tv_usec) / 1000;
		printf("start sending at %d, end receiving at %d\n", startmili,
				endmili);
	}

}

void deliver_telnet(Message* msg)
{
	char* printBuf = new char[100];

	telnetHeader* telnet_strip = new telnetHeader;
	telnet_strip = (telnetHeader*)msg->msgStripHdr(sizeof(telnetHeader));
	msg->msgFlat(printBuf);
	//cout << "TELNET ID: "<<telnet_strip->hlpID  << " "<< msg->msgLen()<<endl;
	pthread_mutex_lock(&m);
	cout << "telnet received original message: " << printBuf << endl;
	pthread_mutex_unlock(&m);
	pthread_mutex_lock(&m3);
	dlv_telnet_count++;
	pthread_mutex_unlock(&m3);

}

void deliver_rdp(Message* msg)
{
	char* printBuf = new char[100];

	rdpHeader* rdp_strip = new rdpHeader;
	rdp_strip = (rdpHeader*)msg->msgStripHdr(sizeof(rdpHeader));
	msg->msgFlat(printBuf);
	pthread_mutex_lock(&m);
	cout << "RDP received original message: " << printBuf << endl;
	pthread_mutex_unlock(&m);
	pthread_mutex_lock(&m3);
	dlv_rdp_count ++;
	pthread_mutex_unlock(&m3);

}

void deliver_dns(Message* msg)
{
	char* printBuf = new char[100];

	dnsHeader* dns_strip = new dnsHeader;
	dns_strip = (dnsHeader*)msg->msgStripHdr(sizeof(dnsHeader));
	msg->msgFlat(printBuf);
	pthread_mutex_lock(&m);
	cout << "DNS received original message: " << printBuf << endl;
	pthread_mutex_unlock(&m);
	pthread_mutex_lock(&m3);
	dlv_dns_count ++;
	pthread_mutex_unlock(&m3);

}


