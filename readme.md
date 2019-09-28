# eden project

  简单易用的通用跨平台工具包

> 我只是站在巨人的肩膀上

# 目录

* 管理工具：
    * emake   - 工程管理框架
* 外部库：
    * libuv   - 跨平台异步事件框架
* 内部库：
    * etk
        * ebase
            * [ethread](#ethread)   - 线程
            * eatom                 - 原子操作
            * [echan](#echan)       - 管道（多线程）
            * [estr](#estr)         - 动态字串
            * evar                  - 变量variant
            * evec                  - 向量
        * eobjs
            * [ejson](#ejson)       - json解析器
            * ell                   - 链表
            * erb                   - 红黑树
            * esl                   - 跳表
            * edict                 - 字典
        * ecodec
            * elz4                  - lz4 压缩解压

## [ethread]()
[目录](#目录)

> * ethread 目前适配基本的工具，`ethread`, `emutex`, `econd`, `erwlock`, `ebarrier`, `ethread_once`, `ethread_key`  
> * 在 mac 和 linux 中底层使用 `pthread`
> * 在 windows 中底层使用 `pthread_win32`

```C
#include "ethread.h"

int counter = 0;

void* t1_cb(void* d)
{
    emutex_t* mu = d;

    int i = 100000;

    while(i--)
    {
        emutex_lock(*mu);
        counter++;
        emutex_ulck(*mu);
    }

    return (void*)1;
}

static int t1_basic_case1()
{

    ethread_t th1, th2; emutex_t mu;

    emutex_init(mu);

    ethread_init(th1, t1_cb, &mu);
    ethread_init(th2, t1_cb, &mu);

    ethread_join(th1);
    ethread_join(th2);

    printf("%d\n", counter);

    return ETEST_OK;
}
```
```
==================== OUTPUT ===================
200000
```

## [echan]()
[目录](#目录)
```C
#include "ethread.h"
#include "echan.h"

int count = 100000;

void* recver(void* d)
{
    echan c = d;

    while (1) {
        evar v = echan_timeRecvV(c, 1000);

        if(v.type == E_NAV)
            return (void*)0;

        if(v.v.i32 == count)
            break;
    }

    return (void*)1;
}

static int t1_basic_case1()
{

    ethread_t th; void* ret;
    echan c = echan_new(E_I32, 128);    // type, cap

    ethread_init(th, recver, c);

    for(int i = 1; i <= count; i ++)
    {
        if(false == echan_sendV(c, EVAR_I32(i)))
        {
            puts("send failed");
            break;
        }
    }

    ethread_join_ex(th, ret);
    echan_free(c);

    if(ret != (void*)1)
    {
        puts("recver failed");
        return ETEST_ERR;
    }

    puts("ok");

    return ETEST_OK;
}
```

```
==================== OUTPUT ===================
ok
```

## [estr](./src/libs/etk/ebase/doc/estr.md)
[目录](#目录)
```c
static int estr_basic_api_tutorial()
{
    estr s = 0;     // init from 0 or null

    //
    // wrt API always write from the beginning
    // cat API write continued
    //
    estr_wrtS(s, "Hello" );  puts(s);       // Hello
    estr_catS(s, ", estr");  puts(s);       // Hello, estr

    estr_wrtS(s, "clear" );  puts(s);       // clear

    // or you can show the detail of estr
    estr_show(s);                           // (estr: e08 5/22):[clear]    // 5 is len, 22 is cap

    puts("----");

    // basic API                                     // DATA            NOTE
    estr_wrtS(s, "str");            estr_show(s);    // [str]           write cstr
    estr_wrtB(s, "123\0567", 7);    estr_show(s);    // [123\0567]      write bin data
    estr_wrtW(s, "  wordd123");     estr_show(s);    // [wordd]         write a word
    estr_wrtL(s, " line\nline2");   estr_show(s);    // [ line]         write a line
    estr_wrtT(s, "abcde", 'e');     estr_show(s);    // [abcd]          write to the end char
    estr_wrtP(s, "%s", "fmt");      estr_show(s);    // [fmt]           write fmt(like sprintf)
    estr_wrtP(s, "%0*d", 4, 1);     estr_show(s);    // [0001]          write fmt(like sprintf)
    estr_wrtF(s, "%s", "efmt");     estr_show(s);    // [efmt]          write fmt(simple internal)

    puts("----");

    // free it after using it
    estr_free(s);
    s = 0;          // you need to set s to 0 by you self if needed
    estr_show(s);

    return ETEST_OK;
}
```
```
==================== OUTPUT ===================
Hello
Hello, estr
clear
(estr: e08 5/22):[clear]
----
(estr: e08 3/22):[str]
(estr: e08 7/22):[123.7 ]
(estr: e08 5/22):[wordd]
(estr: e08 5/22):[ line]
(estr: e08 4/22):[abcd]
(estr: e08 3/22):[fmt]
(estr: e08 4/22):[0001]
(estr: e08 4/22):[efmt]
----
(estr: nullptr)
```


## [ejson](./src/libs/etk/eobj/ejson.md)
[目录](#目录)
``` c
static int t2_val_get2()
{
    char* s =  "{"
                    "\"fruit.name\" :\"apple\","
                    "\"fruit.color\":\"red\","
                    "\"fruit\":{"
                    "   \"name\": \"orange\","
                    "   \"color\": \"orange\","
                    "}"
                "}";

    ejson e = ejson_parseS(s);

    puts("Get val from OBJ by using rawk or path: ");
    ejson e1 = ejson_r(e, "fruit.name");    // r 表示 rawk，不解析 key，直接查找
    ejson e2 = ejson_r(e, "fruit.color");
    ejson e3 = ejson_p(e, "fruit.name");    // p 表示 path，会进行内部解析，支持 . or []
    ejson e4 = ejson_p(e, "fruit.name");
    ejson e5 = ejson_p(e, "fruit[name]");   // p 表示 path，会进行内部解析，支持 . or []
    ejson e6 = ejson_p(e, "fruit[name]");

    printf("e1: %s\n", eobj_valS(e1));
    printf("e2: %s\n", eobj_valS(e2));
    printf("e3: %s\n", eobj_valS(e3));
    printf("e4: %s\n", eobj_valS(e4));
    printf("e5: %s\n", eobj_valS(e5));
    printf("e6: %s\n", eobj_valS(e6));

    printf("%s\n", ejson_rValS(e, "fruit.name"));
    printf("%s\n", ejson_rValS(e, "fruit.color"));
    printf("%s\n", ejson_pValS(e, "fruit.name"));
    printf("%s\n", ejson_pValS(e, "fruit.color"));
    printf("%s\n", ejson_pValS(e, "fruit[name]"));
    printf("%s\n", ejson_pValS(e, "fruit[color]"));

    ejson_free(e);

    puts("Get val from ARR by using index or path: ");
    s = "[4, 3, 2, 1]";
    e = ejson_parseS(s);

    printf("i: %" PRId64 "\n", ejson_iValI(e, 0));  // 对于数组，可以直接 使用 i族 API 获取值
    printf("i: %" PRId64 "\n", ejson_iValI(e, 1));
    printf("i: %" PRId64 "\n", ejson_iValI(e, 2));
    printf("i: %" PRId64 "\n", ejson_iValI(e, 3));

    printf("p: %" PRId64 "\n", ejson_pValI(e, "0"));  // 对于数组，也可以使用 p族 API 取值，但是效率没有 i族高
    printf("p: %" PRId64 "\n", ejson_pValI(e, "1"));
    printf("p: %" PRId64 "\n", ejson_pValI(e, ".2"));
    printf("p: %" PRId64 "\n", ejson_pValI(e, "[3]"));

    ejson_free(e);

    return ETEST_OK;
}
```
```
================ output ================
Get val from OBJ by using rawk or path: 
e1: apple
e2: red
e3: orange
e4: orange
e5: orange
e6: orange
apple
red
orange
orange
orange
orange
Get val from ARR by using index or path: 
i: 4
i: 3
i: 2
i: 1
p: 4
p: 3
p: 2
p: 1
```