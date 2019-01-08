#include <stdio.h>  
#include <stdlib.h>  
#include<unistd.h>  
#include <string.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <arpa/inet.h>  
#include <netdb.h>  


#include<time.h>

int port=12345;  
//
char new_data[255];

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
	short ID_value[3];
	unsigned char ID_asc[6];
};

//数据组的共同体
union datas{
	float data_value[9];
	char data_asc[36];
};

int rand_num(int max);

float rand_data();

void create_new_data(time_of_record *t, IDs *id, datas *d);

void create_time(time_of_record *t);

void create_ID(IDs *id);

void create_data(datas *d);

//重置data_ascII[]函数
void clear_data_ascII();

//重置data_pointer函数
void clear_data_pointer();

//分割一组数据流每一字节函数
void stock(char *get_data, char flag,  char *data_pointer[]);

//将16进制ASCII码转换为10进制ASCII函数
void num_16_to_10(char *data_pointer[], int data_ascII[]);

//译码函数
void encode(int data_ascII[], IDs *id, time_of_record *t, datas *d);

//输出检验函数
void output(IDs *id, time_of_record *t, datas *d);
  

int main(int argc, char** argv) {  
    srand(time(0));
    time_of_record *t = new time_of_record;
    IDs *id = new IDs;
    datas *d = new datas;
    create_new_data(t, id, d);
    strcpy(get_data, new_data);
    stock(get_data, ' ', data_pointer);
    num_16_to_10(data_pointer, data_ascII);
    encode(data_ascII, id, t, d);
    //printf("c:%c\n", id->ID_asc[4]);
    output(id, t, d);
    int socket_descriptor; //套接口描述字  
    char buf[200];  
    struct sockaddr_in address;//处理网络通信的地址  
    bzero(&address,sizeof(address));  
    address.sin_family=AF_INET;  
    address.sin_addr.s_addr=inet_addr("127.0.0.1");//这里不一样  
    address.sin_port=htons(port);  
    //创建一个 UDP socket  
    socket_descriptor=socket(AF_INET,SOCK_DGRAM,0);//IPV4  SOCK_DGRAM 数据报套接字（UDP协议）  
    /* 
         * sprintf(s, "%8d%8d", 123, 4567); //产生：" 123 4567"  
         * 将格式化后到 字符串存放到s当中 
         */  
        sprintf(buf,"%s",new_data);  
        printf("%s\n",new_data); 
        /*int PASCAL FAR sendto( SOCKET s, const char FAR* buf, int len, int flags,const struct sockaddr FAR* to, int tolen);　　 
         * s：一个标识套接口的描述字。　 
         * buf：包含待发送数据的缓冲区。　　 
         * len：buf缓冲区中数据的长度。　 
         * flags：调用方式标志位。　　 
         * to：（可选）指针，指向目的套接口的地址。　 
         * tolen：to所指地址的长度。   
　　      */  
        sendto(socket_descriptor,buf,sizeof(buf),0,(struct sockaddr *)&address,sizeof(address));   
  
    //sprintf(buf,"stop\n");  
    //sendto(socket_descriptor,buf,sizeof(buf),0,(struct sockaddr *)&address,sizeof(address));//发送stop 命令  
    close(socket_descriptor);  
    //printf("Messages Sent,terminating\n");  
  
    exit(0);  
  
    return (EXIT_SUCCESS);  
}  

int rand_num(int max)
{
    int i;
    i = (rand() % max);
    return i;
}

float rand_data()
{
    float i;
    i = (rand() % 10001 * 1.0 / 10000);
    return i;
}

void create_new_data(time_of_record *t, IDs *id, datas *d)
{
    char temp[3];
    temp[0] = ' ';
    temp[3] = '\0';
    strcpy(new_data, "40"); //@
    char area='0'+rand_num(2);
    int ascII=area;
    temp[1] = '0' + ascII/16; //areaID
    temp[2] = '0'+ascII%16;
    strcat(new_data, temp);
    create_ID(id);
    for(int i = 0; i < 6; i++)
    {
        int asc = id->ID_asc[i];
        temp[1] = '0' + (asc / 16);
        if(asc % 16 >= 0 && asc % 16 <= 9)
        {
            temp[2] = '0' + (asc % 16);
        }
        else
        {
            temp[2] = 'a' + (asc % 16 - 10);
        }
        strcat(new_data, temp);
    }
    create_time(t);
    for(int i = 0; i < 7; i++)
    {
        if(i >= 0 && i <= 2)
        {
            temp[1] = '0' + (t->time_value[i] / 10);
            temp[2] = '0' + (t->time_value[i] % 10);
        }
        else
        {
            temp[1] = '0' + (t->time_value[i] / 10);
	    int year=t->time_value[i]-(t->time_value[i]/10)*10;
            if(year % 16 >= 0 && year % 16 <= 9)
                temp[2] = '0' + year % 16;
            else
                temp[2] = 'a' + (year % 16 -10);
        }
        strcat(new_data, temp);
    }
    create_data(d);
    for(int i = 0; i < 36; i++)
    {
        int asc = d->data_asc[i];
        temp[1] = '0' + (asc / 16);
        if(asc % 16 >= 0 && asc % 16 <= 9)
        {
            temp[2] = '0' + (asc % 16);
        }
        else
        {
            temp[2] = 'A' + (asc % 16 - 10);
        }
        strcat(new_data, temp); 
    }
    strcat(new_data, " 32 46 46 46 46 46 46 46 46 OD OA\0");
    //printf("%s\n", new_data);
    return;
}
void create_time(time_of_record *t)
{
    t->time_value[0] = rand_num(60);
    t->time_value[1] = rand_num(60);
    t->time_value[2] = rand_num(24);
    t->time_value[4] = rand_num(12) + 1;
    if(t->time_value[4] == 2)
    	t->time_value[3] = rand_num(28) + 1;
    else
        t->time_value[3] = rand_num(30) + 1;
    t->time_value[5] = rand_num(7) + 1;
    t->time_value[6] = 17;
    return;
}
void create_ID(IDs *id)
{
    id->ID_value[0] = 1+rand_num(3);
    id->ID_value[1] = 46;
    id->ID_value[2] = rand_num(100) + 1;
    return; 
}
void create_data(datas *d)
{
    for(int i = 0; i < 9; i++)
        d->data_value[i] = rand_data();
    return;   
}
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

void encode(int data_ascII[], IDs *id, time_of_record *t, datas *d)
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
			t->time_value[i - 9] = data_ascII[i];
		}
		if( i >= 16 && i <= 51)
		{
			d->data_asc[i - 16] = data_ascII[i];
		}
		if(i >= 52)
		{
			if(t->time_value[6]>=10)
			sprintf(t->time_str, "%d秒 %d分 %d点 %d日 %d月 星期%d 20%d", t->time_value[0], t->time_value[1], t->time_value[2], t->time_value[3], t->time_value[4], t->time_value[5], t->time_value[6]);
			else
			sprintf(t->time_str, "%d秒 %d分 %d点 %d日 %d月 星期%d 200%d", t->time_value[0], t->time_value[1], t->time_value[2], t->time_value[3], t->time_value[4], t->time_value[5], t->time_value[6]);
			printf("encode sucessed!\n");
			break;
		}
	}
	return;
}
void output(IDs *id, time_of_record *t, datas *d)
{
	
	printf("areaflag:%c\n", areaflag);
	printf("areaID:%c\n", areaID);
	printf("nodeID:%d\n", id->ID_value[0]);
	printf("length:%d\n", id->ID_value[1]);
	printf("recordID:%d\n", id->ID_value[2]);
	printf("time:%s\n", t->time_str);
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
