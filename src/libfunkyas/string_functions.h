//
// Created by Bas du Pré on 12-02-18.
//

#ifndef WICKEDC_STRING_FUNCTIONS_H
#define WICKEDC_STRING_FUNCTIONS_H

#ifdef _MSC_VER
#define strdup _strdup
#endif

char* fas_str_replace(const char *string, const char *substr, const char *replacement);
void fas_str_replace_inplace(char **string, const char *substr, const char *replacement);

#endif //WICKEDC_STRING_FUNCTIONS_H
