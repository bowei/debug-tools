#include <chrono>
#include <cstdlib>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include <errno.h>
#include <string.h>
#include <netdb.h>

using ChronoMs = std::chrono::duration<double, std::milli>;

struct ThreadArg {
  std::string name;
};

struct {
  size_t thread_count;
  std::string name;
} global_args;

std::mutex report_lock;

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
void run_thread();

int main(int argc, char* argv[]) {
  if (! parse_args(argc, argv)) {
    return 1;
  }

  std::vector<std::shared_ptr<std::thread>> threads;
  for (int i=0; i<5; ++i) {
    auto th = std::make_shared<std::thread>(run_thread);
    threads.push_back(th);
  }
  for (auto th : threads) {
    th->join();
  }
}

bool parse_args(int argc, char* argv[]) {
  for (int i=1; i<argc; ++i) {
    std::string arg(argv[i]);
    if (arg == "-n") {
      global_args.name = argv[i+1];
      ++i;
    } else if (arg == "-t") {
      global_args.thread_count = atoi(argv[i+1]);
      ++i;
    } else {
      std::cerr << "Invalid command line arg: " << arg << std::endl;
      ::exit(1);
    }
  }

  return true;
}

void run_thread() {
  std::vector<char> scratch(1 * 1024);
  struct hostent hret;
  struct hostent* result;
  int h_errno;

  for (int i=0; i<100; ++i) {
    auto start = std::chrono::high_resolution_clock::now();
    std::string name = global_args.name;


    const int ret = gethostbyname_r(
        name.c_str(), &hret, &scratch[0], scratch.size(), &result, &h_errno);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;

    std::ostringstream msg;
    msg << "gethostbyname(" << name << ")=" << ret
        << ", h_errno=" << h_errno << " (" << h_errno_to_str(h_errno) << ")"
        << ", " << elapsed.count() << " ms";

    std::lock_guard<std::mutex> guard(report_lock);
    std::cout << msg.str() << std::endl;
  }
}
