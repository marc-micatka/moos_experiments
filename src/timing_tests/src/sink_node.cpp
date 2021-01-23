#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>

#include "MOOS/libMOOS/MOOSLib.h"


class SinkNode : public CMOOSApp {
 public:
  SinkNode();
  virtual ~SinkNode() {}

  bool OnNewMail(MOOSMSG_LIST &NewMail) override;
  bool Iterate() override;
  bool OnConnectToServer() override;
  bool Configure() override;

 private:
  void ParseParameters();
  void RegisterSubscriptions();
  bool OnMessage(CMOOSMsg& msg);

  std::string input_variable_;
  // How many messages have been received
  int count_;

};  // class SinkNode

SinkNode::SinkNode() : count_(0){
}

bool SinkNode::OnMessage(CMOOSMsg& msg) {
  count_ += 1;
  std::string moos_str = msg.GetString();
  auto msg_time = MOOSTime();
  std::vector<std::string> tokens;
  boost::split(tokens, moos_str, boost::is_any_of(","));
  double dt = msg_time - std::stod(tokens[1]);

  std::cout << "Received " << count_ << "-th message." << std::endl;
  std::cout << "   ... num = " << tokens[0] << ", dt = " << dt << std::endl;
  return true;
}

bool SinkNode::OnNewMail(MOOSMSG_LIST &NewMail) {
  // Everything happens callback-based, rather than allowing
  // the sink node's tick to change the delay calculations.
  return true;
}

bool SinkNode::OnConnectToServer() {
  RegisterSubscriptions();
  return true;
}

bool SinkNode::Iterate() {
  // Everything is done in the message callbacks.
  return true;
}

bool SinkNode::Configure() {
  CMOOSApp::Configure();
  ParseParameters();
  return true;
}

void SinkNode::RegisterSubscriptions() {
  float min_time_between = 0;
  std::string input_queue = "input_queue";
  AddMOOSVariable("input", input_variable_, "None", min_time_between);
  AddActiveQueue(input_queue, this, &SinkNode::OnMessage);
  AddMessageRouteToActiveQueue(input_queue, input_variable_);
  RegisterMOOSVariables();
}

void SinkNode::ParseParameters() {
  STRING_LIST moos_params;
  m_MissionReader.EnableVerbatimQuoting(true);
  m_MissionReader.GetConfigurationAndPreserveSpace(GetAppName(), moos_params);

  input_variable_ = "";
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

      if (param == "input_variable") {
        input_variable_ = value;
      } else {
        std::cout << "WARNING: Unrecognized parameter " << param << std::endl;
      }
    }
  }
}


int main(int argc, char* argv[]) {
  SinkNode source;

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

