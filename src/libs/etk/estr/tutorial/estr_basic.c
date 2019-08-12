/**
* this file is create by subsystem 'ETest' from EMake framework
*/

#include <etest.h>

#include "estr.h"

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

static int sstr_using_tutorial()
{
    puts("sstr_using_tutorial:");

    {
        estr e = estr_new(4); estr_show(e);

        sstr_wrtS(e, "12345678");  estr_show(e);    // will write failed
        sstr_wrtS(e, "1234");      estr_show(e);    // will write ok

        estr_free(e);
    }

    {
        char buf[16];

        // buf 头部有一部分会被内部使用
        sstr s = sstr_init(buf, 8); estr_show(s);

        sstr_wrtS(s, "12345678");   estr_show(s);    // will write failed
        sstr_wrtS(s, "1234");       estr_show(s);    // will write ok
    }

    return ETEST_OK;
}

int estr_basic(int argc, char* argv[])
{
    (void)argc; (void)argv;

    ETEST_RUN( estr_basic_api_tutorial() );
    ETEST_RUN( sstr_using_tutorial() );

    return ETEST_OK;
}

