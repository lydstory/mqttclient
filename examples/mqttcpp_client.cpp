#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef WIN32
#include <unistd.h>
#else
#include <process.h>
#include <winsock2.h>
#define snprintf sprintf_s
#endif
#include <iostream>
#include <session.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>


#include <fcntl.h>

#include <sys/stat.h>
#include <ctype.h>
#include <fstream>

#include <vector>
#include <list>
#include <sstream>

#include <time.h>
#include <iomanip>
#include<json/json.h>
#include <json/reader.h>
#include <json/writer.h>
using namespace std;
//#define MQTT_TESTSERVER
//E1:99:4B:CB:7D:30

#define IPADDRESS "117.78.46.57"
#define DEFAULT_PORT 9099


int c2i(char ch)
{

        if(isdigit(ch))
                return ch - 48;

        if( ch < 'A' || (ch > 'F' && ch < 'a') || ch > 'z' )
                return -1;

        if(isalpha(ch))
                return isupper(ch) ? ch - 55 : ch - 87;

        return -1;
}

int hex2dec(char *hex)
{
        int len;
        int num = 0;
        int temp;
        int bits;
        int i;

        len = strlen(hex);

        for (i=0, temp=0; i<len; i++, temp=0)
        {

                temp = c2i( *(hex + i) );

                bits = (len - i - 1) * 4;
                temp = temp << bits;


                num = num | temp;
        }


        return num;
}

int DistillWordsByString(vector<string>& vecWord, const string sStr, const string strSymbol)
{
	vecWord.clear();

	char cpValue[10240];
	int iSymbolLen=strSymbol.length();
	int iPos;
	char* cpPos;
	char cpLine[102400];
	int iTmpLength;
	strcpy(cpLine, sStr.c_str());
	cpLine[sStr.length()] = 0x0;
	while((cpPos = (char*)strstr(cpLine, strSymbol.c_str())) != NULL){
		iTmpLength = strlen(cpLine);
		iPos = cpPos - cpLine;
		strncpy(cpValue, cpLine, iPos);
       	cpValue[iPos] = 0x0;
       	vecWord.push_back(cpValue);
       	strcpy(cpLine, cpPos+iSymbolLen);
		cpLine[iTmpLength-iPos-1]=0x0;
	}
	vecWord.push_back(cpLine);
	return vecWord.size();
}

void* runner(void *param)
{
//E1:99:4B:CB:7D:30
   char* pmac = (char*)param;
   std::cout<<"pm"<<pmac<<std::endl;


   const char* cmd ="gatttool -i hci0 -t random -b %s -a 0x0f -n 0100 --listen --char-write-req";

   const char* cmd2="gatttool -i hci1 -t random -b  %s -a 0x15 -n 0100 --listen --char-write-req";
   char syscmd[200];
   if(strcmp(pmac,"E1:99:4B:CB:7D:30") == 0)
   sprintf(syscmd,cmd,pmac);
   if(strcmp(pmac,"CC:24:FB:17:2C:4B") == 0)
   sprintf(syscmd,cmd2,pmac);
   std::cout<< "syscmd is "<<syscmd<<std::endl;
    char cpTmp[100];
	string strOldContent="";
	vector<string> vecWord;
	vector<string>::iterator itervecWord;
	char buf[120];
	char sendbuf[120];
    FILE* fp = popen(syscmd,"r");
    int connfd = 0;
	int cLen = 0;
	struct sockaddr_in client;

	client.sin_family = AF_INET;
	client.sin_port = htons(DEFAULT_PORT);
	client.sin_addr.s_addr = inet_addr(IPADDRESS);
	connfd = socket(AF_INET, SOCK_STREAM, 0);
	if(connfd<0)
	{
		perror("socket");

	}
	if(connect(connfd, (struct sockaddr*)&client, sizeof(client))<0)
	{
		perror("connect");

	}


	if(fp != NULL)
		{
			memset(buf,0,120);

			while(fgets(buf,120,fp) != NULL)
			{	memset(sendbuf,0,120);
				 vecWord.clear();
		 					strOldContent.assign(buf);
							char *begin=NULL;
							char *tmp=NULL;
							char *end=NULL;
						     int heatrate = 0;
							 int rr1 = 0;
							 int rr2 = 0;
							begin=(char*)strchr(buf,' ');
							end=(char*)strchr(buf,' ');
							while((tmp=strchr(end+1,' ')) != NULL){
								end=tmp;



								string strNewContent=strOldContent.substr(begin-buf+1,end-begin-1);

											DistillWordsByString(vecWord,strNewContent," ");

											}
	                          int i = 0;
							for(itervecWord=vecWord.begin();itervecWord!=vecWord.end();itervecWord++){
	                          i++;
	                         if(i == 6)
	                         {
	                              char ch[10] = {0};
	                              strcpy(ch, (*itervecWord).c_str());

	                              heatrate =hex2dec(ch);




	                         }

	                         if(i == 7)
	                         {
	                        	 char chr0[10] ={0};
	                        	 char chrr[10] = {0};

	                        	                        	 strcpy(chr0,(*itervecWord).c_str());

	                        	                        	 *itervecWord++;
	                        	                        	 i++;
	                        	                        	 strcpy(chrr,(*itervecWord).c_str());
	                        	                        	 strcat(chrr,chr0);


	                        	                        	                              rr1 =hex2dec(chrr);


	                         }

	                         if(i == 9)
	                         {
	                        	 char chr0[10] ={0};
	                        	                         	 char chrr[10] = {0};

	                        	                         	                        	 strcpy(chr0,(*itervecWord).c_str());

	                        	                         	                        	 *itervecWord++;
	                        	                         	                        	 strcpy(chrr,(*itervecWord).c_str());
	                        	                         	                        	 strcat(chrr,chr0);


	                        	                         	                        	                              rr2 =hex2dec(chrr);

	                         }






	              }
						//break;
							Json::Value   root;
                            root["mac"] = pmac;
                            root["HEATRATE"] = heatrate;
                            Json::Value RR;
                            RR["RR1"] = rr1;
                            RR["RR2"] = rr2;
                            root["RR"] = RR;

                            std::string Mes = root.toStyledString();

							strcpy(sendbuf,Mes.c_str());
							strcat(sendbuf,"\r\n");
							send(connfd, sendbuf, 120, 0);

		}


		}


pclose(fp);
close(connfd);
}
void* runner2(void *param)
{
	char sendbuf[120];

    int connfd = 0;

	struct sockaddr_in client;

	client.sin_family = AF_INET;
	client.sin_port = htons(DEFAULT_PORT);
	client.sin_addr.s_addr = inet_addr(IPADDRESS);
	connfd = socket(AF_INET, SOCK_STREAM, 0);
	if(connfd<0)
	{
		perror("socket");

	}
	if(connect(connfd, (struct sockaddr*)&client, sizeof(client))<0)
	{
		perror("connect");

	}
	Json::Value   root;
	                            root["mac"] = "00:00:00:00:00:00";
	                            root["HEATRATE"] = 100;
	                            Json::Value RR;
	                            RR["RR1"] = 11;
	                            RR["RR2"] = 22;
	                            root["RR"] = RR;

	                            std::string Mes = root.toStyledString();

								strcpy(sendbuf,Mes.c_str());
								strcat(sendbuf,"\r\n");
								while(1)
								{
									sleep(1000);
								send(connfd, sendbuf, 120, 0);
								}

}
class SimpleClient
{
public:
    SimpleClient(const std::string &client_id)
        : client_id_(client_id)
    {
    	std::cout<<"gouzao"<<std::endl;

        session_.set_connect_handler([&](int rc) -> void {

           if (rc == 0) {
               printf("Connected OK\n");
               mqttcpp::Session::Result result = session_.subscribe(client_id_ + "/#");
               printf("Subscribing to %s (mid=%d)...\n",  client_id_.data(), std::get<0>(result));
           }
        });

        session_.set_disconnect_handler([&](int rc) {
           printf("Disconnected\n");
           cond_var_.notify_one();
        });

        session_.set_message_handler([&](int mid, std::string topic, std::string message) {
            std::size_t id_pos = topic.find_last_of("/") + 1;
            std::string from = topic.substr(id_pos, topic.size() - id_pos);

            std::cout<<message<<std::endl;
        //	string test="{\"id\":1,\"name\":\"kurama\"}";
        	Json::Reader  reader;
        	Json::Value   value;

        	if(reader.parse(message,value))
        	{
        	if(!value["applicantid"].isNull())
        	{
        //	cout<<value["id"].asInt()<<endl;
        //	cout<<value["name"].asString()<<endl;
        	std::string macstr = value["applicantid"].asString();
            pthread_t tid1,tid2;

            char *message = new char[macstr.size() + 1];
            strcpy(message,macstr.c_str());

        	std::cout<<macstr<<std::endl;
        	pthread_create(&tid1,NULL,runner,message);

        //	pthread_create(&tid2,NULL,runner2,NULL);
        	}
        	}



            std::cout<<"message"<<std::endl;
          //  printf("[%s] %s\n", from.data(), message.data());
        });

        session_.set_subscribe_handler([&](int mid, int qos_count) {
            printf("Subscribed (mid=%d, qos_count=%d)\n", mid, qos_count);
        });
    }


    ~SimpleClient()
    {
        session_.disconnect();
        {   //wait for disconnect result
            std::unique_lock<std::mutex> lk(mutex_);
            cond_var_.wait(lk);
        }
        session_.stop();
    }

    std::string id() const { return client_id_; }

    mqttcpp::Session::Result publish(const std::string topic, const std::string &message)
    {
        return session_.publish(topic, message);
    }

    mqttcpp::Session::Result connect()
    {
    	std::cout<<"connect"<<std::endl;

#ifdef MQTT_TESTSERVER
       mqttcpp::Session::Result result = session_.connect("beewifi","Win2008!");
#else
   	mqttcpp::Session::Result result = session_.connect("gqQ4HdRAKIYXQtr5","uwbnyWdPp4l8SIs6LN4AhSZnbUQ=");
#endif


        result = session_.start();

        std::cout<<"start"<<std::endl;
        return result;
    }

private:
    mqttcpp::Session session_;
    std::string client_id_;
    std::mutex mutex_;
    std::condition_variable cond_var_;
};

int main(int argc, char *argv[])
{

/*
    if (argc != 3) {
        printf("Usage: %s <your_id> <friend_id>\n", argv[0]);
        return -1;
    }
    */

//test
  /*
    char cmd[128];
    InfoCollector partcmd;
    sprintf(cmd,"reboot");
    partcmd.execCmd(cmd);
 */


#ifdef MQTT_TESTSERVER
 	SimpleClient client("test");
#else
 	SimpleClient client("zerogame_push/topic");
#endif
    mqttcpp::Session::Result result = client.connect();


    if (std::get<0>(result) == mqttcpp::Session::INVALID) {
        printf("Connection failed\n");
        return -1;
    }

     std::cout<<"friend"<<std::endl;
    std::string friend_id(argv[2]);

    for (std::string message; std::getline(std::cin, message);) {
    	std::cout<<message<<std::endl;
      if (message == "exit")
            break;
      // client.publish(friend_id + "/" + client.id(), message);

#ifdef MQTT_TESTSERVER
      client.publish("test", message);
#else
      client.publish("zerogame_push/topic", message);
#endif

        std::cout<<message<<std::endl;
    }
}
