

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

#define BUFLEN 2048

int ftp_send_pipe[2];
int ftp_recv_pipe[2];
int telnet_send_pipe[2];
int telnet_recv_pipe[2];
int rdp_send_pipe[2];
int rdp_recv_pipe[2];
int dns_send_pipe[2];
int dns_recv_pipe[2];
int tcp_send_pipe[2];
int tcp_recv_pipe[2];
int udp_send_pipe[2];
int udp_recv_pipe[2];
int ip_send_pipe[2];
int ip_recv_pipe[2];
int eth_send_pipe[2];
int eth_recv_pipe[2];

pthread_mutex_t m;   //cout lock
pthread_mutex_t tcp_send_m;
pthread_mutex_t udp_send_m;
pthread_mutex_t ip_send_m;

/*----------------global states for send messages-------------*/
char *outBuf = new char[1024];
struct sockaddr_in remaddr;	/* remote address */
int fd, i, slen = sizeof(remaddr);
char *server = "127.0.0.1";
int portnum;

//recording start sending time and end receiving time
int startmili=0, endmili=0;

typedef struct {
	int protocol_id;
	Message* message;
} send_pipe_unit;

typedef struct {
	Message* message;
} recv_pipe_unit;

typedef struct {
	int hlpID;
	char OI[8];
	int dataLength;
} ftpHeader;

typedef struct {
	int hlpID;
	char OI[8];
	int dataLength;
} telnetHeader;

typedef struct {
	int hlpID;
	char OI[12];
	int dataLength;
} rdpHeader;

typedef struct {
	int hlpID;
	char OI[8];
	int dataLength;
} dnsHeader;

typedef struct {
	int hlpID;
	char OI[4];
	int dataLength;
} tcpHeader;

typedef struct {
	int hlpID;
	char OI[4];
	int dataLength;
} udpHeader;

typedef struct {
	int hlpID;
	char OI[12];
	int dataLength;
} ipHeader;

typedef struct {
	int hlpID;
	char OI[8];
	int dataLength;
} ethHeader;

void* ftp_send(void* message) {
	//attach its own header
	send_pipe_unit * s = new send_pipe_unit;

	for (;;) {
		/*-------read from pipe-------*/
		pthread_mutex_lock(&m);
		//cout << "ftp_send" << endl;
		pthread_mutex_unlock(&m);

		int result_read = read(ftp_send_pipe[0], (send_pipe_unit*) s, sizeof(send_pipe_unit));
		pthread_mutex_lock(&m);
		if(result_read < 0)
			cout << "ftp_send read failed" << endl;
		pthread_mutex_unlock(&m);


		ftpHeader* ftp_add = new ftpHeader;
		ftp_add->hlpID = 0;
		ftp_add->dataLength = 100;
		s->message->msgAddHdr((char*) ftp_add, sizeof(ftpHeader));

		/*------write to the pipe------*/
		s->protocol_id = 5;
		pthread_mutex_lock(&tcp_send_m);
		int result_write = write(tcp_send_pipe[1], (char*) s, sizeof(send_pipe_unit));
		pthread_mutex_unlock(&tcp_send_m);
		if (result_write < 0)
			cout << "ftp_send writing failed" << endl;

	}

}

void* telnet_send(void* message) {
	//attach its own header
	send_pipe_unit * s = new send_pipe_unit;

	for (;;) {
		pthread_mutex_lock(&m);
		//cout << "telnet_send" << endl;
		pthread_mutex_unlock(&m);

		/*-------read from pipe-------*/
		int result_read = read(telnet_send_pipe[0], (send_pipe_unit*) s,
				sizeof(send_pipe_unit));
		if (result_read < 0)
			cout << "telnet_send reading failed" << endl;

		telnetHeader* telnet_add = new telnetHeader;
		telnet_add->hlpID = 0;
		telnet_add->dataLength = 100;
		s->message->msgAddHdr((char*) telnet_add, sizeof(telnetHeader));

		/*------write to the pipe------*/
		s->protocol_id = 6;
		pthread_mutex_lock(&tcp_send_m);
		int result_write = write(tcp_send_pipe[1], (char*) s, sizeof(send_pipe_unit));
		pthread_mutex_unlock(&tcp_send_m);
		if (result_write < 0)
			cout << "ftp_send writing failed" << endl;

	}
}

void* rdp_send(void* message) {
	//attach its own header
	send_pipe_unit * s = new send_pipe_unit;

	for (;;) {
		pthread_mutex_lock(&m);
		//cout << "rdp_send" << endl;
		pthread_mutex_unlock(&m);

		/*-------read from pipe-------*/
		int result_read = read(rdp_send_pipe[0], (send_pipe_unit*) s,
				sizeof(send_pipe_unit));
		if (result_read < 0)
			cout << "rdp_send reading failed" << endl;

		rdpHeader* rdp_add = new rdpHeader;
		rdp_add->hlpID = 0;
		rdp_add->dataLength = 100;
		s->message->msgAddHdr((char*) rdp_add, sizeof(rdpHeader));

		/*------write to the pipe------*/
		s->protocol_id = 7;
		pthread_mutex_lock(&udp_send_m);
		int result_write = write(udp_send_pipe[1], (char*) s, sizeof(send_pipe_unit));
		pthread_mutex_unlock(&udp_send_m);

		if (result_write < 0)
			cout << "rdp_send writing failed" << endl;

	}
}

void* dns_send(void* message) {
	//attach its own header
	send_pipe_unit * s = new send_pipe_unit;

	for (;;) {
		pthread_mutex_lock(&m);
		//cout << "dns_send" << endl;
		pthread_mutex_unlock(&m);

		/*-------read from pipe-------*/
		int result_read = read(dns_send_pipe[0], (send_pipe_unit*) s,
				sizeof(send_pipe_unit));
		if (result_read < 0)
			cout << "dns_send reading failed" << endl;

		dnsHeader* dns_add = new dnsHeader;
		dns_add->hlpID = 0;
		dns_add->dataLength = 100;
		s->message->msgAddHdr((char*) dns_add, sizeof(dnsHeader));

		/*------write to the pipe------*/
		s->protocol_id = 8;
		pthread_mutex_lock(&udp_send_m);
		int result_write = write(udp_send_pipe[1], (char*) s, sizeof(send_pipe_unit));
		pthread_mutex_unlock(&udp_send_m);

		if (result_write < 0)
			cout << "dns_send writing failed" << endl;

	}
}

void* tcp_send(void* message) {
	//attach its own header
	send_pipe_unit * s = new send_pipe_unit;

	for (;;) {
		/*-------read from pipe-------*/
		pthread_mutex_lock(&m);
		//cout << "tcp_send" << endl;
		pthread_mutex_unlock(&m);

		int result_read = read(tcp_send_pipe[0], (send_pipe_unit*) s,
				sizeof(send_pipe_unit));

		//pthread_mutex_lock(&m);
		//cout << "result_read: " << endl;
		//pthread_mutex_unlock(&m);

		if (result_read < 0)
			cout << "ftp_send reading failed" << endl;

		tcpHeader* tcp_add = new tcpHeader;
		//pthread_mutex_lock(&m);
		//cout << "protocol_id: " << s->protocol_id << endl;
		//pthread_mutex_unlock(&m);

		/*-----add header-----*/
		if (s->protocol_id == 5)
			tcp_add->hlpID = 5;
		else if (s->protocol_id == 6)
			tcp_add->hlpID = 6;
		tcp_add->dataLength = 100;
		s->message->msgAddHdr((char*) tcp_add, sizeof(tcpHeader));

		/*-----write to pipe-----*/
		s->protocol_id = 3;
		pthread_mutex_lock(&ip_send_m);
		int result_write = write(ip_send_pipe[1], (char*) s, sizeof(send_pipe_unit));
		pthread_mutex_unlock(&ip_send_m);
		if (result_write < 0)
			cout << "tcp_send writing failed" << endl;
	}

}

void* udp_send(void* message) {
	//attach its own header
	send_pipe_unit * s = new send_pipe_unit;

	for (;;) {
		/*-------read from pipe-------*/
		pthread_mutex_lock(&m);
		//cout << "udp_send" << endl;
		pthread_mutex_unlock(&m);

		int result_read = read(udp_send_pipe[0], (send_pipe_unit*) s,
				sizeof(send_pipe_unit));

		if (result_read < 0)
			cout << "udp_send reading failed" << endl;

		udpHeader* udp_add = new udpHeader;

		/*-----add header-----*/
		if (s->protocol_id == 7)
			udp_add->hlpID = 7;
		else if (s->protocol_id == 8)
			udp_add->hlpID = 8;
		udp_add->dataLength = 100;
		s->message->msgAddHdr((char*) udp_add, sizeof(udpHeader));

		/*-----write to pipe-----*/
		s->protocol_id = 4; // udp id
		int result_write = write(ip_send_pipe[1], (char*) s,
				sizeof(send_pipe_unit));
		if (result_write < 0)
			cout << "udp_send writing failed" << endl;
	}

}

void* ip_send(void* message) {
	//attach its own header
	send_pipe_unit * s = new send_pipe_unit;

	for (;;) {
		/*-------read from pipe-------*/
		pthread_mutex_lock(&m);
		//cout << "ip_send" << endl;
		pthread_mutex_unlock(&m);

		int result_read = read(ip_send_pipe[0], (send_pipe_unit*) s,
				sizeof(send_pipe_unit));

		if (result_read < 0)
			cout << "ip_send reading failed" << endl;

		udpHeader* ip_add = new udpHeader;

		/*-----add header-----*/
		if (s->protocol_id == 3)
			ip_add->hlpID = 3;
		else if (s->protocol_id == 4)
			ip_add->hlpID = 4;
		ip_add->dataLength = 100;
		s->message->msgAddHdr((char*)ip_add, sizeof(ipHeader));

		/*-----write to pipe-----*/
		s->protocol_id = 2; // udp id
		int result_write = write(eth_send_pipe[1], (char*) s, sizeof(send_pipe_unit));
		if (result_write < 0)
			cout << "ip_send writing failed" << endl;
	}

}

void* eth_send(void* message) {
	//attach its own header
	send_pipe_unit * s = new send_pipe_unit;

	for (;;) {
		pthread_mutex_lock(&m);
		//cout << "eth_send" << endl;
		pthread_mutex_unlock(&m);

		/*-------read from pipe-------*/

		int result_read = read(eth_send_pipe[0], (send_pipe_unit*) s, sizeof(send_pipe_unit));

		if (result_read < 0)
			cout << "eth_send reading failed" << endl;

		ethHeader* eth_add = new ethHeader;

		/*-----add header-----*/
		eth_add->hlpID = 2;
		eth_add->dataLength = 100;
		s->message->msgAddHdr((char*)eth_add, sizeof(ethHeader));
		s->message->msgFlat(outBuf);

		if (sendto(fd, outBuf, 1024, 0, (struct sockaddr *)&remaddr, slen)==-1) {
			perror("sendto");
			exit(1);
		}
		memset(outBuf, 0, 1024);//清零

	}

}

void* ftp_recv(void* message){
	recv_pipe_unit *r = new recv_pipe_unit;
	int count = 0;    //count the messages that have been received
	for(;;){
		//pthread_mutex_lock(&m);
		//cout << "ftp_recv" << endl;
		//pthread_mutex_unlock(&m);

		/*------read from pipe-----*/

		int result_read = read(ftp_recv_pipe[0], (recv_pipe_unit *)r, sizeof(recv_pipe_unit));
		if (result_read < 0)
			cout << "ftp_recv reading failed" << endl;

		ftpHeader *ftp_strip = new ftpHeader;
		ftp_strip = (ftpHeader *)r->message->msgStripHdr(sizeof(ftpHeader));
		int HLP = ftp_strip->hlpID;
		/*-------print------*/
		char *printBuf = new char[2048];
		r->message->msgFlat(printBuf);
		pthread_mutex_lock(&m);
		cout << "FTP received original message: "<<printBuf<<endl;
		count ++;
		pthread_mutex_unlock(&m);
		if ((strcmp(printBuf, "This is the FTP message 99!") == 0)||(strcmp(printBuf, "FTP MESSAGE 99!") == 0)) {
			struct timeval end;
			gettimeofday(&end, NULL);
			endmili = (end.tv_sec) * 1000 + (end.tv_usec) / 1000;
			printf("start sending at %d, end receiving at %d\n", startmili,
					endmili);
		}
	}
}

void* telnet_recv(void* message){
	recv_pipe_unit *r = new recv_pipe_unit;
	int count = 0;
	for(;;){

		/*------read from pipe-----*/

		int result_read = read(telnet_recv_pipe[0], (recv_pipe_unit *)r, sizeof(recv_pipe_unit));
		if (result_read < 0)
			cout << "telnet_recv reading failed" << endl;

		telnetHeader *telnet_strip = new telnetHeader;
		telnet_strip = (telnetHeader *)r->message->msgStripHdr(sizeof(telnetHeader));
		int HLP = telnet_strip->hlpID;
		/*-------print------*/
		char *printBuf = new char[2048];
		r->message->msgFlat(printBuf);
		pthread_mutex_lock(&m);
		cout << "TELNET received original message: "<< printBuf<< endl;
		count ++;
		pthread_mutex_unlock(&m);
	}
}

void* rdp_recv(void* message){
	recv_pipe_unit *r = new recv_pipe_unit;
	int count = 0;
	for(;;){

		/*------read from pipe-----*/

		int result_read = read(rdp_recv_pipe[0], (recv_pipe_unit *)r, sizeof(recv_pipe_unit));
		if (result_read < 0)
			cout << "rdp_recv reading failed" << endl;

		rdpHeader *rdp_strip = new rdpHeader;
		rdp_strip = (rdpHeader *)r->message->msgStripHdr(sizeof(rdpHeader));
		int HLP = rdp_strip->hlpID;
		/*-------print------*/
		char *printBuf = new char[2048];
		r->message->msgFlat(printBuf);
		pthread_mutex_lock(&m);
		cout << "RDP received original message: "<<printBuf<< endl;
		count ++;
		pthread_mutex_unlock(&m);
	}
}

void* dns_recv(void* message){
	recv_pipe_unit *r = new recv_pipe_unit;
	int count = 0;
	for(;;){
		//pthread_mutex_lock(&m);
		//cout << "dns_recv" << endl;
		//pthread_mutex_unlock(&m);

		/*------read from pipe-----*/

		int result_read = read(dns_recv_pipe[0], (recv_pipe_unit *)r, sizeof(recv_pipe_unit));
		if (result_read < 0)
			cout << "dns_recv reading failed" << endl;

		dnsHeader *dns_strip = new dnsHeader;
		dns_strip = (dnsHeader *)r->message->msgStripHdr(sizeof(dnsHeader));
		int HLP = dns_strip->hlpID;
		/*-------print------*/
		char *printBuf = new char[2048];
		r->message->msgFlat(printBuf);
		pthread_mutex_lock(&m);
		cout << "UDP received original message: "<<printBuf<< endl;
		count ++;
		pthread_mutex_unlock(&m);
	}
}

void* tcp_recv(void* message){
	recv_pipe_unit *r = new recv_pipe_unit;

	for(;;){
		//pthread_mutex_lock(&m);
		//cout << "tcp_recv" << endl;
		//pthread_mutex_unlock(&m);

		/*------read from pipe-----*/

		int result_read = read(tcp_recv_pipe[0], (recv_pipe_unit *)r, sizeof(recv_pipe_unit));
		if (result_read < 0)
			cout << "tcp_recv reading failed" << endl;

		tcpHeader *tcp_strip = new tcpHeader;
		tcp_strip = (tcpHeader *)r->message->msgStripHdr(sizeof(tcpHeader));
		int HLP = tcp_strip->hlpID;
		/*-------write to pipe------*/
		if(HLP == 5){
			int result_write = write(ftp_recv_pipe[1], (char*)r, sizeof(recv_pipe_unit));
			if (result_write < 0)
				cout << "tdp_recv writing failed" << endl;
		}else if(HLP == 6){
			int result_write = write(telnet_recv_pipe[1], (char*)r, sizeof(recv_pipe_unit));
			if (result_write < 0)
				cout << "tcp_recv writing failed" << endl;
		}
	}
}

void* udp_recv(void* message){
	recv_pipe_unit *r = new recv_pipe_unit;

	for(;;){
		//pthread_mutex_lock(&m);
		//cout << "udp_recv" << endl;
		//pthread_mutex_unlock(&m);

		/*------read from pipe-----*/

		int result_read = read(udp_recv_pipe[0], (recv_pipe_unit *)r, sizeof(recv_pipe_unit));
		if (result_read < 0)
			cout << "udp_recv reading failed" << endl;

		udpHeader *udp_strip = new udpHeader;
		udp_strip = (udpHeader *)r->message->msgStripHdr(sizeof(udpHeader));
		int HLP = udp_strip->hlpID;
		/*-------write to pipe------*/
		if(HLP == 7){
			int result_write = write(rdp_recv_pipe[1], (char*)r, sizeof(recv_pipe_unit));
			if (result_write < 0)
				cout << "udp_recv writing failed" << endl;
		}else if(HLP == 8){
			int result_write = write(dns_recv_pipe[1], (char*)r, sizeof(recv_pipe_unit));
			if (result_write < 0)
				cout << "udp_recv writing failed" << endl;
		}
	}
}

void* ip_recv(void* message){
   //!!!!!
	recv_pipe_unit *r = new recv_pipe_unit;

	for(;;){
		//pthread_mutex_lock(&m);
		//cout << "ip_recv" << endl;
		//pthread_mutex_unlock(&m);

		/*------read from pipe-----*/

		int result_read = read(ip_recv_pipe[0], (recv_pipe_unit *)r, sizeof(recv_pipe_unit));
		if (result_read < 0)
			cout << "ip_recv reading failed" << endl;

		ipHeader *ip_strip = new ipHeader;
		ip_strip = (ipHeader *)r->message->msgStripHdr(sizeof(ipHeader));
		int HLP = ip_strip->hlpID;
		/*-------write to pipe------*/
		if(HLP == 3){
			int result_write = write(tcp_recv_pipe[1], (char*)r, sizeof(recv_pipe_unit));
			if (result_write < 0)
				cout << "ip_recv writing failed" << endl;
		}else if(HLP == 4){
			int result_write = write(udp_recv_pipe[1], (char*)r, sizeof(recv_pipe_unit));
			if (result_write < 0)
				cout << "ip_recv writing failed" << endl;
		}
	}
}

void* eth_recv(void* message){
	recv_pipe_unit *r = new recv_pipe_unit;

	for(;;){
		//pthread_mutex_lock(&m);
		//cout << "eth_recv" << endl;
		//pthread_mutex_unlock(&m);

		/*------read from pipe-----*/
		char* buf = new char[2048];
		int result_read = read(eth_recv_pipe[0], buf, 2048);
		if (result_read < 0)
			cout << "eth_recv reading failed" << endl;
		Message *s = new Message(buf,2048);

		ethHeader *eth_strip = new ethHeader;
		eth_strip = (ethHeader *)s->msgStripHdr(sizeof(ethHeader));

		r->message = s;
		/*-------write to pipe------*/
		int result_write = write(ip_recv_pipe[1], (char*)r, sizeof(recv_pipe_unit));
		if (result_write < 0)
			cout << "eth_recv writing failed" << endl;
	}
}

void* ftp_app(void* message){

	char *a = new char[100];
	//a = "This is the FTP message!";

	for(int i=0;i<100 ; i++){
		if (i == 0) {
			struct timeval start;
			gettimeofday(&start, NULL);
			startmili = (start.tv_sec) * 1000 + (start.tv_usec) / 1000;

		}
		sprintf(a, "This is the FTP message %d!", i);
		Message* me = new Message(a, 100);
		//memset(outBuf,0, 1024);
		send_pipe_unit* unit = new send_pipe_unit;
		unit->protocol_id = 0;
		unit->message = me;
		int write_result = write(ftp_send_pipe[1], (char*) unit, sizeof(send_pipe_unit));
		if (write_result == -1) {
			perror("ftp_app write");
			exit(2);
			//cout << "ftp_app writing successfully i :" << i << endl;
		}
		unsigned int x = 200000;
		usleep(x);
	}

}

void* telnet_app(void* message){
	char *a = new char[100];
	//a = "This is the telnet message!";

	for(int i=0; i<100 ; i++){
		sprintf(a, "This is the TELNET message %d!", i);
		Message* me = new Message(a, 100);
		//memset(outBuf,0, 1024);
		send_pipe_unit* unit = new send_pipe_unit;
		unit->protocol_id = 0;
		unit->message = me;
		int write_result = write(telnet_send_pipe[1], (char*) unit, sizeof(send_pipe_unit));
		if (write_result == -1) {
			perror("telnet_app write");
			exit(2);
			//cout << "ftp_app writing successfully i :" << i << endl;
		}
		unsigned int x = 200000;
		usleep(x);
	}
}

void* rdp_app(void* message){
	char *a = new char[100];
	//a = "This is the rdp message!";

	for(int i=0; i<100 ; i++){
		sprintf(a, "This is the RDP message %d!", i);
		Message* me = new Message(a, 100);
		//memset(outBuf,0, 1024);
		send_pipe_unit* unit = new send_pipe_unit;
		unit->protocol_id = 0;
		unit->message = me;
		int write_result = write(rdp_send_pipe[1], (char*) unit, sizeof(send_pipe_unit));
		if (write_result == -1) {
			perror("rdp_app write");
			exit(2);
			//cout << "ftp_app writing successfully i :" << i << endl;
		}
		unsigned int x = 200000;
		usleep(x);
	}
}

void* dns_app(void* message){
	char *a = new char[100];
	//a = "This is the dns message!";

	for(int i=0; i<100 ; i++){
		sprintf(a, "This is the DNS message %d!", i);
		Message* me = new Message(a, 100);
		//memset(outBuf,0, 1024);
		send_pipe_unit* unit = new send_pipe_unit;
		unit->protocol_id = 0;
		unit->message = me;
		int write_result = write(dns_send_pipe[1], (char*) unit, sizeof(send_pipe_unit));
		if (write_result == -1) {
			perror("dns_app write");
			exit(2);
			//cout << "ftp_app writing successfully i :" << i << endl;
		}
		unsigned int x = 200000;
		usleep(x);
	}
}

void* receive(void* message)
{
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
			cout << "receiving successfully" << endl;
			pthread_mutex_unlock(&m);

			//Message* me = new Message(buf_recv,1024);
			//deliver_eth(me);
			//!!!!!!!!!!!!!!!!!!!!!!!!!
			int result_write = write(eth_recv_pipe[1], buf_recv, 2048);
			if(result_write < 0)
				cout << "udp writing failed" << endl;
		}
	}
}

int main() {
	pthread_mutex_init(&m, NULL);
	/*-----create pipes first-----*/
	int result = pipe(ftp_send_pipe);
	int result2 = pipe(telnet_send_pipe);
	int result3 = pipe(rdp_send_pipe);
	int result4 = pipe(dns_send_pipe);
	int result5 = pipe(tcp_send_pipe);
	int result6 = pipe(udp_send_pipe);
	int result7 = pipe(ip_send_pipe);
	int result8 = pipe(eth_send_pipe);

	int result_1 = pipe(ftp_recv_pipe);
	int result_2 = pipe(telnet_recv_pipe);
	int result_3 = pipe(rdp_recv_pipe);
	int result_4 = pipe(dns_recv_pipe);
	int result_5 = pipe(tcp_recv_pipe);
	int result_6 = pipe(udp_recv_pipe);
	int result_7 = pipe(ip_recv_pipe);
	int result_8 = pipe(eth_recv_pipe);

	if ((result < 0) || (result2 < 0) || (result3 < 0) || (result4 < 0)
		|| (result5 < 0) || (result6 < 0) || (result7 < 0) || (result8 < 0)) {
		perror("pipe ");
		exit(1);
	}

	if ((result_1< 0) || (result_2 < 0) || (result_3 < 0) || (result_4 < 0)
		|| (result_5 < 0) || (result_6 < 0) || (result_7 < 0) || (result_8 < 0)) {
		perror("pipe ");
		exit(1);
	}

	/*------establish protocols-----*/
	pthread_t *ftp_send_th;
	pthread_t *telnet_send_th;
	pthread_t *rdp_send_th;
	pthread_t *dns_send_th;
	pthread_t *tcp_send_th;
	pthread_t *udp_send_th;
	pthread_t *ip_send_th;
	pthread_t *eth_send_th;
	pthread_t *ftp_recv_th;
	pthread_t *telnet_recv_th;
	pthread_t *rdp_recv_th;
	pthread_t *dns_recv_th;
	pthread_t *tcp_recv_th;
	pthread_t *udp_recv_th;
	pthread_t *ip_recv_th;
	pthread_t *eth_recv_th;

	ftp_send_th = (pthread_t *) malloc(sizeof(pthread_t));
	telnet_send_th = (pthread_t *) malloc(sizeof(pthread_t));
	rdp_send_th = (pthread_t *) malloc(sizeof(pthread_t));
	dns_send_th = (pthread_t *) malloc(sizeof(pthread_t));
	tcp_send_th = (pthread_t *) malloc(sizeof(pthread_t));
	udp_send_th = (pthread_t *) malloc(sizeof(pthread_t));
	ip_send_th = (pthread_t *) malloc(sizeof(pthread_t));
	eth_send_th = (pthread_t *) malloc(sizeof(pthread_t));

	ftp_recv_th = (pthread_t *) malloc(sizeof(pthread_t));
	telnet_recv_th = (pthread_t *) malloc(sizeof(pthread_t));
	rdp_recv_th = (pthread_t *) malloc(sizeof(pthread_t));
	dns_recv_th = (pthread_t *) malloc(sizeof(pthread_t));
	tcp_recv_th = (pthread_t *) malloc(sizeof(pthread_t));
	udp_recv_th = (pthread_t *) malloc(sizeof(pthread_t));
	ip_recv_th = (pthread_t *) malloc(sizeof(pthread_t));
	eth_recv_th = (pthread_t *) malloc(sizeof(pthread_t));

	if (pthread_create(ftp_send_th, NULL, ftp_send, NULL)) {
		perror("error creating ftp_send_th");
		exit(1);
	}

	if (pthread_create(telnet_send_th, NULL, telnet_send, NULL)) {
		perror("error creating telnet_send_th");
		exit(1);
	}

	if (pthread_create(rdp_send_th, NULL, rdp_send, NULL)) {
		perror("error creating rdp_send_th");
		exit(1);
	}

	if (pthread_create(dns_send_th, NULL, dns_send, NULL)) {
		perror("error creating dns_send_th");
		exit(1);
	}

	if (pthread_create(tcp_send_th, NULL, tcp_send, NULL)) {
		perror("error creating tcp_send_th");
		exit(1);
	}

	if (pthread_create(udp_send_th, NULL, udp_send, NULL)) {
		perror("error creating udp_send_th");
		exit(1);
	}

	if (pthread_create(ip_send_th, NULL, ip_send, NULL)) {
		perror("error creating ip_send_th");
		exit(1);
	}

	if (pthread_create(eth_send_th, NULL, eth_send, NULL)) {
		perror("error creating eth_send_th");
		exit(1);
	}

	if (pthread_create(ftp_recv_th, NULL, ftp_recv, NULL)) {
		perror("error creating ftp_send_th");
		exit(1);
	}

	if (pthread_create(telnet_recv_th, NULL, telnet_recv, NULL)) {
		perror("error creating telnet_send_th");
		exit(1);
	}

	if (pthread_create(rdp_recv_th, NULL, rdp_recv, NULL)) {
		perror("error creating rdp_send_th");
		exit(1);
	}

	if (pthread_create(dns_recv_th, NULL, dns_recv, NULL)) {
		perror("error creating dns_send_th");
		exit(1);
	}

	if (pthread_create(tcp_recv_th, NULL, tcp_recv, NULL)) {
		perror("error creating tcp_send_th");
		exit(1);
	}

	if (pthread_create(udp_recv_th, NULL, udp_recv, NULL)) {
		perror("error creating udp_send_th");
		exit(1);
	}

	if (pthread_create(ip_recv_th, NULL, ip_recv, NULL)) {
		perror("error creating ip_send_th");
		exit(1);
	}

	if (pthread_create(eth_recv_th, NULL, eth_recv, NULL)) {
		perror("error creating eth_send_th");
		exit(1);
	}
	sleep(2); // wait for the establishments of protocols

	/*---------create receive thread--------*/
	pthread_t * recv;
	recv = (pthread_t*)malloc(sizeof(pthread_t));
	if(pthread_create(recv, NULL, receive, NULL)){
		perror("error creating receive thread!");
		exit(1);
	}

	/*---------create a send socket--------*/
	if ((fd=socket(AF_INET, SOCK_DGRAM, 0))==-1)
		cout << "failed creating socket" << endl;
	pthread_mutex_lock(&m);
	cout << "Please input a port numbuer: ";
	pthread_mutex_unlock(&m);
	cin >> portnum;	//input

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


	/*---------establish 4 applications------*/
	pthread_t *ftp;
	pthread_t *telnet;
	pthread_t *rdp;
	pthread_t *dns;
	ftp = (pthread_t *) malloc(sizeof(pthread_t));
	if(pthread_create(ftp, NULL, ftp_app, NULL)){
		perror("error creating ftp thread!");
		exit(1);
	}

	telnet = (pthread_t *) malloc(sizeof(pthread_t));
	if(pthread_create(telnet, NULL, telnet_app, NULL)){
		perror("error creating telnet thread!");
		exit(1);
	}

	rdp = (pthread_t *) malloc(sizeof(pthread_t));
	if(pthread_create(rdp, NULL, rdp_app, NULL)){
		perror("error creating rdp thread!");
		exit(1);
	}

	dns = (pthread_t *) malloc(sizeof(pthread_t));
	if(pthread_create(dns, NULL, dns_app, NULL)){
		perror("error creating dns thread!");
		exit(1);
	}


	pthread_join(*ftp, 0);
	pthread_join(*telnet, 0);
	pthread_join(*rdp, 0);
	pthread_join(*dns, 0);
	pthread_join(*dns, 0);
	
	pthread_join(*ftp_recv_th, 0);
	pthread_join(*telnet_recv_th, 0);
	pthread_join(*rdp_recv_th, 0);
	pthread_join(*dns_recv_th, 0);
	pthread_join(*tcp_recv_th, 0);
	pthread_join(*udp_recv_th, 0);
	pthread_join(*ip_recv_th, 0);
	pthread_join(*eth_recv_th, 0);


	return 0;

}


