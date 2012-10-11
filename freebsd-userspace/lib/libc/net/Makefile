# lib/libc/net Yacc and lex files

nslexer.c: nslexer.l
	${LEX} -P _nsyy -t $^ | \
		sed -e '/YY_BUF_SIZE/s/16384/1024/' >$@

nsparser.c: nsparser.y
	yacc -d -p_nsyy -o nsparser.i $^
	cat nsparser.i | \
		sed -e '/YY_BUF_SIZE/s/16384/1024/' >$@
	rm -f nsparser.i

