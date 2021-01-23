#ifndef MOOS_EXPERIMENTS_LISTENER_PROTOBUF_H
#define MOOS_EXPERIMENTS_LISTENER_PROTOBUF_H

#include <random>
#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/circular_buffer.hpp>

#include "MOOS/libMOOS/MOOSLib.h"
#include "test_messages.pb.h"

class Listener : public CMOOSApp {
 public:
  Listener();
  virtual ~Listener() {}

  bool OnNewMail(MOOSMSG_LIST &NewMail) override;
  bool Iterate() override;
  bool OnConnectToServer() override;
  bool Configure() override;

  void ParseParameters();
  void RegisterSubscriptions();

 private:
  // Variables for input
  std::string sub_variable_;
  // Variables for output
  std::string pub_variable_;
  // In this trivially simple node, the estimate will be the average of the last 3 values
  boost::circular_buffer<double> x_buffer_;
  boost::circular_buffer<double> y_buffer_;
  boost::circular_buffer<double> z_buffer_;

};  // class Listener

#endif  // MOOS_EXPERIMENTS_LISTENER_PROTOBUF_H
