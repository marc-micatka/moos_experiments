#include "talker_protobuf.h"

Talker::Talker() : random_engine_(rd_()), distribution_(0, 100) {
}

// NB: We're required to implement this, even if we don't do anything.
bool Talker::OnNewMail(MOOSMSG_LIST &NewMail) {
  std::cout << "Talker::OnNewMail" << std::endl;
  return true;
}

bool Talker::OnConnectToServer() {
  std::cout << "Talker::OnConnectToServer" << std::endl;
  return true;
}

bool Talker::Iterate() {
  Point3 msg;
  msg.set_x(distribution_(random_engine_));
  msg.set_y(distribution_(random_engine_));
  msg.set_z(distribution_(random_engine_));
  std::cout << "Talker publishing Point data: " << msg.x() << ", " << msg.y() << ", " << msg.z() << std::endl;

  // NB: Even though Protobuf serializes into a std::string, which MOOs
  //     supports as the STRING type, Notify needs to be called with
  //     the corresponding std::vector<unsigned char> for the binary data
  //     to be transmitted.
  //     Oddly, on the deserialization side, it's fine to use GetString
  //     and never convert into a vector of unsigned chars.
  std::string msg_string;
  msg.SerializeToString(&msg_string);
  std::vector<unsigned char> msg_chars(msg_string.begin(), msg_string.end());
  Notify(pub_variable_, msg_chars);

  return true;
}

bool Talker::OnStartUp() {
  std::cout << "Talker::OnStartUp" << std::endl;

  ParseParameters();

  return true;
}

void Talker::ParseParameters() {
  STRING_LIST moos_params;
  // Doublequotes are retained in incoming string
  m_MissionReader.EnableVerbatimQuoting(true);
  m_MissionReader.GetConfigurationAndPreserveSpace(GetAppName(), moos_params);

  // Using default values that may be overwritten. Probably cleaner to clear
  // them and then set them to non-default after parsing mission file.
  pub_variable_ = "";
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

      if (param == "pub_variable") {
        pub_variable_ = value;
      } else {
        std::cout << "Unrecognized parameter: " << param << std::endl;
      }
    }
  }
}
