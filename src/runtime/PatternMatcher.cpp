#include "fred/runtime/PatternMatcher.hpp"

#include <algorithm>
#include <cstddef>
#include <vector>

namespace fred {
namespace {

using Positions = std::vector<std::size_t>;

void append_unique(Positions& target, std::size_t value) {
    if (std::find(target.begin(), target.end(), value) == target.end()) {
        target.push_back(value);
    }
}

void append_unique(Positions& target, const Positions& values) {
    for (const auto value : values) {
        append_unique(target, value);
    }
}

Positions match_from(const PatternNode& node,
                     std::string_view text,
                     std::size_t position);

Positions match_repetition(const RepetitionPatternNode& repetition,
                           std::string_view text,
                           std::size_t position) {
    Positions result;
    Positions frontier;

    if (repetition.type() == RepetitionPatternNode::Type::ZeroOrMore) {
        result.push_back(position);
        frontier.push_back(position);
    } else {
        frontier = match_from(repetition.operand(), text, position);
        append_unique(result, frontier);
    }

    while (!frontier.empty()) {
        Positions next_frontier;
        for (const auto current : frontier) {
            const auto ends = match_from(repetition.operand(), text, current);
            for (const auto end : ends) {
                if (end == current) {
                    continue;
                }
                if (std::find(result.begin(), result.end(), end) == result.end()) {
                    result.push_back(end);
                    next_frontier.push_back(end);
                }
            }
        }
        frontier = std::move(next_frontier);
    }

    return result;
}

Positions match_from(const PatternNode& node,
                     std::string_view text,
                     std::size_t position) {
    switch (node.kind()) {
    case AstNodeKind::PatternLiteral: {
        const auto& literal = static_cast<const LiteralPatternNode&>(node);
        if (position < text.size() && text[position] == literal.value()) {
            return {position + 1};
        }
        return {};
    }
    case AstNodeKind::PatternAnyCharacter:
        return position < text.size() ? Positions{position + 1} : Positions{};
    case AstNodeKind::PatternAnchor: {
        const auto& anchor = static_cast<const AnchorPatternNode&>(node);
        if (anchor.type() == AnchorPatternNode::Type::StartOfLine) {
            return position == 0 ? Positions{position} : Positions{};
        }
        return position == text.size() ? Positions{position} : Positions{};
    }
    case AstNodeKind::PatternSequence: {
        const auto& sequence = static_cast<const SequencePatternNode&>(node);
        Positions positions{position};
        for (const auto& element : sequence.elements()) {
            Positions next;
            for (const auto current : positions) {
                append_unique(next, match_from(*element, text, current));
            }
            positions = std::move(next);
            if (positions.empty()) {
                break;
            }
        }
        return positions;
    }
    case AstNodeKind::PatternAlternation: {
        const auto& alternation = static_cast<const AlternationPatternNode&>(node);
        Positions result;
        for (const auto& alternative : alternation.alternatives()) {
            append_unique(result, match_from(*alternative, text, position));
        }
        return result;
    }
    case AstNodeKind::PatternRepetition:
        return match_repetition(
            static_cast<const RepetitionPatternNode&>(node), text, position);
    case AstNodeKind::PatternCharacterClass: {
        const auto& character_class =
            static_cast<const CharacterClassPatternNode&>(node);
        if (position >= text.size()) {
            return {};
        }
        const auto value = text[position];
        bool contained = false;
        for (const auto range : character_class.ranges()) {
            if (value >= range.first && value <= range.last) {
                contained = true;
                break;
            }
        }
        if (character_class.negated()) {
            contained = !contained;
        }
        return contained ? Positions{position + 1} : Positions{};
    }
    case AstNodeKind::PatternGroup: {
        const auto& group = static_cast<const GroupPatternNode&>(node);
        return match_from(group.expression(), text, position);
    }
    default:
        return {};
    }
}

} // namespace

bool PatternMatcher::search(const PatternNode& pattern,
                            std::string_view text) const {
    for (std::size_t start = 0; start <= text.size(); ++start) {
        if (!match_from(pattern, text, start).empty()) {
            return true;
        }
    }
    return false;
}

} // namespace fred
