#include "fred/ast/CommandNode.hpp"
#include "fred/command/CommandRegistry.hpp"
#include "fred/core/BufferManager.hpp"
#include "fred/lexer/Lexer.hpp"
#include "fred/lexer/TokenStream.hpp"
#include "fred/parser/CommandParser.hpp"
#include "fred/runtime/CommandExecutor.hpp"
#include "fred/runtime/ExecutionContext.hpp"
#include "fred/runtime/Output.hpp"

#include <cassert>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>

namespace {

std::unique_ptr<fred::CommandNode> parse(std::string_view source) {
    fred::Lexer lexer(source);
    fred::TokenStream tokens(lexer);
    const auto registry = fred::make_core_command_registry();
    fred::CommandParser parser(tokens, registry);
    return parser.parse();
}

void expect_error(std::string_view source, std::string_view fragment) {
    try {
        (void)parse(source);
        assert(false && "expected parse error");
    } catch (const std::exception& error) {
        assert(std::string_view(error.what()).find(fragment) !=
               std::string_view::npos);
    }
}

void execute(std::string_view source,
             fred::CommandExecutor& executor,
             fred::ExecutionContext& context) {
    auto command = parse(source);
    executor.execute(*command, context);
}

} // namespace

int main() {
    {
        const auto node = parse("\" commentaire : 1,$D n'est pas exécuté");
        assert(node->kind() == fred::AstNodeKind::CommentCommand);
        const auto& comment =
            static_cast<const fred::CommentCommandNode&>(*node);
        assert(comment.text() == "commentaire : 1,$D n'est pas exécuté");
    }

    {
        const auto node = parse("JM Bonjour, le monde !");
        assert(node->kind() == fred::AstNodeKind::MessageCommand);
        const auto& message =
            static_cast<const fred::MessageCommandNode&>(*node);
        assert(message.newline());
        assert(message.message() == "Bonjour, le monde !");
    }

    {
        const auto node = parse("jp/Chargement... /");
        assert(node->kind() == fred::AstNodeKind::MessageCommand);
        const auto& message =
            static_cast<const fred::MessageCommandNode&>(*node);
        assert(!message.newline());
        assert(message.message() == "Chargement... ");
    }

    expect_error("J", "only JM and JP are implemented");
    expect_error("JE test", "only JM and JP are implemented");
    expect_error("1JM test", "J does not accept a line address");
    expect_error("JM/message", "unterminated J message");

    fred::BufferManager buffers;
    fred::StringOutput output;
    fred::ExecutionContext context(buffers, output);
    fred::CommandExecutor executor;

    execute("\" aucun affichage", executor, context);
    assert(output.empty());

    execute("JP/Début... /", executor, context);
    execute("JP/OK/", executor, context);
    execute("JM", executor, context);
    assert(output.content() == "Début... OK\n");

    output.clear();
    execute("JM Message avec retour", executor, context);
    assert(output.content() == "Message avec retour\n");

    return 0;
}
