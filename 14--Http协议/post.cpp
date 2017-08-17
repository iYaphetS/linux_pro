#include <iostream>
using namespace std;
#include <curl/curl.h>
#include <string.h>
#include <stdlib.h>

/*
用curl将客户端的请求协议封装，发送给服务器
然后将服务器返回的响应解析得到正文内容
*/

struct Data
{
    char* buf;
    int size;
    int dataSize;
};
//size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata);
size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
    struct Data* data = (struct Data*)userdata;
    // ptr --> point to data
    // size * nmemb = sizeof  data
    // userdata: callback data pointer

    int dataSize = size*nmemb;

    memcpy(data->buf + data->dataSize, ptr, dataSize);
    data->dataSize += dataSize;

    return dataSize;
}

// CURLcode curl_easy_setopt(CURL *handle, CURLoption option, parameter);
int main()
{

    CURLcode code;

    // 创建curl对象
    CURL* curl = curl_easy_init();
    if(curl == NULL)
    {
        printf("create curl error");
        return 0;
    }

    // 设置curl对象参数
    code = curl_easy_setopt(curl, CURLOPT_URL, "http://127.0.0.1/cgi-bin/a.out");
    if(code != CURLE_OK)
    {
        printf("setopt error\n");
        return 0;
    }

    struct Data data;
    data.buf = (char *)malloc(8192);
    data.size = 8192;
    data.dataSize = 0;
    //设置服务器响应后，不向标准输出设备输出，将回调回调函数
    code = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    //设置回调函数的传出参数
    code = curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);

    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "user=aa&pass=bb");
    curl_easy_setopt(curl, CURLOPT_POST, 1);

    // 通过curl对象向服务器发起请求, 阻塞调用，默认服务器响应将会输出到标准输出
    code = curl_easy_perform(curl);
    if(code  != CURLE_OK)
    {
        printf("perform error code=%d\n", (int)code);
        return 0;
    }

    data.buf[data.dataSize] = 0;
    printf("recv data is: %s\n", data.buf);
    free(data.buf);


    curl_easy_cleanup(curl);
}
