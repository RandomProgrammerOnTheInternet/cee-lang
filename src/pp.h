#ifdef PP_IMPL

#include <stdio.h>
#include "util.h"

char *remove_line(char *str, u64 start, u64 *i);
char *remove_comments(char *str);
char *preprocess(char *str);

#endif // PP_IMPL

#ifndef PP_H
#define PP_H

char *remove_line(char *str, u64 start, u64 *i) {
	while(str[*i] != '\0' && str[*i] != '\n') {
		++*i;
	}
	return strcpy(substr(str, 0, start - 1), substr(str, *i, strlen(str) - 1));
}

char *remove_comments(char *str) {
	for(u64 i = 0; str[i]; i++) {
		if(str[i] == '/' && str[i + 1] == '/') {
			u64 start = i;
			i++;
			str = remove_line(str, start, &i);
		}
	}
	return str;
}

char *preprocess(char *str) {
	str = remove_comments(str);
	return str;
}




#endif // PP_H
