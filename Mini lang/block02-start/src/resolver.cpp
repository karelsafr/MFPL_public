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

  Resolution run() {
    beginScope();
    resolveStatements(program_.main_body);
    endScope();
    return Resolution{};
  }

private:
  using Scope = std::unordered_map<std::string, Binding>;

  void beginScope() { scopes_.emplace_back(); }

  void endScope() { scopes_.pop_back(); }

  void resolveStatements(std::vector<Statement> &statements) {
    for (auto &statement : statements) {
      resolveStatement(statement);
    }
  }

  void resolveStatement(Statement &statement) {
    if (auto *print = std::get_if<PrintStatement>(&statement.node)) {
      resolveExpression(print->expression);
      return;
    }

    if (auto *variable = std::get_if<VarStatement>(&statement.node)) {
      const SymbolId id = declare(variable->name, variable->name_span);
      variable->resolved_id = id;
      resolveExpression(variable->initializer);
      define(variable->name);
      return;
    }

    if (auto *assignment = std::get_if<AssignmentStatement>(&statement.node)) {
      assignment->resolved_id = resolveBinding(assignment->name, assignment->name_span);
      resolveExpression(assignment->value);
      return;
    }

    if (auto *expression = std::get_if<ExpressionStatement>(&statement.node)) {
      resolveExpression(expression->expression);
      return;
    }

    if (auto *block = std::get_if<BlockStatement>(&statement.node)) {
      beginScope();
      resolveStatements(block->statements);
      endScope();
      return;
    }

    throw CompileError(Phase::Resolve, source_.name(), statement.span, "unsupported statement node");
  }

  void resolveExpression(Expression &expression) {
    if (auto *variable = std::get_if<VariableExpression>(&expression.node)) {
      variable->resolved_id = resolveBinding(variable->name, expression.span);
      return;
    }

    if (auto *grouping = std::get_if<GroupingExpression>(&expression.node)) {
      resolveExpression(*grouping->expression);
    }
  }

  SymbolId declare(const std::string &name, const SourceSpan &span) {
    Scope &scope = scopes_.back();
    if (scope.find(name) != scope.end()) {
      throw CompileError(Phase::Resolve, source_.name(), span, "duplicate variable '" + name + "' in the same scope");
    }

    const SymbolId id = next_symbol_id_++;
    scope.emplace(name, Binding{id, false});
    return id;
  }

  void define(const std::string &name) { scopes_.back().at(name).defined = true; }

  SymbolId resolveBinding(const std::string &name, const SourceSpan &span) {
    for (auto index = scopes_.size(); index > 0; --index) {
      const auto &scope = scopes_[index - 1];
      const auto binding = scope.find(name);

      if (binding == scope.end()) {
        continue;
      }

      if (!binding->second.defined) {
        throw CompileError(Phase::Resolve, source_.name(), span,
        "cannot read variable '" + name + "' in its own initializer");
      }

      return binding->second.id;
    }

    throw CompileError(Phase::Resolve, source_.name(), span, "unknown variable '" + name + "'");
  }

  const SourceText &source_;
  Program &program_;
  std::vector<Scope> scopes_;
  SymbolId next_symbol_id_{0};
};

} // namespace

Resolution resolve(const SourceText &source, Program &program) {
  return Resolver(source, program).run();
}

} // namespace mini
