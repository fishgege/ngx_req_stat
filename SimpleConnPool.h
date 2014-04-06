/*************************************************
 * Author: jie123108@163.com
 * Copyright: jie123108
 *************************************************/
#ifndef __JSONLIB_CONN_POOL_H__
#define __JSONLIB_CONN_POOL_H__
#include <pthread.h>
#include <stdlib.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"{
#endif

uint64_t uint64_inc(uint64_t* pcount);

typedef void* (*conn_new_cb)(void* args);
typedef int (*conn_connect_cb)(void* conn,void* args);
typedef int (*conn_reconnect_cb)(void* conn,void* args);
typedef int (*conn_close_cb)(void* conn);
typedef int (*conn_test_close_cb)(void* conn); //�����Ƿ���Ҫ�ر�����(�����ӳ���������) 0:����Ҫ�رգ�1:��Ҫ�ر�
typedef void (*conn_free_cb)(void* conn);

typedef struct rs_conn_cb_t{
	conn_new_cb conn_new;
	conn_free_cb conn_free;
	conn_connect_cb connect;
	conn_reconnect_cb reconnect;
	conn_close_cb close;
	conn_test_close_cb test_close;
}rs_conn_cb_t;

typedef struct rs_conn_statis_t{
	uint64_t connect; 	//��������(�������ڼ�����)
	uint64_t close; 	//�ر���������
	uint64_t get;//��ȡ��������
	uint64_t get_real; //�ӳ������ӵ�����
	uint64_t release;//�����ͷ�������
	uint64_t release_real;//�����ͷ���������
}rs_conn_statis_t;

typedef struct rs_conn_pool_t{
	volatile int curconns; 
	void* args; //���Ӳ���
	void** conns; //����
	//char* status; //�Ƿ������ϡ�'y':�Ѿ������ϣ�'n':δ�����ϡ�'0':��ʾ�����ӡ�
	rs_conn_cb_t* cbs; //���ӳ���ػص���
	int size;
	int start; //��start==endʱ���п��������ˣ�Ҳ�����ǿ��ˡ���curconns�жϡ�curconns > 0 ���ˣ�<=0 ���ˡ�
	int end;
	time_t pre_err_time; //�ϴ����ӳ����ʱ�䡣
	int reconn_interval; 	  //����������Сʱ����(��)�� ����������ӳ�����ڸõ�λʱ�䲻�ٽ��������ӡ�
	pthread_mutex_t mutex;
	rs_conn_statis_t statis;
}rs_conn_pool_t;

/**
 * size ���ӳش��С
 * lazy_init ��ʾ��newʱ������ʵ�ʵ�����
 */
rs_conn_pool_t* rs_conn_pool_new(int size,int lazy_init,rs_conn_cb_t* cbs, void* args);
void rs_conn_pool_free(rs_conn_pool_t* pool);
void* rs_conn_pool_get(rs_conn_pool_t* pool);
int rs_conn_pool_put(rs_conn_pool_t* pool, void* conn);
int rs_conn_pool_put2(rs_conn_pool_t* pool, void* conn);


#ifdef __cplusplus
}
#endif

//}
#endif

