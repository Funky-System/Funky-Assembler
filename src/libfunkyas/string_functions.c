//
// Created by Bas du Pré on 12-02-18.
//

#include <string.h>
#include <stdlib.h>

#include "string_functions.h"

char* fas_str_replace(const char *string, const char *substr, const char *replacement) {
	char* tok = NULL;
	char* newstr = NULL;
	char* oldstr = NULL;
	size_t   oldstr_len = 0;
	size_t   substr_len = 0;
	size_t   replacement_len = 0;

	newstr = strdup(string);
	substr_len = strlen(substr);
	replacement_len = strlen(replacement);

	if (substr == NULL || replacement == NULL) {
		return newstr;
	}

    size_t offset = 0;
	while ((tok = strstr(newstr + offset, substr))) {
        offset = (tok - newstr) + replacement_len;

		oldstr = newstr;
		oldstr_len = strlen(oldstr);
		newstr = (char*)malloc(sizeof(char) * (oldstr_len - substr_len + replacement_len + 1));

		if (newstr == NULL) {
			free(oldstr);
			return NULL;
		}

		memcpy(newstr, oldstr, tok - oldstr);
		memcpy(newstr + (tok - oldstr), replacement, replacement_len);
		memcpy(newstr + (tok - oldstr) + replacement_len, tok + substr_len, oldstr_len - substr_len - (tok - oldstr));
		memset(newstr + oldstr_len - substr_len + replacement_len, 0, 1);

		free(oldstr);
	}

	return newstr;
}

void fas_str_replace_inplace(char **old_str, const char *substr, const char *replacement) {
	char *new_str = fas_str_replace(*old_str, substr, replacement);
	free(*old_str);
	*old_str = new_str;
}
