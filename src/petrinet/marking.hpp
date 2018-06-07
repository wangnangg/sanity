#pragma once
#include <memory>
#include <vector>

namespace sanity::petrinet
{
using Token = uint;

class MarkingIntf
{
public:
    virtual ~MarkingIntf() = default;
    virtual Token nToken(uint pid) const = 0;
    virtual uint size() const = 0;
    virtual void setToken(uint pid, Token num) = 0;
    virtual std::unique_ptr<MarkingIntf> clone() const = 0;
    virtual std::size_t hash() const = 0;
    virtual bool equal(const MarkingIntf* other) const = 0;
};

class Marking : public MarkingIntf
{
    std::vector<Token> _tokens;
    static std::hash<std::string_view> hasher;

public:
    Marking() = default;
    Marking(uint nplace, uint ntoken = 0) : _tokens(nplace, ntoken) {}

    virtual ~Marking() = default;
    virtual Token nToken(uint pid) const override;
    virtual uint size() const override;
    virtual void setToken(uint pid, Token num) override;
    virtual std::unique_ptr<MarkingIntf> clone() const override;
    virtual std::size_t hash() const override;
    virtual bool equal(const MarkingIntf* other) const override;
};

class BitMarking : public MarkingIntf
{
    std::vector<bool> _tokens;
    static std::hash<std::vector<bool>> hasher;

public:
    BitMarking() = default;
    BitMarking(uint nplace, uint ntoken = 0) : _tokens(nplace, ntoken) {}

    virtual ~BitMarking() = default;
    virtual Token nToken(uint pid) const override;
    virtual uint size() const override;
    virtual void setToken(uint pid, Token num) override;
    virtual std::unique_ptr<MarkingIntf> clone() const override;
    virtual std::size_t hash() const override;
    virtual bool equal(const MarkingIntf* other) const override;
};

class ByteMarking : public MarkingIntf
{
    std::vector<std::uint8_t> _tokens;
    static std::hash<std::string_view> hasher;

public:
    ByteMarking() = default;
    ByteMarking(uint nplace, uint ntoken = 0) : _tokens(nplace, ntoken) {}

    virtual ~ByteMarking() = default;
    virtual Token nToken(uint pid) const override;
    virtual uint size() const override;
    virtual void setToken(uint pid, Token num) override;
    virtual std::unique_ptr<MarkingIntf> clone() const override;
    virtual std::size_t hash() const override;
    virtual bool equal(const MarkingIntf* other) const override;
};

}  // namespace sanity::petrinet
