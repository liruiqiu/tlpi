
//#include "tlpi_hdr.h"
#include <stdio.h>
#include <stdlib.h>
#include "xmlparse.h"
#define BUF_SIZE    1024

int main(int argc, char *argv[])
{    

	XTREE xTree;
	int fd = open("dns/a.cn",O_RDONLY); 
	char *xmlbuf = malloc(BUF_SIZE);
	memset(xmlbuf, 0, BUF_SIZE);
	read(fd, xmlbuf, BUF_SIZE);
	XTREE xTree;
	memset(xTree,0,sizeof(XTREE));
	XmlParseFree(xmlbuf, BUF_SIZE, &xTree); 

	printXtree(&xTree);

/*
	char xmlbuf[BUF_SIZE]  = {0};

	XmlWriteHead(xmlbuf, "utf-8") ;
	XmlWriteNodeBeg(xmlbuf, "test");
	XmlWriteTag(xmlbuf, "data", "abc") ;
	XmlWriteNodeEnd(xmlbuf, "test");

	XmlParseRead(xmlbuf, strlen(xmlbuf), &xTree) ;
	printXtree(&xTree) ;
	XmlParseFree(&xTree ) ;
*/

	return 0;
	//exit(EXIT_SUCCESS);
}
