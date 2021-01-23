#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

#include "MOOS/libMOOS/MOOSLib.h"


class SourceNode : public CMOOSApp {
 public:
  SourceNode();
  virtual ~SourceNode() {}

  bool OnNewMail(MOOSMSG_LIST &NewMail) override;
  bool Iterate() override;
  bool OnConnectToServer() override;
  bool OnStartUp() override;

  void ParseParameters();

 private:
  std::string output_variable_;
  int count_;

};  // class SourceNode

SourceNode::SourceNode() : count_(0) {
}

bool SourceNode::OnNewMail(MOOSMSG_LIST &NewMail) {
  // Source node doesn't have any subscriptions
  return true;
}

bool SourceNode::OnConnectToServer() {
  return true;
}

bool SourceNode::Iterate() {
  // For later analysis, we need to include both sequence numbers and
  // timestamps in the same message.
  std::ostringstream msg;
  msg << boost::format("%1%,%2$10.3f") % count_ % MOOSTime();
  // BOO. This doesn't become available until C++20.
  // std::string msg = std::format("{},{}", count_, MOOSTime());
  count_ += 1;
  Notify(output_variable_, msg.str());
  return true;
}

bool SourceNode::OnStartUp() {
  ParseParameters();
  return true;
}

void SourceNode::ParseParameters() {
  STRING_LIST moos_params;
  m_MissionReader.EnableVerbatimQuoting(true);
  m_MissionReader.GetConfigurationAndPreserveSpace(GetAppName(), moos_params);

  output_variable_ = "";
  for (auto const& param_line : moos_params) {
    std::vector<std::string> tokens;
    boost::split(tokens, param_line, boost::is_any_of("="));
    if (2 != tokens.size()) {
      std::cout << "WARNING: input parameter line had invalid format. "
	      << "Expected 'name=value', got: " << param_line << std::endl;
    } else {
      std::string param = tokens.at(0);
      std::string value = tokens.at(1);
      std::cout << "Got parameter " << param << " with value = " << value << std::endl;

      if (param == "output_variable") {
        output_variable_ = value;
      } else {
        std::cout << "WARNING: Unrecognized parameter " << param << std::endl;
      }
    }
  }
}


int main(int argc, char* argv[]) {
  SourceNode source;

  // Only handle the default arguments; all other configuration should
  // happen through the MOOS mission file.
  if (argc != 3) {
    std::string run_command = argv[0];
    std::cout << "USAGE: " << run_command << " [mission_file] [node_name]\n";
    return 1;
  }

  std::string mission_file = argv[1];
  std::string node_name = argv[2];
  source.Run(node_name.c_str(), mission_file.c_str());
  return 0;
}

