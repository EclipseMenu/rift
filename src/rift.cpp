#include <rift.hpp>

#include <rift/parser.hpp>

namespace rift {

    CompileResult compile(std::string_view source, bool directMode) noexcept {
        Parser parser(Lexer(source, directMode), directMode);
        auto result = parser.parse();
        if (result.isErr()) {
            return geode::Err(result.unwrapErr());
        }
        return geode::Ok(std::make_unique<Script>(std::move(result.unwrap())));
    }

    FormatResult format(std::string_view source, Object const& variables) noexcept {
        auto compileResult = compile(source, false);
        if (compileResult.isErr()) {
            return geode::Err(compileResult.unwrapErr());
        }

        auto runResult = compileResult.unwrap()->run(variables);
        if (runResult.isErr()) {
            return geode::Err(CompileError(
                std::string(source),
                std::string(runResult.unwrapErr().message()),
                runResult.unwrapErr().index(),
                runResult.unwrapErr().index() + 1
            ));
        }

        return geode::Ok(runResult.unwrap());
    }

    EvaluateResult evaluate(std::string_view source, Object const& variables) noexcept {
        auto compileResult = compile(source, true);
        if (compileResult.isErr()) {
            return geode::Err(compileResult.unwrapErr());
        }

        auto evalResult = compileResult.unwrap()->eval(variables);
        if (evalResult.isErr()) {
            return geode::Err(CompileError(
                std::string(source),
                std::string(evalResult.unwrapErr().message()),
                evalResult.unwrapErr().index(),
                evalResult.unwrapErr().index() + 1
            ));
        }

        return geode::Ok(evalResult.unwrap());
    }

}
