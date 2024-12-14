#pragma once
#ifndef RIFT_VISITOR_HPP
#define RIFT_VISITOR_HPP

#include "nodes/node.hpp"
#include "nodes/accessor.hpp"
#include "nodes/binary.hpp"
#include "nodes/call.hpp"
#include "nodes/identifier.hpp"
#include "nodes/indexer.hpp"
#include "nodes/root.hpp"
#include "nodes/ternary.hpp"
#include "nodes/unary.hpp"

namespace rift {

    using VisitorResult = geode::Result<Value, RuntimeError>;

    class Visitor {
    public:
        explicit Visitor(Object const& variables) noexcept : m_variables(variables) {}

        /// @brief Visit a node and evaluate its value.
        /// @param node the node to visit
        /// @return the result of the evaluation as a VisitorResult containing the value or an error
        [[nodiscard]] VisitorResult visit(Node const& node) const noexcept;

        /// @brief Visit a root node and evaluate its value.
        /// @param node the root node to visit
        /// @return the result of the evaluation as a VisitorResult containing the value
        [[nodiscard]] VisitorResult visit(RootNode const& node) const noexcept;

        /// @brief Visit an identifier node and evaluate its value.
        /// @param node the identifier node to visit
        /// @return the result of the evaluation as a VisitorResult containing the value
        [[nodiscard]] VisitorResult visit(IdentifierNode const& node) const noexcept;

        /// @brief Visit a binary node and evaluate its value.
        /// @param node the binary node to visit
        /// @return the result of the evaluation as a VisitorResult containing the value
        [[nodiscard]] VisitorResult visit(BinaryNode const& node) const noexcept;

        /// @brief Visit a unary node and evaluate its value.
        /// @param node the unary node to visit
        /// @return the result of the evaluation as a VisitorResult containing the value
        [[nodiscard]] VisitorResult visit(UnaryNode const& node) const noexcept;

        /// @brief Visit a ternary node and evaluate its value.
        /// @param node the ternary node to visit
        /// @return the result of the evaluation as a VisitorResult containing the value
        [[nodiscard]] VisitorResult visit(TernaryNode const& node) const noexcept;

        /// @brief Visit a call node and evaluate its value.
        /// @param node the call node to visit
        /// @return the result of the evaluation as a VisitorResult containing the value
        [[nodiscard]] VisitorResult visit(CallNode const& node) const noexcept;

        /// @brief Visit an accessor node and evaluate its value.
        /// @param node the accessor node to visit
        /// @return the result of the evaluation as a VisitorResult containing the value
        [[nodiscard]] VisitorResult visit(AccessorNode const& node) const noexcept;

        /// @brief Visit an indexer node and evaluate its value.
        /// @param node the indexer node to visit
        /// @return the result of the evaluation as a VisitorResult containing the value
        [[nodiscard]] VisitorResult visit(IndexerNode const& node) const noexcept;

    private:
        std::reference_wrapper<Object const> m_variables;
    };

}

#endif // RIFT_VISITOR_HPP