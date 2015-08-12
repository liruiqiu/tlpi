
//#include "tlpi_hdr.h"
#include <stdio.h>
#include <stdlib.h>
#include "xmlparse.h"
#define BUF_SIZE    1024

int main(int argc, char *argv[])
{    
	XTREE xTree;

	char xmlbuf[BUF_SIZE]  = {0};

	XmlWriteHead(xmlbuf, "utf-8") ;
	XmlWriteNodeBeg(xmlbuf, "test");
	XmlWriteTag(xmlbuf, "data", "abc") ;
	XmlWriteNodeEnd(xmlbuf, "test");

	XmlParseRead(xmlbuf, strlen(xmlbuf), &xTree) ;
	printXtree(&xTree) ;
	XmlParseFree(&xTree ) ;

	return 0;
	//exit(EXIT_SUCCESS);
}
