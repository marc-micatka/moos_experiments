#include "listener_protobuf.h"

int main(int argc, char* argv[]) {
  Listener listener;

  // Only handle the default arguments; all other configuration should
  // happen through the MOOS mission file.
  if (argc != 3) {
    std::string run_command = argv[0];
    std::cout << "USAGE: " << run_command << " [mission_file] [node_name]\n";
    return 1;
  }

  std::string mission_file = argv[1];
  std::string node_name = argv[2];
  listener.Run(node_name.c_str(), mission_file.c_str());
  return 0;
}

