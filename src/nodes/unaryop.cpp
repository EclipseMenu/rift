#include <rift/nodes/unaryop.hpp>
#include <rift/visitor.hpp>

namespace rift {

    void UnaryOpNode::accept(Visitor* visitor) {
        visitor->visit(this);
    }

}