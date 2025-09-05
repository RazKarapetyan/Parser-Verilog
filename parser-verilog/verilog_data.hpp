#ifndef DATA_VERILOG_HPP_
#define DATA_VERILOG_HPP_

#include <string>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <variant>
#include <string_view>
#include <unordered_map>
#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>
#include <cstdint>
namespace verilog {

using NameId = uint32_t;

struct InternTable {
  std::unordered_map<std::string, NameId> map;  // no transparent functors needed
  std::vector<const std::string*> id2ptr;

  void reserve(size_t n){ map.reserve(n); id2ptr.reserve(n); }

  NameId id_of(std::string_view sv) {
    // C++17: find requires key_type; use a temporary std::string
    auto it = map.find(std::string(sv));
    if (it != map.end()) return it->second;

    NameId id = static_cast<NameId>(id2ptr.size());
    auto [node_it, inserted] = map.emplace(std::string(sv), id);
    id2ptr.push_back(&node_it->first);
    return id;
  }

  std::string_view view(NameId id) const { return *id2ptr[id]; }
  const std::string& str(NameId id) const { return *id2ptr[id]; }
};

  enum class ConstantType {
    NONE,
    INTEGER,
    BINARY,
    OCTAL, 
    DECIMAL,
    HEX, 
    REAL,
    EXP
  };

  inline std::ostream& operator<<(std::ostream& os, const ConstantType& t) {  
    switch(t) {
      case ConstantType::NONE:    os << "NONE";    break; 
      case ConstantType::INTEGER: os << "INTEGER"; break;
      case ConstantType::BINARY:  os << "BINARY";  break; 
      case ConstantType::OCTAL:   os << "OCTAL";   break; 
      case ConstantType::DECIMAL: os << "DECIMAL"; break;
      case ConstantType::HEX:     os << "HEX";     break; 
      case ConstantType::REAL:    os << "REAL";    break;
      case ConstantType::EXP:     os << "EXP";     break; 
    }
    return os;  
  }  
  
  struct Constant {
    Constant() = default;  // Need this default constructor for return token
    Constant(std::string&& v, ConstantType t) : value(std::move(v)), type(t) {}
    std::string value;
    ConstantType type {ConstantType::NONE};
  };

  inline std::ostream& operator<<(std::ostream& os, const Constant& c) {  
    return os;  
  }  

  enum class PortDirection {
    INPUT,
    OUTPUT,
    INOUT
  };

  inline std::ostream& operator<<(std::ostream& os, const PortDirection& dir) {  
    switch(dir){
      case PortDirection::INPUT:  os << "INPUT";  break; 
      case PortDirection::OUTPUT: os << "OUTPUT"; break;
      case PortDirection::INOUT:  os << "INOUT";  break; 
    }
    return os;  
  }  
    
  enum class ConnectionType {
    NONE,
    WIRE,
    REG
  };

  inline std::ostream& operator<<(std::ostream& os, const ConnectionType& ct) {  
    switch(ct){
      case ConnectionType::NONE:  os << "NONE"; break;
      case ConnectionType::WIRE:  os << "WIRE"; break;
      case ConnectionType::REG:   os << "REG";  break; 
    }
    return os;  
  } 

  struct Port {
    std::vector<NameId> names;
    int beg {-1};
    int end {-1};
    PortDirection dir;
    ConnectionType type {ConnectionType::NONE};
  };

  inline std::ostream& operator<<(std::ostream& os, const Port& port) {  
    os << "beg: " << port.beg << "  end: " << port.end << '\n';
    os << "Dir: " << port.dir << "  type: " << port.type << '\n';
    for(const auto& n: port.names){
      os << n << '\n';
    }
    return os;  
  }

  enum class NetType {
    NONE,
    REG,
    WIRE, 
    WAND,
    WOR,
    TRI,
    TRIOR,
    TRIAND,
    SUPPLY0,
    SUPPLY1
  };

  inline std::ostream& operator<<(std::ostream& os, const NetType& t) {  
    switch(t){
      case NetType::NONE:    os << "NONE";    break; 
      case NetType::REG:     os << "REG";     break; 
      case NetType::WIRE:    os << "WIRE";    break;
      case NetType::WAND:    os << "WAND";    break;
      case NetType::WOR:     os << "WOR";     break;
      case NetType::TRI:     os << "TRI";     break;
      case NetType::TRIAND:  os << "TRIAND";  break;
      case NetType::TRIOR:   os << "TRIOR";   break;
      case NetType::SUPPLY0: os << "SUPPLY0"; break;
      case NetType::SUPPLY1: os << "SUPPLY1"; break;
    }
    return os;  
  } 

  struct Net {
    std::vector<NameId> names;
    int beg {-1};
    int end {-1};
    NetType type {NetType::NONE};
  };

  inline std::ostream& operator<<(std::ostream& os, const Net& net) {  
    os << "beg: " << net.beg << "  end: " << net.end << '\n';
    os << "type: " << net.type << '\n';
    for(const auto& n: net.names){
      os << n << '\n';
    }
    return os;  
  }

  struct NetBit {
    NetBit(NameId n, int b): name(std::move(n)), bit(b) {}
    NameId name;
    int bit {-1};
  };

  inline std::ostream& operator<<(std::ostream& os, const NetBit& n) {  
    os << n.name << '[' << n.bit << "] ";
    return os;
  }

  struct NetRange {
    NetRange(NameId n, int b, int e): name(std::move(n)), beg(b), end(e) {}
    NameId name;
    int beg {-1};
    int end {-1};
  };

  inline std::ostream& operator<<(std::ostream& os, const NetRange& n) {  
    os << n.name << '[' << n.beg << ':' << n.end << "] ";
    return os;
  }
  
  struct Assignment {
    // Left hand side can be: a wire, a bit in a wire, a part of a wire  
    std::vector<std::variant<NameId, NetBit, NetRange>> lhs;

    // Right hand side can be: a wire, a bit in a wire, a part of a wire, a constant
    std::vector<std::variant<NameId, NetBit, NetRange, Constant>> rhs;
  };

  inline std::ostream& operator<<(std::ostream& os, const Assignment& ast) {  
    os << "LHS: ";
    for(const auto& l: ast.lhs){
      switch(l.index()){
        case 0: os << std::get<0>(l) << ' '; break;
        case 1: os << std::get<1>(l).name << '/' << std::get<1>(l).bit << ' '; 
                break;
        case 2: os << std::get<2>(l).name << '/' << std::get<2>(l).beg << '/' << std::get<2>(l).end << ' '; 
                break;
      }
    }
    os << '\n';
    os << "RHS: ";
    for(const auto& r: ast.rhs){
      switch(r.index()){
        case 0: os << std::get<0>(r) << ' '; break;
        case 1: os << std::get<1>(r).name << '/' << std::get<1>(r).bit << ' '; 
                break;
        case 2: os << std::get<2>(r).name << '/' << std::get<2>(r).beg << '/' << std::get<2>(r).end << ' '; 
                break;
        case 3: os << std::get<3>(r) << ' '; break;
      }
    }

    return os;  
  }

  using NetConcat = std::variant<NameId, NetBit, NetRange, Constant>;

  struct Instance {
    NameId module_name;
    NameId inst_name;
  
    // pin_names might be empty. e.g. my_module m1(net1, net2);
    std::vector<std::variant<NameId, NetBit, NetRange>> pin_names;
    std::vector<std::vector<NetConcat>> net_names;
  };

  inline std::ostream& operator<<(std::ostream& os, const Instance& inst) {  
    return os;
  }



} 
#endif  


