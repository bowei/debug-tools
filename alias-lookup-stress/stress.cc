#include <iostream>
#include <string>
#include <vector>

#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <pthread.h>

struct ThreadArg {
  std::string name;
};

const char* h_errno_to_str(int v) {
  switch (v) {
    case HOST_NOT_FOUND: return "HOST_NOT_FOUND";
    case NO_DATA: return "NO_DATA";
    case NO_RECOVERY: return "NO_RECOVERY";
    case TRY_AGAIN: return "TRY_AGAIN";
    default: return "INVALID";
  }
}

bool parse_args(int argc, char* argv[]);
void* run(void* arg);

int main(int argc, char* argv[]) {
  if (! parse_args(argc, argv)) {
    return 1;
  }

  pthread_t thr;
  pthread_create(&thr, nullptr, run, nullptr);
  pthread_join(thr, nullptr);
}

bool parse_args(int argc, char* argv[]) {
  return true;
}

void* run(void* arg) {
  std::cout << "run()" << std::endl;
  std::string name("no.such.domain.svc");
  //std::string name("google.com");

  std::vector<char> scratch(1 * 1024);
  struct hostent hret;
  struct hostent* result;
  int h_errno;

  while (true) {
    const int ret = gethostbyname_r(
        name.c_str(), &hret, &scratch[0], scratch.size(), &result, &h_errno);
    if (ret != 0) {
      std::cout
          << "ret=" << ret << ": " << strerror(errno)
          << ", " << h_errno << ": " << h_errno_to_str(h_errno)
          << std::endl;
    }
  }

  return nullptr;
}
