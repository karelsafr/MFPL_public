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

private:
  const SourceText &source_;
};

} // namespace

Resolution resolve(const SourceText &source, Program &program) {
  return Resolver(source, program).run();
}

} // namespace mini
