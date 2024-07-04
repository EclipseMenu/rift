#include <rift/nodes/value.hpp>
#include <rift/visitor.hpp>

namespace rift {

    void ValueNode::accept(Visitor* visitor) {
        visitor->visit(this);
    }

}