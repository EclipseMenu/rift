#include <rift/nodes/segment.hpp>
#include <rift/visitor.hpp>

namespace rift {

    void SegmentNode::accept(Visitor* visitor) {
        visitor->write(getText());
    }

}