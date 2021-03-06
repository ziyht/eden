/*
  Copyright (c) 2009 Dave Gamble

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#include <stdio.h>
#include <stdlib.h>
#include "ejson.h"

/* Parse text to JSON, then render back to text, and print! */
void doit(char *text)
{
    char *out;ejson json;

    json = ejss_eval(text);
    if (!json) {printf("Error before: [%s]\n",ejson_errp());}
    else
    {
        out=ejso_toFStr(json, 0);
        ejso_free(json);
        printf("%s\n",out);
        ejss_free(out);
    }
    fflush(stdout);
}

/* Read a file, parse, render back, etc. */
void dofile(char *filename)
{
    FILE *f;long len;char *data;

    f=fopen(filename,"rb");fseek(f,0,SEEK_END);len=ftell(f);fseek(f,0,SEEK_SET);
    data=(char*)malloc(len+1);fread(data,1,len,f);data[len]='\0';fclose(f);
    doit(data);
    free(data);
}

/* Used by some code below as an example datatype. */
struct record {const char *precision;double lat,lon;const char *address,*city,*state,*zip,*country; };

/* Create a bunch of objects as demonstration. */
void create_objects()
{
    // cJSON *root,*fmt,*img,*thm,*fld;char *out;int i;	/* declare a few. */
    ejson root, fmt, img, thm, fld;char *out;int i;
    /* Our "days of the week" array: */
    //const char *strings[7]={"Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"};
    /* Our matrix: */
    //int numbers[3][3]={{0,-1,0},{1,0,0},{0,0,1}};
    /* Our "gallery" item: */
    //int ids[4]={116,943,234,38793};
    /* Our array of "records": */
    struct record fields[2]={
        {"zip",37.7668,-1.223959e+2,"","SAN FRANCISCO","CA","94107","US"},
        {"zip",37.371991,-1.22026e+2,"","SUNNYVALE","CA","94085","US"}};
    //volatile double zero = 0.0;

    /* Here we construct some JSON standards, from the JSON site. */

    /* Our "Video" datatype: */
//	root=cJSON_CreateObject();
//	cJSON_AddItemToObject(root, "name", cJSON_CreateString("Jack (\"Bee\") Nimble"));
//	cJSON_AddItemToObject(root, "format", fmt=cJSON_CreateObject());
//	cJSON_AddStringToObject(fmt,"type",		"rect");
//	cJSON_AddNumberToObject(fmt,"width",		1920);
//	cJSON_AddNumberToObject(fmt,"height",		1080);
//	cJSON_AddFalseToObject (fmt,"interlace");
//	cJSON_AddNumberToObject(fmt,"frame rate",	24);

    root = ejso_new(_OBJ_);
    ejso_addS(root, "name", "Jack (\"Bee\") Nimble");
    fmt =  ejso_addE(root, "format","{}");
    ejso_addE(fmt, "type",       "rect");
    ejso_addN(fmt, "width",      1920);
    ejso_addN(fmt, "height",     1080);
    ejso_addT(fmt, "interlace",  _FALSE_);
    ejso_addN(fmt, "frame rate", 24);

//	out=cJSON_Print(root);	cJSON_Delete(root);	printf("%s\n",out);	free(out);	/* Print to text, Delete the cJSON, print it, release the string. */
    out = ejso_toFStr(root, 0); ejso_free(root); printf("%s\n", out); fflush(stdout);

    /* Our "days of the week" array: */
    //root=cJSON_CreateStringArray(strings,7);
    //out=cJSON_Print(root);	cJSON_Delete(root);	printf("%s\n",out);	free(out);
    root = ejss_eval("[\"Sunday\", \"Monday\", \"Tuesday\", \"Wednesday\", \"Thursday\", \"Friday\", \"Saturday\"]");
    out = ejso_toFStr(root, out); ejso_free(root); printf("%s\n", out); fflush(stdout);

    /* Our matrix: */
    //root=cJSON_CreateArray();
    //for (i=0;i<3;i++) cJSON_AddItemToArray(root,cJSON_CreateIntArray(numbers[i],3));
    root = ejss_eval("[]");
    ejso_addE(root, 0, "[0, -1, 0]");
    ejso_addE(root, 0, "[1,  0, 0]");
    ejso_addE(root, 0, "[0,  0, 1]");

/*	cJSON_ReplaceItemInArray(root,1,cJSON_CreateString("Replacement")); */

    //out=cJSON_Print(root);	cJSON_Delete(root);	printf("%s\n",out);	free(out);
    out = ejso_toFStr(root, out); ejso_free(root); printf("%s\n", out); fflush(stdout);

    /* Our "gallery" item: */
//	root=cJSON_CreateObject();
//	cJSON_AddItemToObject(root, "Image", img=cJSON_CreateObject());
//	cJSON_AddNumberToObject(img,"Width",800);
//	cJSON_AddNumberToObject(img,"Height",600);
//	cJSON_AddStringToObject(img,"Title","View from 15th Floor");
//	cJSON_AddItemToObject(img, "Thumbnail", thm=cJSON_CreateObject());
//	cJSON_AddStringToObject(thm, "Url", "http:/*www.example.com/image/481989943");
//	cJSON_AddNumberToObject(thm,"Height",125);
//	cJSON_AddStringToObject(thm,"Width","100");
//	cJSON_AddItemToObject(img,"IDs", cJSON_CreateIntArray(ids,4));

//	out=cJSON_Print(root);	cJSON_Delete(root);	printf("%s\n",out);	free(out);

    root = ejso_new(_OBJ_);
    img =  ejso_addT(root, "Image", _OBJ_);
    ejso_addN(img, "Width", 800);
    ejso_addN(img, "Height", 600);
    ejso_addS(img, "Title", "View from 15th Floor");
    thm = ejso_addT(img, "Thumbnail", _OBJ_);
    ejso_addS(thm, "Url", "http:/*www.example.com/image/481989943");
    ejso_addN(thm, "Height", 125);
    ejso_addN(thm, "Width", 100);
    ejso_addE(img, "IDs", "[116,943,234,38793]");
    out = ejso_toFStr(root, out); ejso_free(root); printf("%s\n", out); fflush(stdout);


    /* Our array of "records": */

//	root=cJSON_CreateArray();
//	for (i=0;i<2;i++)
//	{
//		cJSON_AddItemToArray(root,fld=cJSON_CreateObject());
//		cJSON_AddStringToObject(fld, "precision", fields[i].precision);
//		cJSON_AddNumberToObject(fld, "Latitude", fields[i].lat);
//		cJSON_AddNumberToObject(fld, "Longitude", fields[i].lon);
//		cJSON_AddStringToObject(fld, "Address", fields[i].address);
//		cJSON_AddStringToObject(fld, "City", fields[i].city);
//		cJSON_AddStringToObject(fld, "State", fields[i].state);
//		cJSON_AddStringToObject(fld, "Zip", fields[i].zip);
//		cJSON_AddStringToObject(fld, "Country", fields[i].country);
//	}

    root = ejso_new(_ARR_);
    for (i=0;i<2;i++)
    {
        fld = ejso_addT(root, 0, _OBJ_);
        ejso_addS(fld, "precision", fields[i].precision);
        ejso_addN(fld, "Latitude",  fields[i].lat);
        ejso_addN(fld, "Longitude", fields[i].lon);
        ejso_addS(fld, "Address",   fields[i].address);
        ejso_addS(fld, "City",      fields[i].city);
        ejso_addS(fld, "Zip",       fields[i].zip);
        ejso_addS(fld, "Country",   fields[i].country);
    }
    out = ejso_toFStr(root, out); ejso_free(root); printf("%s\n", out); fflush(stdout);

///*	cJSON_ReplaceItemInObject(cJSON_GetArrayItem(root,1),"City",cJSON_CreateIntArray(ids,4)); */

//	out=cJSON_Print(root);	cJSON_Delete(root);	printf("%s\n",out);	free(out);

//	root=cJSON_CreateObject();
//	cJSON_AddNumberToObject(root,"number", 1.0/zero);
//	out=cJSON_Print(root);	cJSON_Delete(root);	printf("%s\n",out);	free(out);

    ejss_free(out);
}

void ejson_cJSON_test() {
    /* a bunch of json: */
    char text1[]="{\n\"name\": \"Jack (\\\"Bee\\\") Nimble\", \n\"format\": {\"type\":       \"rect\", \n\"width\":      1920, \n\"height\":     1080, \n\"interlace\":  false,\"frame rate\": 24\n}\n}";
    char text2[]="[\"Sunday\", \"Monday\", \"Tuesday\", \"Wednesday\", \"Thursday\", \"Friday\", \"Saturday\"]";
    char text3[]="[\n    [0, -1, 0],\n    [1, 0, 0],\n    [0, 0, 1]\n	]\n";
    char text4[]="{\n		\"Image\": {\n			\"Width\":  800,\n			\"Height\": 600,\n			\"Title\":  \"View from 15th Floor\",\n			\"Thumbnail\": {\n				\"Url\":    \"http:/*www.example.com/image/481989943\",\n				\"Height\": 125,\n				\"Width\":  \"100\"\n			},\n			\"IDs\": [116, 943, 234, 38793]\n		}\n	}";
    char text5[]="[\n	 {\n	 \"precision\": \"zip\",\n	 \"Latitude\":  37.7668,\n	 \"Longitude\": -122.3959,\n	 \"Address\":   \"\",\n	 \"City\":      \"SAN FRANCISCO\",\n	 \"State\":     \"CA\",\n	 \"Zip\":       \"94107\",\n	 \"Country\":   \"US\"\n	 },\n	 {\n	 \"precision\": \"zip\",\n	 \"Latitude\":  37.371991,\n	 \"Longitude\": -122.026020,\n	 \"Address\":   \"\",\n	 \"City\":      \"SUNNYVALE\",\n	 \"State\":     \"CA\",\n	 \"Zip\":       \"94085\",\n	 \"Country\":   \"US\"\n	 }\n	 ]";

    char text6[] = "<!DOCTYPE html>"
        "<html>\n"
        "<head>\n"
        "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n"
        "  <style type=\"text/css\">\n"
        "    html, body, iframe { margin: 0; padding: 0; height: 100%; }\n"
        "    iframe { display: block; width: 100%; border: none; }\n"
        "  </style>\n"
        "<title>Application Error</title>\n"
        "</head>\n"
        "<body>\n"
        "  <iframe src="//s3.amazonaws.com/heroku_pages/error.html">\n"
        "    <p>Application Error</p>\n"
        "  </iframe>\n"
        "</body>\n"
        "</html>\n";

    /* Process each json textblock by parsing, then rebuilding: */
    doit(text1);
    doit(text2);
    doit(text3);
    doit(text4);
    doit(text5);
    doit(text6);

    /* Parse standard testfiles: */
/*	dofile("../../tests/test1"); */
/*	dofile("../../tests/test2"); */
/*	dofile("../../tests/test3"); */
/*	dofile("../../tests/test4"); */
/*	dofile("../../tests/test5"); */
/*	dofile("../../tests/test6"); */

    /* Now some samplecode for building objects concisely: */
    create_objects();

}

