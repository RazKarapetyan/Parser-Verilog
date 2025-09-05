#ifndef PARSER_VERILOG_HPP_
#define PARSER_VERILOG_HPP_

#include <string>
#include <cstddef>
#include <fstream>
#include <variant>
#include <unordered_map>
#include <string_view>

#include "verilog_scanner.hpp"
#include "verilog_parser.tab.hh"

namespace verilog {

class ParserVerilogInterface {
  public:
    virtual ~ParserVerilogInterface(){
      if(_scanner) delete _scanner;
      if(_parser) delete _parser;
    }
    virtual void add_module(NameId) = 0;
    // port names, begin index, end index, port type (IOB), connection type (wire, reg)
    virtual void add_port(Port&&) = 0;
    virtual void add_net(Net&&) = 0;
    virtual void add_assignment(Assignment&&) = 0;
    virtual void add_instance(Instance&&) = 0;

    int read(const std::string&); 
    InternTable& intern() { return intern_; }
    const InternTable& intern() const { return intern_; }

  private:
    VerilogScanner* _scanner {nullptr};
    VerilogParser*  _parser {nullptr};
    InternTable     intern_;
};

inline int ParserVerilogInterface::read(const std::string& path){
  std::ifstream ifs(path);
  if (!ifs.good()) {
    return -1;  // file does not exist or cannot be opened
  }

  if(!_scanner){
    _scanner = new VerilogScanner(&ifs);
  }
  if(!_parser){
    _parser = new VerilogParser(*_scanner, this);
  }
  return _parser->parse();
}


} 
#endif 
