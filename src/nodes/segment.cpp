#include <rift/nodes/segment.hpp>
#include <rift/visitor.hpp>

namespace rift {

    void SegmentNode::accept(Visitor* visitor) {
        visitor->visit(this);
    }

    Value SegmentNode::getValue(Visitor* visitor) const {
        return Value::string("<segment>");
    }

}