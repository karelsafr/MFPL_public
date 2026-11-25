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
      : source_(source), program_(program) {}

  // Scope těla main, průchod příkazy v pořadí zdroje, uzavření scope
  Resolution run() {
    pushScope();
    for (Statement &statement : program_.main_body) {
      resolveStatement(statement);
    }
    popScope();
    return Resolution{};
  }

  void pushScope() { scopes_.push_back({}); } // Vstup do bloku -> vytvoření nové prázdné mapy
  void popScope() { scopes_.pop_back(); } // Výstup z bloku -> odebere vrchní mapu i s jejími jmény

  // Registrace proměnné v aktuálním bloku -> duplicitní jméno ve stejném bloku je chyba
  SymbolId declare(const std::string &name, std::size_t offset) {
    if (scopes_.back().count(name) > 0) { // je to jméno už v TOMTO bloku?
      error(offset, "duplicate variable '" + name + "' in the same scope");
    }
    SymbolId id = nextId_; // vezmeme aktuální hodnotu počítadla
    nextId_ = nextId_ + 1; // posuneme ho pro příští deklaraci
    scopes_.back()[name] = Binding{id, false}; // zapíšeme s defined=false
    return id;
  }

  // Dokončení deklarace: po zpracování inicializátoru je proměnná použitelná
  void define(const std::string &name) {
    scopes_.back()[name].defined = true;
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

  // Průchod výrazem
  void resolveExpression(Expression &expression) {
    if (auto *variable = std::get_if<VariableExpression>(&expression.node)) { // je tenhle uzel proměnná?
      const Binding *binding = lookup(variable->name); // hledání ve scopes
      if (binding == nullptr) { // jméno neexistuje
        error(expression.span.offset, "unknown variable '" + variable->name + "'");
      }
      if (!binding->defined) { // jméno existuje, ale je ještě rozdělané
        error(expression.span.offset,
              "cannot read variable '" + variable->name + "' in its own initializer");
      }
      variable->resolved_id = binding->id; // zapíšeme do AST, kterou deklaraci to jméno označuje
    } else if (auto *grouping = std::get_if<GroupingExpression>(&expression.node)) { // závorky -> jdeme dovnitř
      resolveExpression(*grouping->expression);
    }
  }

  // Průchod příkazem
  void resolveStatement(Statement &statement) {
    if (auto *print = std::get_if<PrintStatement>(&statement.node)) {
      resolveExpression(print->expression);

    } else if (auto *expr = std::get_if<ExpressionStatement>(&statement.node)) {
      resolveExpression(expr->expression);

    } else if (auto *var = std::get_if<VarStatement>(&statement.node)) {
      SymbolId id = declare(var->name, var->name_span.offset);  // 1. zavést jako nedefinovanou
      resolveExpression(var->initializer);                      // 2. zkontrolovat inicializátor
      define(var->name);                                        // 3. teprve teď je použitelná
      var->resolved_id = id;

    } else if (auto *assign = std::get_if<AssignmentStatement>(&statement.node)) {
      const Binding *binding = lookup(assign->name);            // 1. najít cíl
      if (binding == nullptr) {
        error(assign->name_span.offset, "unknown variable '" + assign->name + "'");
      }
      assign->resolved_id = binding->id;                        // 2. zapsat id
      resolveExpression(assign->value);                         // 3. až pak pravá strana

    } else if (auto *block = std::get_if<BlockStatement>(&statement.node)) {
      pushScope();                                              // nový scope
      for (Statement &inner : block->statements) {              // projít všechny příkazy
        resolveStatement(inner);                                // rekurze
      }
      popScope();                                               // scope zaniká

    } else {
      error(statement.span.offset, "unsupported statement node");
    }
  }

private:
  const SourceText &source_;
  Program &program_;
  std::vector<std::unordered_map<std::string, Binding>> scopes_; // zásobník map
  SymbolId nextId_ = 0; // počítadlo id (zvýší se po každé deklaraci o 1)
};

} // namespace

Resolution resolve(const SourceText &source, Program &program) {
  return Resolver(source, program).run();
}

} // namespace mini
