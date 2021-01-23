#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>

#include "MOOS/libMOOS/MOOSLib.h"


class RelayQueueNode : public CMOOSApp {
 public:
  RelayQueueNode();
  virtual ~RelayQueueNode() {}

  bool OnNewMail(MOOSMSG_LIST &NewMail) override;
  bool Iterate() override;
  bool OnConnectToServer() override;
  bool Configure() override;

 private:
  void ParseParameters();
  void RegisterSubscriptions();
  bool OnMessage(CMOOSMsg& msg);

  std::string input_variable_;
  std::string output_variable_;

};  // class RelayQueueNode

RelayQueueNode::RelayQueueNode() {
}

bool RelayQueueNode::OnMessage(CMOOSMsg& msg) {
  Notify(output_variable_, msg.GetString());
  return true;
}

bool RelayQueueNode::OnNewMail(MOOSMSG_LIST &NewMail) {
  // We're purely using the queue callbacks.
  std::cout << "RelayQueueNode::OnNewMail called with " << NewMail.size() << " messages.\n";
  return true;
}

bool RelayQueueNode::OnConnectToServer() {
  RegisterSubscriptions();
  return true;
}

bool RelayQueueNode::Iterate() {
  // Everything is done in the message callbacks.
  return true;
}

bool RelayQueueNode::Configure() {
  CMOOSApp::Configure();
  ParseParameters();
  return true;
}

void RelayQueueNode::RegisterSubscriptions() {
  float min_time_between = 0;
  std::string input_queue = "input_queue";

  SetIterateMode(REGULAR_ITERATE_AND_COMMS_DRIVEN_MAIL);
  AddActiveQueue(input_queue, this, &RelayQueueNode::OnMessage);
  AddMessageRouteToActiveQueue(input_queue, input_variable_);
  Register(input_variable_, min_time_between);
}

void RelayQueueNode::ParseParameters() {
  STRING_LIST moos_params;
  m_MissionReader.EnableVerbatimQuoting(true);
  m_MissionReader.GetConfigurationAndPreserveSpace(GetAppName(), moos_params);

  input_variable_ = "";
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

      if (param == "input_variable") {
        input_variable_ = value;
      } else if (param == "output_variable") {
        output_variable_ = value;
      } else {
        std::cout << "WARNING: Unrecognized parameter " << param << std::endl;
      }
    }
  }
}


int main(int argc, char* argv[]) {
  RelayQueueNode relay;

  // Only handle the default arguments; all other configuration should
  // happen through the MOOS mission file.
  if (argc != 3) {
    std::string run_command = argv[0];
    std::cout << "USAGE: " << run_command << " [mission_file] [node_name]\n";
    return 1;
  }

  std::string mission_file = argv[1];
  std::string node_name = argv[2];
  relay.Run(node_name.c_str(), mission_file.c_str());
  return 0;
}

