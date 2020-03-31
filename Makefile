#CC=gcc
#My local: 
CC=C:/cygwin64/bin/gcc.exe
CFLAGS= -Wall
DEPS= crawler.h crawler.c str_util/str_util.c str_util/str_util.h vector/vector_strings.h vector/vector_strings.c uri/uri.h uri/uri.c vector/vector.h vector/vector.c filter/filter.c filter/filter.h html/get_links.h html/get_links.c fetch_page/http_response.c fetch_page/http_response.h fetch_page/fetch_page.c fetch_page/fetch_page.h
EXE = crawler
$(EXE): 
	$(CC) $(CFLAGS) $(DEPS)  main.c -o $(EXE)


clean:
	rm crawler

