#pragma once


class Packet {
public:
    virtual ~Packet() = default;
    [[nodiscard]] virtual unsigned get_version_sum() const = 0;
    [[nodiscard]] virtual long long get_value() const = 0;
};


class ValuePacket final : public Packet {
public:

    ValuePacket(unsigned version, long long value) :
        version{version},
        value{value} {}

    [[nodiscard]]
    unsigned get_version_sum() const override {
        return version;
    }

    [[nodiscard]]
    long long get_value() const override {
        return value;
    }

private:
    unsigned version{};
    long long value{};
};


class OperatorPacket : public Packet {
public:

    OperatorPacket(unsigned version, std::vector<std::unique_ptr<Packet>>&& packets) :
        version{version},
        packets{std::move(packets)} {}

    ~OperatorPacket() override = default;

    [[nodiscard]]
    unsigned get_version_sum() const override {
        const auto accumulator = [](unsigned previous, const auto& current) {
            return previous + current->get_version_sum();
        };
        return std::accumulate(std::begin(packets), std::end(packets), version, accumulator);
    }

protected:
    unsigned version{};
    std::vector<std::unique_ptr<Packet>> packets{};
};


class SumPacket final : public OperatorPacket {
public:

    SumPacket(unsigned version, std::vector<std::unique_ptr<Packet>>&& packets) :
        OperatorPacket(version, std::move(packets)) {}

    [[nodiscard]]
    long long get_value() const override {
        const auto value_accumulator = [](long long previous, const auto& current) {
            return previous + current->get_value();
        };
        return std::accumulate(std::begin(packets), std::end(packets), 0LL, value_accumulator);
    }

};


class ProductPacket final : public OperatorPacket {
public:

    ProductPacket(unsigned version, std::vector<std::unique_ptr<Packet>>&& packets) :
        OperatorPacket(version, std::move(packets)) {}

    [[nodiscard]]
    long long get_value() const override {
        const auto value_accumulator = [](long long previous, const auto& current) {
            return previous * current->get_value();
        };
        return std::accumulate(std::begin(packets), std::end(packets), 1LL, value_accumulator);
    }
};


class MinimumPacket final : public OperatorPacket {
public:

    MinimumPacket(unsigned version, std::vector<std::unique_ptr<Packet>>&& packets) :
        OperatorPacket(version, std::move(packets)) {}

    [[nodiscard]]
    long long get_value() const override {
        const auto value_comparator = [](const auto& lhs, const auto& rhs) {
            return lhs->get_value() < rhs->get_value();
        };
        return (*std::min_element(std::begin(packets), std::end(packets), value_comparator))->get_value();
    }
};


class MaximumPacket final : public OperatorPacket {
public:

    MaximumPacket(unsigned version, std::vector<std::unique_ptr<Packet>>&& packets) :
        OperatorPacket(version, std::move(packets)) {}

    [[nodiscard]]
    long long get_value() const override {
        const auto value_comparator = [](const auto& lhs, const auto& rhs) {
            return lhs->get_value() < rhs->get_value();
        };
        return (*std::max_element(std::begin(packets), std::end(packets), value_comparator))->get_value();
    }
};


class GreaterThanPacket final : public OperatorPacket {
public:

    GreaterThanPacket(unsigned version, std::vector<std::unique_ptr<Packet>>&& packets) :
        OperatorPacket(version, std::move(packets)) {}

    [[nodiscard]]
    long long get_value() const override {
        return (packets.at(0)->get_value() > packets.at(1)->get_value() ? 1 : 0);
    }
};


class LessThanPacket final : public OperatorPacket {
public:

    LessThanPacket(unsigned version, std::vector<std::unique_ptr<Packet>>&& packets) :
        OperatorPacket(version, std::move(packets)) {}

    [[nodiscard]]
    long long get_value() const override {
        return (packets.at(0)->get_value() < packets.at(1)->get_value() ? 1 : 0);
    }
};


class EqualToPacket final : public OperatorPacket {
public:

    EqualToPacket(unsigned version, std::vector<std::unique_ptr<Packet>>&& packets) :
        OperatorPacket(version, std::move(packets)) {}

    [[nodiscard]]
    long long get_value() const override {
        return (packets.at(0)->get_value() == packets.at(1)->get_value() ? 1 : 0);
    }
};
