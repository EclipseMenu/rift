#include <rift/nodes/ternary.hpp>
#include <rift/visitor.hpp>

namespace rift {

    void TernaryNode::accept(Visitor* visitor) {
        visitor->visit(this);
    }

}