
#include<stdio.h>
#include<string.h>
#include"mysql.h"
#include <stdlib.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <arpa/inet.h>  
#include <netdb.h>  
#include<unistd.h>
      
int port=12345;  
      
  
//数据进制
int data_count = 16;

//存储接受的一组数据流
char get_data[255];

//存储转换后的ASCII值
int data_ascII[100];

//指向接受数据流每一字节的指针数组
char *data_pointer[100];

//记录数据流中的地区表示符
char areaflag;

//记录一组数据流中areaID
char areaID;

//记录一组数据流中时间
struct time_of_record{
	int time_value[7];
	char time_str[100];
};

//nodeID, length, recordID共同体
union IDs{
	short ID_vlaue[3];
	unsigned char ID_asc[6];
};

//数据组的共同体
union datas{
	float data_value[9];
	char data_asc[36];
};

//重置data_ascII[]函数
void clear_data_ascII();


//重置data_pointer函数
void clear_data_pointer();

//分割一组数据流每一字节函数
void stock(char *get_data, char flag,  char *data_pointer[]);

//将16进制ASCII码转换为10进制ASCII函数
void num_16_to_10(char *data_pointer[], int data_ascII[]);

//译码函数
void encode(int data_ascII[], IDs *id, time_of_record *tm_of_re, datas *d);

//输出检验函数
void output(IDs *id, time_of_record *tm_of_re, datas *d);

//插入MYSQL数据库1
void insert_to_mysql(IDs *id, time_of_record *tm_of_re, datas *d);

//插入MYSQL数据库2
void insert_to_mysql2(char *ip,int port);
int main(int argc, char** argv)
{
	/*
	datas d;
	d.data_asc[0] = 0;
	d.data_asc[1] = 192;
	d.data_asc[2] = 191;
	d.data_asc[3] = 63;
	d.data_asc[4] = 0;
	d.data_asc[5] = 72;
	d.data_asc[6] = 191;
	d.data_asc[7] = 63;
	printf("%f\n%f\n",d.data_value[0], d.data_value[2]);
	

	IDs i;
	int index = 0;
	time_of_record tm;
	tm.time_value[0] = 27;
	sprintf(tm.time_str, "%d ", tm.time_value[0]);
	strcat(tm.time_str, "\0");
	printf("%s\n",tm.time_str);
	*/
	socklen_t sin_len;  
        char message[2560];  
      
        int socket_descriptor;  
        struct sockaddr_in sin;  
        printf("Waiting for data form sender \n");  
      
        bzero(&sin,sizeof(sin));  
        sin.sin_family=AF_INET;  
        sin.sin_addr.s_addr=htonl(INADDR_ANY);  
        sin.sin_port=htons(port);  
        sin_len=sizeof(sin);  
      
        socket_descriptor=socket(AF_INET,SOCK_DGRAM,0);  
        bind(socket_descriptor,(struct sockaddr *)&sin,sizeof(sin));  
      	char flag[30]="get it!";
        while(1)  
        {  
            recvfrom(socket_descriptor,message,sizeof(message),0,(struct sockaddr *)&sin,&sin_len);  
            printf("Response from server:%s\n",flag);
            char c = 2;
	    time_of_record *tm_of_re = new time_of_record;
	    IDs *id = new IDs;
	datas *d = new datas;
	//printf("%s\n",message);	
	strcpy(get_data,message /*"40 32 30 31 45 00 01 00 27 27 10 1d 04 01 0c 00 5C C1 3F 00 C0 BF 3F 00 C8 BE 3F 00 DB BF 3F 00 E0 BF 3F 00 72 C1 3F 00 32 C1 3F 00 48 BF 3F 00 00 80 BD 32 46 46 46 46 46 46 46 46 0D 0A\0"*/);
	stock(get_data, ' ', data_pointer);
	num_16_to_10(data_pointer, data_ascII);
	encode(data_ascII, id, tm_of_re, d);
	//printf("test\n");
	//printf("c:%c\n", id->ID_asc[4]);
	//char str[INET_ADDRSTRLEN];  
	//const char *ptr =new char(100);
	char str[INET_ADDRSTRLEN];
	inet_ntop(AF_INET,&sin.sin_addr, str, INET_ADDRSTRLEN);
	insert_to_mysql2(str,sin.sin_port);
        insert_to_mysql(id, tm_of_re, d);
        output(id, tm_of_re, d);  
            if(strncmp(message,"stop",4) == 0)//接受到的消息为 “stop”  
            {  
      
                printf("Sender has told me to end the connection\n");  
                break;  
            }  
        }  
      
        close(socket_descriptor);  
        exit(0);  
      
        return (EXIT_SUCCESS);
        } 
/*	char c = 2;
	time_of_record *tm_of_re = new time_of_record;
	IDs *id = new IDs;
	datas *d = new datas;
	strcpy(get_data,40 32 30 31 45 00 01 00 27 27 10 1d 04 01 0c 00 5C C1 3F 00 C0 BF 3F 00 C8 BE 3F 00 DB BF 3F 00 E0 BF 3F 00 72 C1 3F 00 32 C1 3F 00 48 BF 3F 00 00 80 BD 32 46 46 46 46 46 46 46 46 0D 0A\0");
	stock(get_data, ' ', data_pointer);
	num_16_to_10(data_pointer, data_ascII);
	encode(data_ascII, id, tm_of_re, d);
	//printf("c:%c\n", id->ID_asc[4]);
        insert_to_mysql(id, tm_of_re, d);
        output(id, tm_of_re, d);
	return 0;
}*/

void clear_data_ascII()
{
	for(int i = 0; i < 100; i++)
		data_ascII[i] = -1;
	return;
}

void clear_data_pointer()
{
	for(int i = 0; i < 100; i++)
		data_pointer[i] = NULL;
	return;
}

void stock(char *get_data, char flag,  char *data_pointer[])
{
	clear_data_pointer();
	int i = 0, j = 1, flag_count = 0;
	int index[100] = {0};
      char *data_first = get_data;
	while(*get_data)
	{
		if(*get_data == flag)
		{
			index[++j] = i;
			flag_count ++;
			*get_data = '\0';
		}
		if(*get_data == ';')
			*get_data = '\0';
		get_data++;
		i++;
	}
	data_pointer[1] = data_first;
	for(int count = 2; count <= flag_count + 1; count++)
	{
		data_pointer[count] = data_first + index[count] + 1;
	}
	return;
}

void num_16_to_10(char *data_pointer[], int data_ascII[])
{
	clear_data_ascII();
	for(int i = 1; data_pointer[i] != NULL; i++)
	{
		int num;
		char *index = data_pointer[i];
		data_ascII[i] = 0;
		if(i < 9 || i > 15)
		{
			while(*index)
			{
				if(*index >= '0' && *index <= '9')
					num = *index - '0';
				if(*index >= 'a' && *index <= 'z')
					num = *index - 'a' + 10;
				if(*index >= 'A' && *index <= 'Z')
					num = *index - 'A' + 10;
				data_ascII[i] = data_ascII[i] * data_count + num;
				index ++;
			}
		}
		else
		{
			while(*index)
			{
				if(*index >= '0' && *index <= '9')
					num = *index - '0';
				if(*index >= 'a' && *index <= 'z')
					num = *index - 'a' + 10;
				if(*index >= 'A' && *index <= 'Z')
					num = *index - 'A' + 10;
				data_ascII[i] = data_ascII[i] * 10 + num;
				index ++;
			}
		}
			
	}
	return;
}

void encode(int data_ascII[], IDs *id, time_of_record *tm_of_re, datas *d)
{
	for(int i = 1; data_ascII[i] != -1; i++)
	{
		if(i == 1)
			areaflag = data_ascII[i];
		if(i == 2)
			areaID = data_ascII[i];
		if(i >= 3 && i <= 8)
		{
			id->ID_asc[i - 3] = data_ascII[i];
		}
		if(i >= 9 && i <= 15)
		{
			tm_of_re->time_value[i - 9] = data_ascII[i];
		}
		if( i >= 16 && i <= 51)
		{
			d->data_asc[i - 16] = data_ascII[i];
		}
		if(i >= 52)
		{
			if(tm_of_re->time_value[6]>=10)
			sprintf(tm_of_re->time_str, "20%d-%d-%d %d-%d-%d,%d", tm_of_re->time_value[6], tm_of_re->time_value[4], tm_of_re->time_value[3], tm_of_re->time_value[2], tm_of_re->time_value[1], tm_of_re->time_value[0], tm_of_re->time_value[5]);
			else
			sprintf(tm_of_re->time_str, "200%d-%d-%d %d-%d-%d,%d", tm_of_re->time_value[6], tm_of_re->time_value[4], tm_of_re->time_value[3], tm_of_re->time_value[2], tm_of_re->time_value[1], tm_of_re->time_value[0], tm_of_re->time_value[5]);			
			printf("encode sucessed!\n");
			break;
		}
	}
	return;
}

void output(IDs *id, time_of_record *tm_of_re, datas *d)
{
	
	printf("areaflag:%c\n", areaflag);
	printf("areaID:%c\n", areaID);
	printf("nodeID:%d\n", id->ID_vlaue[0]);
	printf("length:%d\n", id->ID_vlaue[1]);
	printf("recordID:%d\n", id->ID_vlaue[2]);
	printf("time:%s\n", tm_of_re->time_str);
	printf("data1:%f\n",d->data_value[0]);
	printf("data2:%f\n",d->data_value[1]);
	printf("data3:%f\n",d->data_value[2]);
	printf("data4:%f\n",d->data_value[3]);
	printf("data5:%f\n",d->data_value[4]);
	printf("data6:%f\n",d->data_value[5]);
	printf("data7:%f\n",d->data_value[6]);
	printf("data8:%f\n",d->data_value[7]);
	printf("tempreture:%f\n",d->data_value[8]);
	return;
}
void insert_to_mysql(IDs *id, time_of_record *tm_of_re, datas *d)
{
    int i;
    int res;
    char sql_command[1000] = "";
    char record_time[200] = "";
    strcpy(record_time, tm_of_re->time_str);
    stock(record_time, ',', data_pointer);
    MYSQL *my_connection;
    my_connection = mysql_init(NULL);
    if (!my_connection) {
        fprintf(stderr, "mysql_init failed\n");
        return ;
    }
    //if(mysql_real_connect(my_connection, "localhost", "n152414", "Zn103467", "n152414", 0, NULL, 0))
    if(mysql_real_connect(my_connection,"localhost","root","03005B72..","test",0,NULL,0))
    for(i = 0; i < 9; i++)
    {
    if(i<8)
    {
      char type[10] ="";
      char num[2]="";
      num[0]='1' + i;
     // printf("%c",num[0]);
      strcpy(type, "data");
      strcat(type, num);
      sprintf(sql_command, "insert into data(area,node,r_id,data,type,time,week) values('%c',%d,%d,%f,'%s','%s',%d);", areaID, id->ID_vlaue[0], id->ID_vlaue[2],d->data_value[i],type,data_pointer[1],tm_of_re->time_value[5]);
    // printf("%s",sql_command);
 res = mysql_query(my_connection, sql_command);
    }
   if(i==8)
    { 
      char type[20]="";
      strcpy(type,"temperature");
      sprintf(sql_command, "insert into data(area,node,r_id,data,type,time,week) values('%c',%d,%d,%f,'%s','%s',%d);", areaID, id->ID_vlaue[0], id->ID_vlaue[2],d->data_value[i],type,data_pointer[1],tm_of_re->time_value[5]);
     // printf("%s\n",sql_command);
      res = mysql_query(my_connection, sql_command);
    }   
 }        
    mysql_close(my_connection);
    return;
}
void insert_to_mysql2(char *ip,int port)
{
    int i;
    int res;
    char sql_command[300] = ""; 
    MYSQL *my_connection;
    my_connection = mysql_init(NULL);
    if (!my_connection) {
        fprintf(stderr, "mysql_init failed\n");
        return ;
    }
    //if(mysql_real_connect(my_connection, "localhost", "n152414", "Zn103467", "n152414", 3, NULL, 0))
    if(mysql_real_connect(my_connection,"localhost","root","03005B72..","test",0,NULL,0))
    if(1){
    sprintf(sql_command, "insert into Host(IP,port) values('%s','%d');",ip,port);
    res = mysql_query(my_connection, sql_command);
    }
    mysql_close(my_connection);
    return;
}
