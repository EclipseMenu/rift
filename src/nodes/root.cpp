#include <rift/nodes/root.hpp>
#include <rift/visitor.hpp>

namespace rift {

    void RootNode::accept(Visitor* visitor) {
        visitor->visit(this);
    }

}