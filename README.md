#nginx ͳ��ģ�顣#  
  ngx_req_stat��һ��nginx״̬ͳ��ģ�飬��ͳ�����ǿ����õģ����ҿ���ͳ�Ʋ�ͬ��������������ͬ��URL������ͳ�Ƶİ����������������״̬��Ĵ�������ͬ��ʱ��εĴ���������������ۼ���Ϣ��ƽ������ʱ��ȵȡ�

ͳ����Ϣ���մ洢��mongodb�С�ʹ��mongodb����ͳ����Ϣ�洢������Ϊmongodb֧������Ĳ�����䣺
```bash
	db.stat_table.update({'date':'2014-04-04', 'url':'/test01'}, {'$inc':{'count':1,'req_time.all':0.005,'status.200': 1}}, true)
	#������䷴��ִ�У����ݿ��������Ӧ���ۼӼ�¼�������������(�Ե���ID�ֶ�)��
	{"count" : 1, "date" : "2014-04-04", "req_time" : { "all" : 0.005 }, "status" : { "200" : 1 }, "url" : "/test01" }
```
�����������а���������Ҫ��Ϣ��
`stat_table` �Ǳ�����
`{'date':'2014-04-04', 'url':'/test01'}` �Ǹ���������������������������в�ѯ��
`{'$inc':{'count':1,'req_time.all':0.005,'status.200': 1}}` �Ǹ��±��ʽ��������һ��json��ʽ�ķ���mongodb���ġ�

##��װ��Ŀ������##
* ��ַ
```
mongo-c-driver  https://github.com/mongodb/mongo-c-driver<br/> 
json-c http://www.linuxfromscratch.org/blfs/view/svn/general/json-c.html
����mongo-c-driver��json-c��ͬ�汾���в����ݵ�������֣�ngx_req_stat/libs�������Ѿ����Կ��Ա���ͨ���İ汾��
```

* mongo-c-driver��װ��
```	
cd path/to/ngx_req_stat/libs
tar -xvf mongo-c-driver-0.98.0.tar.gz
cd mongo-c-driver-0.92.2
./configure --prefix=/usr
make && make install
```
* json-c ��װ:
```
cd path/to/ngx_req_stat/libs
tar -xvf json-c.tar.gz
./configure --prefix=/usr
make && make install
```
* ��װmongodb:
```
ͳ��ģ���ͳ����Ϣ�洢��mongodb�У�������Ҫ�Ȱ�װmongodb��
��http://www.mongodb.org/downloadsҳ�����غ��ʵİ汾��
```
###��������32λlinux�汾�İ�װ����:	
cd /usr/local
wget http://fastdl.mongodb.org/linux/mongodb-linux-i686-2.x.x.tgz
####\#��ѹ���ѳ����ѹ��/usr/localĿ¼�¡�
`tar -xvf mongodb-linux-i686-2.0.9.tgz`
####\#����������
`ln -s mongodb-linux-i686-2.0.9 mongodb`
####\#��������Ŀ¼
`mkdir /usr/local/mongodb/conf/`
#####\#���������ļ���`/usr/local/mongodb/conf/mongodb.conf`, �������£�
```
fork = true
bind_ip = 127.0.0.1
port = 27017
quiet = true
dbpath = /data/mongodb-ngx-stat/
pidfilepath = /data/mongodb-ngx-stat/mongodb.pid
logpath = /data/mongodb-ngx-stat/logs/mongod.log
journal = true
journalCommitInterval=300
syncdelay=300
```
#�������ݴ洢Ŀ¼
`mkdir -p /data/mongodb-ngx-stat/logs`
#�������ݿ�
`/usr/local/mongodb/bin/mongod -f` `/usr/local/mongodb/conf/mongodb.conf`
#����,����Ĭ�ϵ�ַ���˿ڡ�
`/usr/local/mongodb/bin/mongo`
����������Ϣ����ʾ���ӳɹ��ˣ�
```
MongoDB shell version: 2.0.9
connecting to: test
>> 
```
	
#��ģ�����#
```
��ģ����ʹ����C++��STL,������Ҫ��nginx���C++֧��(�޸�make�ļ�)��
C++֧�ֵĲ����ڣ�https://github.com/jie123108/ngx_cpp_make_patch ���������ء�
�򲹶�ʹ�ã�
cd nginx-1.x.x
patch -p1 < /path/to/nginx-cpp_make.patch
���뱾ģ�飺
cd nginx-1.x.x
./configure --add-module=/path/to/ngx_req_stat
make
make install
```

#��׼����ʾ��#
```
nginx.conf 
server {
    # Ĭ�ϵ����������ö��壬����Ϊdef���Ƽ�������Ϊ��$date+$uri �������԰��켰URL��ͳ�ƽ��в鿴��
    stat_key def "{'date':'$date','url':'$uri'}";
    # ����ʹ��stat_key����������ʽ����������ȡ��ͬ�����ƣ�Ȼ��������req_statָ����ʹ�á�
    
    # mongodb����(������Ĭ������ֵ)��
    mongo_uri mongodb:://127.0.0.1:27017/
    
    #ͳ�����ã�����ʹ��Ĭ�϶����def�������õĺ����ǣ�
    req_stat stat_tbl def "{'%inc': {'count':1,'hour_cnt.$hour':1, 'status.$status':1,'req_time.all': $request_time, 'req_time.$hour': $request_time}}";
    # %inc ��mongodb��$incָ���ʾΪ�����ÿһ��ֵ���мӲ���������nginx.conf��$�ᱻ�����ɱ���������ʹ��%�����
    # 'count':1 ��count�ֶμ�һ
    # 'hour_cnt.$hour':1 ��Сʱ���Ե�ǰСʱ��ͳ�����һ
    # 'status.$status:1' �Դ�����״̬���һ
    # 'req_time.all' ��������ʱ���ۼ�
    # 'req_time.$hour' ÿ��Сʱ������ʱ���ۼ�
    # req_stat ָ����Գ�����http,server,location�������У����Ҳ�ͬ�ĵط��������ò�ͬ��������ͳ��ֵ��
    
    # ���Ҫ�ر�ĳ��location��ͳ�ƣ�ʹ��:
    #req_stat off;
}
```

������ע�⣺req_statΪ��ͳ��ģ��������Ч�ʸ��ߣ���һ������ر��������������������£�������<br/>
>
```bash
/usr/local/mongodb/bin/mongo`
#����mongo���ݿ⣬��ͳ�����ݿ�ngx_stat
use ngx_stat;
#�������������� stat_tblΪreq_stat�����ָ����ͳ�Ʊ�������һ����������
#��������������date��url �ֱ�Ϊdef������Ӧ�������ֶΡ������ʹ�õ����������������ֶΣ����޸������ֶΡ�
db.stat_tbl.ensureIndex({date:1,url:1},{unique:true,dropDups:true})
```

##��С����ʾ����

```bash
server {
    #�������ö�ʹ��Ĭ��ֵ��
    req_stat stat_tbl def "{'%inc': {'count':1,'hour_cnt.$hour':1, 'status.$status':1,'req_time.all': $request_time, 'req_time.$hour': $request_time}}";
}
```

##��������ʹ��˵����

```
��Ŀ����conf/nginx.conf���������У�Ϊ�˲��Է��㣬ʹ����echo-nginx-moduleģ�顣�����Ҫ���иò��ԣ�����������echo-nginx-moduleģ�飬�ٱ��룺
```
	./configure --add-module=/path/to/ngx_req_stat --add-module=/path/to/echo-nginx-module
	nake & make install

####����nginx�󣬿���ʹ������Ľű��򵥲����£�
```bash
for ((i=0;i<20;i++));do
curl http://127.0.0.1:80/login/404 > /dev/null 2>&1
curl http://127.0.0.1:80/login?username=jie123108\&pwd=123
curl http://127.0.0.1:80/loginbyemail?email=jie123108@163.com\&pwd=123
done

for ((i=0;i<20;i++));do
curl http://127.0.0.1:80/login_new?client_type=pc\&username=jie123108\&pwd=123
curl http://127.0.0.1:80/login_new?client_type=android\&username=jie123108\&pwd=123
curl http://127.0.0.1:80/login_new?client_type=ios\&username=jie123108\&pwd=123
done

for ((i=0;i<20;i++));do
curl http://127.0.0.1:81/login/404 > /dev/null 2>&1
curl http://127.0.0.1:81/login?username=jie123108\&pwd=123
curl http://127.0.0.1:81/loginbyemail?email=jie123108@163.com\&pwd=123
curl http://127.0.0.1:81/nolog?xxxxx=xx
done
```
&nbsp;&nbsp;&nbsp;&nbsp;ע�⣺��ʵ��ʹ��ʱ��Ҫ�ȶԱ�������ʾ����ʹ����������(stat_80,stat_81)�����������������£�
```
/usr/local/mongodb/bin/mongo ngx_stat
MongoDB shell version: 2.0.9
connecting to: ngx_stat
> db.stat_80.ensureIndex({date:1,url:1},{unique:true,dropDups:true});
> db.stat_81.ensureIndex({date:1,url:1},{unique:true,dropDups:true});
```
####����֮�����ݿ��Ż�������������ļ�¼(_id�ֶ�������)��
```bash
> db.stat_80.find();
{ "count" : 20, "date" : "2014-04-06", "hour_cnt" : { "17" : 20 }, "req_time" : { "all" : 0, "17" : 0 }, "status" : { "200" : 20 }, "url" : "/login" }
{ "count" : 20, "date" : "2014-04-06", "hour_cnt" : { "17" : 20 }, "req_time" : { "all" : 0, "17" : 0 }, "status" : { "404" : 20 }, "url" : "/login/404" }
{ "count" : 20, "date" : "2014-04-06", "hour_cnt" : { "17" : 20 }, "req_time" : { "all" : 0, "17" : 0 }, "status" : { "200" : 20 }, "url" : "/loginbyemail" }
> db.stat_81.find();
{ "count" : 20, "date" : "2014-04-06", "hour_cnt" : { "17" : 20 }, "req_time" : { "all" : 0, "17" : 0 }, "status" : { "200" : 20 }, "url" : "/login" }
{ "count" : 20, "date" : "2014-04-06", "hour_cnt" : { "17" : 20 }, "req_time" : { "all" : 0, "17" : 0 }, "status" : { "404" : 20 }, "url" : "/login/404" }
{ "count" : 20, "date" : "2014-04-06", "hour_cnt" : { "17" : 20 }, "req_time" : { "all" : 0, "17" : 0 }, "status" : { "200" : 20 }, "url" : "/loginbyemail" }
```

##���ڲ�ѯ���棺
>
ngx_req_stat/NgxStatMgr ������һ���򵥵Ĳ�ѯ���档ʹ��Python��д��
#### ʹ��ǰ���Ȱ�װ�����Ŀ⣺
```bash
easy_install flask
easy_install simplejson
easy_install pymongo
```
#### ���У�
```sh
cd path/to/NgxStatMgr
export PYTHONPATH=`pwd`
python py/Main.py
```
#### ����:
```
http://ip:83/ngxstatmgr
```
��ѯ��������ͼ��
![��ѯ����](NgxStatMgr/static/NginxStatMgr.png)

��ϵ����:
=====
jie123108@163.com

