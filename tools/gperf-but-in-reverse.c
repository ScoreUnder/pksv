#include <stdio.h>
#include <string.h>
#include <stdbool.h>

int main(int argc, char **argv) {
  if (argc > 1) {
    fprintf(stderr,
            "Usage: %s < input > output\nThere are no arguments, please use "
            "shell redirections.\n",
            argv[0]);
    return 1;
  }

  char lookup_function_name[256] = {0};
  char buf[1024];
  bool raw = false;

  while (fgets(buf, sizeof(buf), stdin)) {
    if (strcmp(buf, "%}\n") == 0) {
      raw = false;
      continue;
    }
    if (raw) {
      fputs(buf, stdout);
      continue;
    }
    if (strcmp(buf, "%{\n") == 0) {
      raw = true;
      continue;
    }
    if (strcmp(buf, "%%\n") == 0) {
      break;
    }

    char *cmd = strtok(buf, " \t\r\n");
    if (cmd && strcmp(cmd, "%define") == 0) {
      char *key = strtok(NULL, " \t\r\n");
      if (key && strcmp(key, "lookup-function-name") == 0) {
        char *val = strtok(NULL, " \t\r\n");
        if (val) {
          strcpy(lookup_function_name, val);
        } else {
          return 1;
        }
      }
    }
  }

  if (!*lookup_function_name) {
    fprintf(stderr, "lookup-function-name not defined\n");
    return 1;
  }

  printf("#include <stddef.h>\n");
  printf("static char const* %s_reverse_arr[] = {\n", lookup_function_name);
  while (fgets(buf, sizeof(buf), stdin)) {
    if (strcmp(buf, "%%\n") == 0) {
      break;
    }
    char *key = strtok(buf, ", \t\r\n");
    char *value = strtok(NULL, ", \t\r\n");
    printf("  [%s] = \"%s\",\n", value, key);
  }
  printf("};\n\n");
  printf("char const* %s_reverse(int value) {\n", lookup_function_name);
  printf(
      "  if (value < 0 || value >= "
      "(int)(sizeof(%s_reverse_arr)/sizeof(%s_reverse_arr[0]))) return NULL;\n",
      lookup_function_name, lookup_function_name);
  printf("  return %s_reverse_arr[value];\n", lookup_function_name);
  printf("}\n");

  return 0;
}
