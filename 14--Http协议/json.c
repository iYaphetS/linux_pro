
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"


struct person
{
    int age;
    char name[32];
    int sex;
};


//将复杂的数据通过json打包，在网络上传输
#if 0
{
    age: 18,
    name: mike,
    sex: man
}
#endif
int main1()
{

    struct person p;
    p.age = 18;
    //p.name = mike;
    strcpy(p.name, "mike");
    p.sex = 1;//man
    //打包
    cJSON *root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "age", p.age);
    cJSON_AddStringToObject(root, "name", p.name);
    cJSON_AddBoolToObject(root, "sex", p.sex);

    char *jsonbuf = cJSON_Print(root);
    printf("%s\n", jsonbuf);

    cJSON_Delete(root);
    free(jsonbuf);
    return 0;
}

#if 0
{
    name: "mike",
    age: 18,
    subject:{
        projname: "c++",
        score: 80
    }
}
#endif
int main()
{
    cJSON *subject = cJSON_CreateObject();
    cJSON_AddStringToObject(subject, "projname", "c++");
    cJSON_AddNumberToObject(subject, "score", 80);

    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "name", "mike");
    cJSON_AddNumberToObject(root, "age", 18);
    cJSON_AddItemToObject(root, "subject", subject);

    char *p = cJSON_Print(root);


    {
        cJSON *root = cJSON_Parse(p);
        cJSON *name = cJSON_GetObjectItem(root, "name");
        printf("%s=%s\n", name->string, name->valuestring);

        cJSON *age = cJSON_GetObjectItem(root, "age");
        printf("%s=%d, %s=%g\n", age->string, age->valueint, age->string, age->valuedouble);


        cJSON *subject = cJSON_GetObjectItem(root, "subject");
        cJSON *projname = cJSON_GetObjectItem(subject, "projname");
        printf("%s=%s\n", projname->string, projname->valuestring);

        cJSON *score = cJSON_GetObjectItem(subject, "score");
        printf("%s=%g\n", score->string, score->valuedouble);

    }
}
