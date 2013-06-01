#ifndef COSEnv_H
#define COSEnv_H

#include <cstdlib>

namespace COSEnv {
  inline bool checkenv(const std::string &name) {
    return (getenv((char *) name.c_str()) != NULL);
  }

  inline std::string getenv(const std::string &name) {
    char *env = ::getenv((char *) name.c_str());

    return (env ? std::string(env) : "");
  }

  inline void setenv(const std::string &name, const std::string &value) {
    std::string env = name + "=" + value;

    char *envp = strdup((char *) env.c_str());

    int error = ::putenv(envp);
  }

  inline void unsetenv(const std::string &name) {
    int name_len = name.size();

    int i = 0;

    while (environ[i] != NULL) {
      int environ_len = strlen(environ[i]);

      if (environ_len > name_len &&
          strncmp(environ[i], name.c_str(), name_len) == 0 &&
          environ[i][name_len] == '=')
        break;

      i++;
    }

    if (environ[i] == NULL)
      return;

    int i1 = i;

    while (environ[i1] != NULL)
      i1++;

    for (int j = i; j < i1; j++)
      environ[j] = environ[j + 1];
  }
}

#endif
