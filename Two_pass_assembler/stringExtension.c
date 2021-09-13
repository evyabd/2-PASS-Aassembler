#include "stringExtension.h"

/*
 * Custom Implementation for the non-standard method - strdup, from string library.
 * Get string and copy it into dynamic memory.
 *
 * Params:
 * char *src: source string.
 *
 * Returns:
 * char *str: source string as a dynamic memory of char *.
*/
char *duplicateStr(char *src) {
  char *duplicate;
  if (src == NULL) {
    return NULL;
  }

  duplicate = (char *) calloc(strlen(src) + 1, sizeof(char));

  if (duplicate == NULL) {
    printf("Error: Allocation Error! \n");
    return NULL;
  }

  strcpy(duplicate, src);
  return duplicate;
}

/*
 * Custom Implementation for the non-standard method - strsep, from string library.
 * Get string and slice it by delim.
 *
 * Params:
 * char *src: source string.
 * const char *delim: the delimeter.
 *
 * Returns:
 * char *begin: the next "slice".
*/
char *myStrsep(char **string, const char *delim) {
  char *start, *end;
    start = *string;
  if (start == NULL)
    return NULL;
  /* set end to the end of the token */
  end = start + strcspn(start, delim);
  if (*end) {
    /* set *string past NULL character  */
    *(end++) = '\0';
    *string = end;
  } else {
    /* if this is the last token.  */
    *string = NULL;
  }
  return start;
}

/*
 * Remove whitespaces from the sides of string.
 *
 * Params:
 * char *string: a pointer for the string that represent string.
 *
 * Returns:
 * char *string: a pointer for the string that represent string without whitespaces in the sides.
*/
char *trimStr(char *string) {
  char *end;

  if (string == NULL) {
    return NULL;
  }

  /* trim leading space */
  while (isspace((unsigned char) *string)) string++;

  /* check if all the chars are space */
  if (*string == 0)
    return string;

  /* trim trailing space */
  end = string + strlen(string) - 1;
  while (end > string && isspace((unsigned char) *end)) end--;

  /* add null to the end of the string */
  end[1] = '\0';

  return string;
}

/*
 * Splits string to array, by delimiter.
 *
 * Params:
 * char *str: pointer to the string to split.
 * char *del: the character is divided by the sentence.
 *
 * Returns:
 * char** string: pointer to array of words, that contain the sentence.
 */
char **strSplit(char *str, const char del) {
  char **result = 0;
  size_t count = 0;
  char *tmp = str;
  char *last_comma = 0;
  char delim[2];
  delim[0] = del;
  delim[1] = 0;

  /* Count how many elements will be extracted. */
  while (*tmp) {
    if (del == *tmp) {
      count++;
      last_comma = tmp;
    }
    tmp++;
  }

  /* Add space for trailing token. */
  count += last_comma < (str + strlen(str) - 1);

  /* Add space for terminating null string so caller
     knows where the list of returned strings ends. */
  count++;

  result = malloc(sizeof(char *) * count);

  if (result) {
    size_t idx = 0;
    char *token = strtok(str, delim);

    while (token) {
      assert(idx < count);
      *(result + idx++) = duplicateStr(token);
      token = strtok(0, delim);
    }
    assert(idx == count - 1);
    *(result + idx) = 0;
  }

  return result;
}
/* Free array of strings.
 *
 * Params:
 * char **str: array of strings.
*/
void freeStringArray(char **str) {
  char **copy = str;
  if (str == NULL) {
    return;
  }
  while (*copy != NULL) {
    free(*copy);
    copy++;
  }
  free(str);
}