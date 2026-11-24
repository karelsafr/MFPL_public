/*
 * Educational Mini compiler.
 * Demonstrates the translation pipeline for documented Mini 1.0 programs.
 * Diagnostics and edge-case handling are intentionally limited.
 * See the project specification and documented limitations.
 */

#include "resolver.hpp"

#include "diagnostics.hpp"

#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>

namespace mini {
namespace {

struct Binding {
  SymbolId id;
  bool defined;
};

class Resolver {
public:
  Resolver(const SourceText &source, Program &program)
      : source_(source) { (void)program; }

  Resolution run() {
    throw CompileError(Phase::Resolve, source_.name(), source_.span(0),
                       "TODO BLOCK02: implement the complete Resolver");
  }
  void pushScope() { scopes_.push_back({}); } // Vstup do bloku -> vytvoření nové prázdné mapy
  void popScope() { scopes_.pop_back(); } // Výstup z bloku -> odebere vrchní mapu i s jejími jmény

  SymbolId declare(const std::string &name) { // Registrace proměnné v aktuálním bloku
    SymbolId id = nextId_; // vezmeme si aktiální hodnotu počítadla
    nextId_ = nextId_ + 1; // posuneme počítadlo pro příští deklaraci
    scopes_.back()[name] = Binding{id, false}; // do vrchní mapy uložíme jméno s id
    return id;
  }

  const Binding *lookup(const std::string &name) const { // vrací ukazatel na nalezený binding nebo nullptr, když nic nenajde
    for (std::size_t i = scopes_.size(); i > 0; --i) {
      const auto &scope = scopes_[i - 1];
      auto it = scope.find(name);
      if (it != scope.end()) {
        return &it->second;
      }
    }
    return nullptr;
  }
  
  [[noreturn]] void error(std::size_t offset, const std::string &message) const {
    throw CompileError(Phase::Resolve, source_.name(), source_.span(offset), message);
  }
private:
  const SourceText &source_;
  std::vector<std::unordered_map<std::string, Binding>> scopes_; // Vytvoření zásobníku map
  SymbolId nextId_ = 0; // Počítadlo id (zvýší se po každé deklaraci o 1)
};

} // namespace

Resolution resolve(const SourceText &source, Program &program) {
  return Resolver(source, program).run();
}

} // namespace mini
