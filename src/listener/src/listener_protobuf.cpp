#include "listener_protobuf.h"

Listener::Listener() {
}

bool Listener::OnNewMail(MOOSMSG_LIST &NewMail) {
  // I'm *assuming* that this only gets called if there is new mail;
  // so check that here =)
  if (NewMail.empty()) {
    std::cout << "WARNING: Bad assumption! OnNewMail called without any pending messages!" << std::endl;
  }

  for (const CMOOSMsg& moos_msg : NewMail) {
    std::string key = moos_msg.GetKey();
    // For a real estimator, we'd also want to save the timestamps .. and
    // possibly actually update state here rather than just caching it.
    if (key == sub_variable_) {
      if (! moos_msg.IsString()) {
        std::cout << "WARNING: Received non-string msg in Listener::OnNewMail "
    	        << "name: " << moos_msg.GetName() << ", from: "
  		<< moos_msg.GetSource() << std::endl;
      } else {
        if (moos_msg.IsBinary()) {
          std::cout << "Whoops -- data is BINARY" << std::endl;
        }
        Point3 data;
        // Unlike on the Notify side, it's OK to use GetString to get
        // binary data.
        data.ParseFromString(moos_msg.GetString());

        x_buffer_.push_back(data.x());
        y_buffer_.push_back(data.y());
        z_buffer_.push_back(data.z());
      }
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

  Point3 point;
  point.set_x(sum_x);
  point.set_y(sum_y);
  point.set_z(sum_z);

  std::string msg_string;
  point.SerializeToString(&msg_string);
  std::vector<unsigned char> msg_chars(msg_string.begin(), msg_string.end());
  Notify(pub_variable_, msg_chars);

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
  m_Comms.Register(sub_variable_, min_time_between);
}

void Listener::ParseParameters() {
  STRING_LIST moos_params;
  // Not sure what this does; was in PXR_MOOSApp.cpp
  m_MissionReader.EnableVerbatimQuoting(true);
  m_MissionReader.GetConfigurationAndPreserveSpace(GetAppName(), moos_params);

  // Using default values that may be overwritten. Probably cleaner to clear
  // them and then set them to non-default after parsing mission file.
  sub_variable_ = "";
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

      if (param == "sub_variable") {
        sub_variable_ = value;
      } else if (param == "pub_variable") {
        pub_variable_ = value;
      } else {
        std::cout << "Unrecognized parameter: " << param << std::endl;
      }
    }
  }
}
