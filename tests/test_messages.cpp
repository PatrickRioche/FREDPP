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


    {
        const auto node = parse("JM!Mode d'emploi : !");
        assert(node->kind() == fred::AstNodeKind::MessageCommand);
        const auto& message =
            static_cast<const fred::MessageCommandNode&>(*node);
        assert(message.newline());
        assert(message.message() == "Mode d'emploi : ");
    }
    {
        const auto node = parse("JP:Input A/B:");
        assert(node->kind() == fred::AstNodeKind::MessageCommand);
        const auto& message =
            static_cast<const fred::MessageCommandNode&>(*node);
        assert(!message.newline());
        assert(message.message() == "Input A/B");
    }
    {
        const auto node = parse("JM !attention");
        const auto& message =
            static_cast<const fred::MessageCommandNode&>(*node);
        assert(message.message() == "!attention");
    }
    {
        fred::Lexer lexer("JM!A! JP:B/C: JM?D?");
        fred::TokenStream tokens(lexer);
        const auto registry = fred::make_core_command_registry();
        fred::CommandParser parser(tokens, registry);

        const auto first = parser.parse_one();
        assert(static_cast<const fred::MessageCommandNode&>(*first)
                   .message() == "A");

        const auto second = parser.parse_one();
        assert(static_cast<const fred::MessageCommandNode&>(*second)
                   .message() == "B/C");
        assert(!static_cast<const fred::MessageCommandNode&>(*second)
                    .newline());

        const auto third = parser.parse_one();
        assert(static_cast<const fred::MessageCommandNode&>(*third)
                   .message() == "D");
        assert(tokens.eof());
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

    output.clear();
    auto& message_buffer = buffers.create_or_select("msg");
    message_buffer.append("FLOW");
    execute("JM!\\S(msg)!", executor, context);
    assert(output.content() == "FLOW\n");

    expect_error("JM!unterminated", "unterminated J message");

    return 0;
}
