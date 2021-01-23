#include "talker_classic.h"

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
  Notify(x_variable_, distribution_(random_engine_));
  Notify(y_variable_, distribution_(random_engine_));
  Notify(z_variable_, distribution_(random_engine_));

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
  x_variable_ = "X";
  y_variable_ = "Y";
  z_variable_ = "Z";
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

      if (param == "x_variable") {
        x_variable_ = value;
      } else if (param == "y_variable") {
        y_variable_ = value;
      } else if (param == "z_variable") {
        z_variable_ = value;
      }
    }
  }
}
