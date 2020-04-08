#llq: llqueue.c llqueue.h
#	gcc -Wall -o llq.o llqueue.c
#bst: bst.c bst.h
#main: main.c 
#target: main bst llq

#bst_print:target 
	#gcc -Wall -o bst_print main.c bst.c llqueue.c

all: crawler.c html_file_parser.c socket_operations.c http_headers.c
	gcc -Wall -o crawler crawler.c html_file_parser.c socket_operations.c http_headers.c -g 
local:
	gcc -Wall -o crawler crawler.c htmlfile_parser.c socket_operations.c http_headers.-g -DVM_DEBUG_ON
clean: 
	rm crawler

