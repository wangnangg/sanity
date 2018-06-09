#include "marking.hpp"
#include <cassert>
namespace sanity::petrinet
{
std::hash<std::string_view> Marking::hasher = std::hash<std::string_view>();
Token Marking::nToken(uint pid) const { return _tokens[pid]; }
uint Marking::size() const { return _tokens.size(); }
void Marking::setToken(uint pid, Token num) { _tokens[pid] = num; }
std::unique_ptr<MarkingIntf> Marking::clone() const
{
    return std::make_unique<Marking>(*this);
}
std::size_t Marking::hash() const
{
    std::string_view sview((const char*)&_tokens[0],
                           sizeof(Token) * _tokens.size());
    return hasher(sview);
}
bool Marking::equal(const MarkingIntf* other) const
{
    auto mk = dynamic_cast<const Marking*>(other);
    return mk->_tokens == this->_tokens;
}

std::hash<std::vector<bool>> BitMarking::hasher =
    std::hash<std::vector<bool>>();
Token BitMarking::nToken(uint pid) const { return _tokens[pid]; }
uint BitMarking::size() const { return _tokens.size(); }
void BitMarking::setToken(uint pid, Token num)
{
    assert(num <= 1);
    _tokens[pid] = num;
}
std::unique_ptr<MarkingIntf> BitMarking::clone() const
{
    return std::make_unique<BitMarking>(*this);
}
std::size_t BitMarking::hash() const { return hasher(_tokens); }
bool BitMarking::equal(const MarkingIntf* other) const
{
    auto mk = dynamic_cast<const BitMarking*>(other);
    return mk->_tokens == this->_tokens;
}

std::hash<std::string_view> ByteMarking::hasher =
    std::hash<std::string_view>();
Token ByteMarking::nToken(uint pid) const { return _tokens[pid]; }
uint ByteMarking::size() const { return _tokens.size(); }
void ByteMarking::setToken(uint pid, Token num)
{
    assert(num <= 255);
    _tokens[pid] = num;
}
std::unique_ptr<MarkingIntf> ByteMarking::clone() const
{
    return std::make_unique<ByteMarking>(*this);
}
std::size_t ByteMarking::hash() const
{
    std::string_view sview((const char*)&_tokens[0],
                           sizeof(std::uint8_t) * _tokens.size());
    return hasher(sview);
}
bool ByteMarking::equal(const MarkingIntf* other) const
{
    auto mk = dynamic_cast<const ByteMarking*>(other);
    return mk->_tokens == this->_tokens;
}
}  // namespace sanity::petrinet
