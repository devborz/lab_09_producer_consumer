// Copyright 2020 Usman Turkaev
#include <task.hpp>

int main(int argc, char* argv[]) {
  // if (argc >= 6) {
    char*  Argv = "--url=https://wylsa.com --network_threads=3 --depth=2 --parser_threads=3 --output=downloads";
    int Argc = 5;
    task::work(argc, argv);
  // }
  // else {
  //   std::cout << "error\n";
  // }
}
