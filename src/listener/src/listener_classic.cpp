#include "listener_classic.h"

Listener::Listener() {
}

bool Listener::OnNewMail(MOOSMSG_LIST &NewMail) {
  // I'm *assuming* that this only gets called if there is new mail;
  // so check that here =)
  if (NewMail.empty()) {
    std::cout << "WARNING: Bad assumption! OnNewMail called without any pending messages!" << std::endl;
  }

  for (const CMOOSMsg& moos_msg : NewMail) {
    // All relevant variables are doubles, so can just check right here.
    if (! moos_msg.IsDouble()) {
      std::cout << "WARNING: Received non-double msg in Listener::OnNewMail "
	        << "name: " << moos_msg.GetName() << ", from: "
		<< moos_msg.GetSource() << std::endl;
      continue;
    }

    std::string key = moos_msg.GetKey();
    // For a real estimator, we'd also want to save the timestamps .. and
    // possibly actually update state here rather than just caching it.
    if (key == x_variable_) {
      x_buffer_.push_back(moos_msg.GetDouble());
    } else if (key == y_variable_) {
      y_buffer_.push_back(moos_msg.GetDouble());
    } else if (key == z_variable_) {
      z_buffer_.push_back(moos_msg.GetDouble());
    } else {
      std::cout << "WARNING: Received message for variable " << key
	        << " and it's not handled! " << std::endl;
    }

  }  // end iterating over messages in NewMail

  return true;
}

bool Listener::Iterate() {
  float sum_x = std::accumulate(x_buffer_.begin(), x_buffer_.end(), 0);
  float sum_y = std::accumulate(y_buffer_.begin(), y_buffer_.end(), 0);
  float sum_z = std::accumulate(z_buffer_.begin(), z_buffer_.end(), 0);
  Notify(output_x_variable_, sum_x);
  Notify(output_y_variable_, sum_y);
  Notify(output_z_variable_, sum_z);

  return true;
}

bool Listener::Configure() {
  std::cout << "Listener::Configure" << std::endl;
  CMOOSApp::Configure();

  x_buffer_.set_capacity(3);
  y_buffer_.set_capacity(3);
  z_buffer_.set_capacity(3);

  ParseParameters();
  return true;
}

bool Listener::OnConnectToServer() {
  std::cout << "Listener::OnConnectToServer" << std::endl;
  RegisterSubscriptions();
  return true;
}

void Listener::RegisterSubscriptions() {
  // The documentation doesn't specify units; setting to 0 because estimation
  // wants to receive every sensor measurement.
  float min_time_between = 0;
  Register(x_variable_, min_time_between);
  Register(y_variable_, min_time_between);
  Register(z_variable_, min_time_between);
}

void Listener::ParseParameters() {
  STRING_LIST moos_params;
  // Not sure what this does; was in PXR_MOOSApp.cpp
  m_MissionReader.EnableVerbatimQuoting(true);
  m_MissionReader.GetConfigurationAndPreserveSpace(GetAppName(), moos_params);

  // Using default values that may be overwritten. Probably cleaner to clear
  // them and then set them to non-default after parsing mission file.
  x_variable_ = "";
  y_variable_ = "";
  z_variable_ = "";
  output_x_variable_ = "";
  output_y_variable_ = "";
  output_z_variable_ = "";

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
      } else if (param == "output_x_variable") {
        output_x_variable_ = value;
      } else if (param == "output_y_variable") {
        output_y_variable_ = value;
      } else if (param == "output_z_variable") {
        output_z_variable_ = value;
      } else {
        std::cout << "Unrecognized parameter: " << param << std::endl;
      }
    }
  }
}
