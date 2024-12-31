#include "function.hpp"

onto::Function::Function(const std::string_view& a_name, Type& a_type) noexcept
    : Node(Node::Kind::FUNCTION, a_name)
{
    addNeighbor(a_type, Relation::RETURNS);
}
