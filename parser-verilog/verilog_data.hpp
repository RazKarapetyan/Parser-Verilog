#ifndef DATA_VERILOG_HPP_
#define DATA_VERILOG_HPP_

#include <cstdint>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <variant>
#include <vector>

namespace verilog {

using NameId = uint32_t;

// ---------------- Interning ----------------

struct InternTable {
  std::unordered_map<std::string, NameId> map;  // C++17: key_type find only
  std::vector<const std::string*> id2ptr;

  void reserve(size_t n) { map.reserve(n); id2ptr.reserve(n); }

  NameId id_of(std::string_view sv) {
    // C++17: heterogeneous find not available; use temporary std::string
    auto it = map.find(std::string(sv));
    if (it != map.end()) return it->second;

    NameId id = static_cast<NameId>(id2ptr.size());
    auto ins = map.emplace(std::string(sv), id);
    id2ptr.push_back(&ins.first->first); // pointer to owned key
    return id;
  }

  std::string_view view(NameId id) const { return *id2ptr[id]; }
  const std::string& str(NameId id) const { return *id2ptr[id]; }
};

// ---------------- Constants ----------------

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
  switch (t) {
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
  Constant() = default;  // Needed for token returns
  Constant(std::string&& v, ConstantType t) : value(std::move(v)), type(t) {}
  std::string value;
  ConstantType type { ConstantType::NONE };
};

inline std::ostream& operator<<(std::ostream& os, const Constant& c) {
  return os << " constant value: " << c.value << " type: " << c.type;
}

// ---------------- Ports ----------------

enum class PortDirection { INPUT, OUTPUT, INOUT };

inline std::ostream& operator<<(std::ostream& os, const PortDirection& dir) {
  switch (dir) {
    case PortDirection::INPUT:  os << "INPUT";  break;
    case PortDirection::OUTPUT: os << "OUTPUT"; break;
    case PortDirection::INOUT:  os << "INOUT";  break;
  }
  return os;
}

enum class ConnectionType { NONE, WIRE, REG };

inline std::ostream& operator<<(std::ostream& os, const ConnectionType& ct) {
  switch (ct) {
    case ConnectionType::NONE: os << "NONE"; break;
    case ConnectionType::WIRE: os << "WIRE"; break;
    case ConnectionType::REG:  os << "REG";  break;
  }
  return os;
}

struct Port {
  std::vector<NameId> names;
  int beg { -1 };
  int end { -1 };
  PortDirection dir;
  ConnectionType type { ConnectionType::NONE };
};

inline std::ostream& print(std::ostream& os, const Port& port, const InternTable& it) {
  os << "beg: " << port.beg << "  end: " << port.end << '\n';
  os << "Dir: " << port.dir << "  type: " << port.type << '\n';
  for (auto id : port.names)
    os << it.str(id) << '\n';
  return os;
}

// ---------------- Nets ----------------

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
  switch (t) {
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
  int beg { -1 };
  int end { -1 };
  NetType type { NetType::NONE };
};

// quick-fix printer: use InternTable to resolve names
inline std::ostream& print(std::ostream& os, const Net& net, const InternTable& it) {
  os << "beg: " << net.beg << "  end: " << net.end << '\n';
  os << "type: " << net.type << '\n';
  for (auto id : net.names)
    os << it.str(id) << '\n';
  return os;
}

// ---------------- Net slices ----------------

struct NetBit {
  NetBit(NameId&& n, int b) : name(std::move(n)), bit(b) {}
  NameId name;
  int bit { -1 };
};

inline std::ostream& print(std::ostream& os, const NetBit& n, const InternTable& it) {
  os << it.str(n.name) << '[' << n.bit << "] ";
  return os;
}

struct NetRange {
  NetRange(NameId n, int b, int e) : name(std::move(n)), beg(b), end(e) {}
  NameId name;
  int beg { -1 };
  int end { -1 };
};

inline std::ostream& print(std::ostream& os, const NetRange& n, const InternTable& it) {
  os << it.str(n.name) << '[' << n.beg << ':' << n.end << "] ";
  return os;
}

// ---------------- Variants / helpers ----------------

using NetConcat = std::variant<NameId, NetBit, NetRange, Constant>;

// NameId printer
inline std::ostream& print(std::ostream& os, NameId id, const InternTable& it) {
  return os << it.str(id);
}

// Constant printer with table (table unused, but keeps uniform signature)
inline std::ostream& print(std::ostream& os, const Constant& c, const InternTable&) {
  return os << c.value;
}

// Variant printer
inline std::ostream& print(std::ostream& os, const NetConcat& v, const InternTable& it) {
  std::visit([&](auto const& x){ print(os, x, it); }, v);
  return os;
}

// ---------------- Assignment ----------------

struct Assignment {
  // LHS: wire | bit | range
  std::vector<std::variant<NameId, NetBit, NetRange>> lhs;
  // RHS: wire | bit | range | constant
  std::vector<std::variant<NameId, NetBit, NetRange, Constant>> rhs;
};

inline std::ostream& print(std::ostream& os, const Assignment& ast, const InternTable& it) {
  os << "LHS: ";
  for (const auto& l : ast.lhs) {
    std::visit([&](auto const& x){ print(os, x, it); }, l);
  }
  os << '\n';

  os << "RHS: ";
  for (const auto& r : ast.rhs) {
    std::visit([&](auto const& x){ print(os, x, it); }, r);
  }
  return os;
}

// ---------------- Instance ----------------

struct Instance {
  NameId module_name;
  NameId inst_name;

  // pin_names might be empty. e.g. my_module m1(net1, net2);
  std::vector<std::variant<NameId, NetBit, NetRange>> pin_names;
  std::vector<std::vector<NetConcat>> net_names; // per pin or positional
};

inline std::ostream& print(std::ostream& os, const Instance& inst, const InternTable& it) {
  os << it.str(inst.module_name) << ' ' << it.str(inst.inst_name) << " (";

  if (!inst.pin_names.empty()) {
    // Named connections: .pin(expr)
    for (size_t i = 0; i < inst.pin_names.size(); ++i) {
      if (i) os << ", ";

      os << '.';
      std::visit([&](auto const& pin){ print(os, pin, it); }, inst.pin_names[i]);

      os << '(';
      const auto& nets = inst.net_names[i];
      if (nets.size() > 1) os << '{';
      for (size_t j = 0; j < nets.size(); ++j) {
        if (j) os << ", ";
        print(os, nets[j], it);
      }
      if (nets.size() > 1) os << '}';
      os << ')';
    }
  } else {
    // Positional connections: (expr, expr, ...)
    for (size_t i = 0; i < inst.net_names.size(); ++i) {
      if (i) os << ", ";
      const auto& nets = inst.net_names[i];
      if (nets.size() > 1) os << '{';
      for (size_t j = 0; j < nets.size(); ++j) {
        if (j) os << ", ";
        print(os, nets[j], it);
      }
      if (nets.size() > 1) os << '}';
    }
  }

  os << ')';
  return os;
}

} // namespace verilog
#endif // DATA_VERILOG_HPP_