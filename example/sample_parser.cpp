#include <iostream>
#include <fstream>

#include "verilog_driver.hpp"

// A custom parser struct 
struct SampleParser : public verilog::ParserVerilogInterface {
  virtual ~SampleParser(){}

  void add_module(verilog::NameId name){
    std::cout << "Module name = " << name << '\n';
  }

  void add_port(verilog::Port&& port) {
    print(std::cout, port, intern()) << '\n';
    ports.push_back(std::move(port));
  }  

  void add_net(verilog::Net&& net) {
    std::cout << "Net: ";
    print(std::cout, net, intern()) << '\n';
    nets.push_back(std::move(net));
  }  

  void add_assignment(verilog::Assignment&& ast) {
    std::cout << "Assignment: ";
    print(std::cout, ast, intern()) << '\n';
    assignments.push_back(std::move(ast));
  }  

  void add_instance(verilog::Instance&& inst) {
    std::cout << "Instance: ";
    print(std::cout, inst, intern()) << '\n';
    insts.push_back(std::move(inst));
  }
  
  std::vector<verilog::Port> ports;
  std::vector<verilog::Net> nets;
  std::vector<verilog::Assignment> assignments;
  std::vector<verilog::Instance> insts;
};


int main(const int argc, const char **argv){
  if(argc < 2) {
    std::cerr << "Usage: ./sample_parser verilog_file\n";
    return EXIT_FAILURE;
  }
  
  std::ifstream f(argv[1]);
  if (!f.good()) {
      return -1;  // file does not exist or cannot be opened
  }
  
  SampleParser parser;
  parser.read(argv[1]);
  
  return EXIT_SUCCESS;
}
