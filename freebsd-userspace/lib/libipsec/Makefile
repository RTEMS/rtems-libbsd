# libipsec Yacc and lex files

policy_token.c: policy_token.l
	${LEX} -P __libipsecyy -t $^ | \
		sed -e '/YY_BUF_SIZE/s/16384/1024/' >$@

policy_parse.c: policy_parse.y
	yacc -d -p __libipsecyy -o policy_parse.i $^
	cat policy_parse.i | \
		sed -e '/YY_BUF_SIZE/s/16384/1024/' >$@
	rm -f policy_parse.i

