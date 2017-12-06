#include<list>
#include<iostream>
#include<cstdio>
#include<iterator>
using namespace std;

typedef struct
{
	char * msg_content;
	size_t msglen;
}node;

typedef list<node> MES;

class Message
{
public: 
	Message();
	Message(char *msg, size_t len);
	~Message();
	void msgAddHdr(char *hdr, size_t length); 	
	char* msgStripHdr(int len);
	int msgSplit(Message& secondMsg, size_t len);
	void msgJoin(Message& secondMsg);
	size_t msgLen();
	void msgFlat(char *buffer);

	
private:
	
	node node1, node2, node3;
	MES message;//a list
	MES::iterator p; // iterator for backup	
	MES::iterator i; //msgFlat
	MES::iterator x; 
	MES::iterator i_copy;  // restore iterator

	size_t flength;
	char *buffer2;
	char buffer[];
};

Message::Message()
{
	flength = 0;

	node1.msg_content = NULL;
	node1.msglen = 0;
}

Message::Message(char* msg, size_t len)
{
	
	node1.msg_content = msg;
	node1.msglen = len;
	//flength = len;
	message.push_front(node1);
}

Message::~Message()
{
	
}

void Message::msgAddHdr(char *hdr, size_t length)
{
	node2.msg_content = hdr;
	node2.msglen = length;
	message.push_front(node2);
	//flength = flength + node2.msglen;
}

char* Message::msgStripHdr(int len)
{
	int offset;
	size_t flag_copy;  // restore flag;	
	size_t all;	
	size_t length[4096];
	size_t flag = 1;
	char* buffer3 = new char[4096];
	length[0] = 0;
	for(x=message.begin(); x!=message.end(); ++x)
	{
		if(x == message.begin())
		{			
						
			length[flag++] = (*x).msglen;
			all = (*x).msglen;
		}
		else
		{	
			
			all = all + (*x).msglen;
			length[flag++] = all;
		}

		
		for(size_t t = 0; t < flag; t++)
		{
			if((len > length[t]) && (len <= length[t+1]))
			{	
				/*for(int j = 0; j < flag; j++)
				message.pop_front();*/
				
				offset = len - length[t];
				//(*x).msg_content = (*x).msg_content + offset;

				if(x == message.begin())
				  memcpy(buffer3, (*x).msg_content , offset);
				else  
				{
					for(i = message.begin(); i!= x; ++i)
					{
						if(i == message.begin())
						{	
							memcpy(buffer3, (*i).msg_content, (*i).msglen);
			
						}
						else
						{			
							p = i;
							p--;
							buffer3 = buffer3 + (*p).msglen;
							if(i == x)
							memcpy(buffer3, (*i).msg_content, offset);
							else
							memcpy(buffer3, (*i).msg_content, (*i).msglen);
						}	
					}
				}
				
				i_copy = x;
				flag_copy = flag;
				
			}
		}
			//
	}
	
	for(int j = 0; j < flag_copy - 2; j++)
	message.pop_front();
	
	(*i_copy).msg_content = (*i_copy).msg_content + offset;
	(*i_copy).msglen = (*i_copy).msglen - offset;
	return buffer3;
}

int Message::msgSplit(Message& secondMsg, size_t len)
{
	int offset;
	size_t flag_copy;  // restore flag;	
	size_t all;	
	size_t length[4096];
	size_t flag = 1;
	char* buffer3 = new char[4096];
	length[0] = 0;
	
	for(x=message.begin(); x!=message.end(); ++x)
	{
		if(x == message.begin())
		{			
						
			length[flag++] = (*x).msglen;
			all = (*x).msglen;
		}
		else
		{	
			
			all = all + (*x).msglen;
			length[flag++] = all;
		}

		
		for(size_t t = 0; t < flag; t++)
		{
			if((len > length[t]) && (len <= length[t+1]))
			{	
				offset = len - length[t];
				/*----------secondMsg setup-----*/
				(*x).msg_content = (*x).msg_content + offset;//reset the pointer
				(*x).msglen = (*x).msglen -  offset;// reset the length
				for(i = x; i != message.end(); ++i)
				{
                   		secondMsg.message.push_back(*i);
				//secondMsg.flength = secondMsg.flength + (*i).msglen;
				}
				/*-----------OriginalMsg setup-------*/
                		//(*x).msg_content ++;
				//(*x).msg_content = '\0';
			
				(*x).msg_content = (*x).msg_content - offset;
                		(*x).msglen = offset;

				i_copy = x;
				flag_copy = flag;
				
			}
		}
			//
	}

		
		/*flength = 0;
		i_copy ++;
		for( p = message.begin(); p != i_copy; ++p)
		{
			
			flength = flength + (*p).msglen;
		}
		i_copy --;*/
		if(i_copy != --message.end())		
		{
			
			message.erase(i_copy,message.end());
		}
	
}

void Message::msgJoin(Message& secondMsg)
{
	for(i = secondMsg.message.begin(); i != secondMsg.message.end(); ++i)	
	{
		message.push_back(*i);
		(*i).msg_content = NULL;
		(*i).msglen = 0;
	}


}

size_t Message::msgLen()
{
	flength = 0;
	for(i = message.begin(); i != message.end(); ++i)
	{
		flength = flength + (*i).msglen;
	}

	return flength;

}

void Message::msgFlat(char *buffer)
{
	
	//buffer2 = buffer;	
	for(i=message.begin();i!=message.end();++i)
	{
		if(i == message.begin())
		{	
			memcpy(buffer, (*i).msg_content, (*i).msglen);
			
		}
		else
		{			
			p = i;
			p--;
			buffer = buffer + (*p).msglen;
			memcpy(buffer, (*i).msg_content, (*i).msglen);
		}
	}
	
}


