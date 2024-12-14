#pragma once
#ifndef RIFT_SEGMENT_NODE_HPP
#define RIFT_SEGMENT_NODE_HPP

#include <rift/token.hpp>

#include "node.hpp"

namespace rift {

    class SegmentNode final : public Node {
    public:
        explicit SegmentNode(std::string value, size_t fromIndex, size_t toIndex) noexcept
            : Node(fromIndex, toIndex), m_value(std::move(value)) { m_type = Type::Segment; }

        explicit SegmentNode(Token const& token) noexcept
            : Node(token.fromIndex, token.toIndex), m_value(token.value) { m_type = Type::Segment; }

        [[nodiscard]] std::string toDebugString() const noexcept override {
            return fmt::format("SegmentNode(\"{}\")", m_value);
        }

        [[nodiscard]] std::string const& value() const noexcept {
            return m_value;
        }

    private:
        std::string m_value;
    };

}

#endif // RIFT_SEGMENT_NODE_HPP