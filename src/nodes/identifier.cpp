#include <rift/nodes/identifier.hpp>
#include <rift/visitor.hpp>

namespace rift {

    void IdentifierNode::accept(Visitor* visitor) {
        visitor->visit(this);
    }

}
