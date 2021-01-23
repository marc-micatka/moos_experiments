#ifndef MOOS_EXPERIMENTS_TALKER_CLASSIC_H
#define MOOS_EXPERIMENTS_TALKER_CLASSIC_H

#include <random>
#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>

#include "MOOS/libMOOS/MOOSLib.h"

class Talker : public CMOOSApp {
 public:
  Talker();
  virtual ~Talker() {}

  bool OnNewMail(MOOSMSG_LIST &NewMail) override;
  bool Iterate() override;
  bool OnConnectToServer() override;
  bool OnStartUp() override;

  void ParseParameters();

  private:
  // Names of variables in MOOSDB for this node's outputs
  std::string x_variable_;
  std::string y_variable_;
  std::string z_variable_;

  // Random generators for sensor data
  std::random_device rd_;
  std::mt19937 random_engine_;
  std::uniform_real_distribution<float> distribution_;

};  // class Talker

#endif  // MOOS_EXPERIMENTS_TALKER_CLASSIC_H
