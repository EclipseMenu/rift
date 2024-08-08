#pragma once

#include "node.hpp"

#include <string>
#include <utility>

namespace rift {

    /// @brief A node in the AST representing a static text segment.
    class SegmentNode : public Node {
    public:
        /// @brief Construct the segment node.
        /// @param text The text of the segment.
        explicit SegmentNode(std::string_view text) : m_text(text) {}

        /// @brief Get the text of the segment.
        /// @return The text of the segment.
        [[nodiscard]] std::string_view getText() const { return m_text; }

        /// @copydoc Node::getType
        [[nodiscard]] Type getType() const override { return Type::Segment; }

        /// @copydoc Node::accept
        void accept(Visitor* visitor) override;

        /// @copydoc Node::getValue
        [[nodiscard]] Value getValue(Visitor* visitor) const override;

        /// @copydoc Node::print
        std::ostream& print(std::ostream& out) const override {
            return out << "SegmentNode('" << m_text << "')";
        }

    private:
        std::string m_text;
    };

}