#include <rift/nodes/binaryop.hpp>
#include <rift/visitor.hpp>

namespace rift {

    void BinaryOpNode::accept(Visitor* visitor) {
        visitor->visit(this);
    }

}