#include <iostream>
#include <iomanip>
#include <chrono>

#include "verilog_driver.hpp"


// A custom parser struct 
struct VerilogParser : public verilog::ParserVerilogInterface {
  virtual ~VerilogParser(){}

  void add_module(verilog::NameId name){}
  void add_port(verilog::Port&& port) {}  
  void add_net(verilog::Net&& net) {}  
  void add_assignment(verilog::Assignment&& ast) {}  
  void add_instance(verilog::Instance&& inst) { } 
};


int main(const int argc, const char **argv) {
  return( EXIT_SUCCESS );
}
