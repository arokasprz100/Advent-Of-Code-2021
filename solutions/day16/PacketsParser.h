#pragma once

#include <Packets.h>
#include <Converters.h>

class PacketsParser {
public:

    static std::unique_ptr<Packet> parse(const std::string& binary_transmission) {
        return extract_packet_data(binary_transmission).first;
    }

private:
    using PacketData = std::pair<std::unique_ptr<Packet>, unsigned>;

    static constexpr unsigned PACKET_VERSION_LENGTH = 3;
    static constexpr unsigned PACKET_TYPE_ID_LENGTH = 3;
    static constexpr unsigned PACKET_LENGTH_TYPE_ID_LENGTH = 1;
    static constexpr unsigned CONTENT_SIZE_IN_BITS_LENGTH = 15;
    static constexpr unsigned NUMBER_OF_SUB_PACKETS_LENGTH = 11;
    static constexpr unsigned VALUE_PACKET_TYPE_ID = 4;

    static inline unsigned get_packet_version(const std::string& packet_as_binary) {
        std::string version_as_binary = packet_as_binary.substr(0, PACKET_VERSION_LENGTH);
        return converters::binary_to_decimal(version_as_binary);
    }

    static inline unsigned get_packet_type_id(const std::string& packet_as_binary) {
        std::string type_id_as_binary = packet_as_binary.substr(PACKET_VERSION_LENGTH, PACKET_TYPE_ID_LENGTH);
        return converters::binary_to_decimal(type_id_as_binary);
    }

    static inline std::string remove_packet_version_and_type(const std::string& packet_as_binary) {
        return packet_as_binary.substr(PACKET_VERSION_LENGTH + PACKET_TYPE_ID_LENGTH);
    }

    static inline bool contains_not_only_zeros(const std::string& binary_transmission) {
        return binary_transmission.find('1') != std::string::npos;
    }

    static PacketData extract_packet_data(const std::string& binary_transmission) {
        unsigned packet_version = get_packet_version(binary_transmission);
        unsigned packet_type_id = get_packet_type_id(binary_transmission);
        auto packet_content = remove_packet_version_and_type(binary_transmission);
        return packet_type_id == VALUE_PACKET_TYPE_ID
            ? extract_value_packet_data(packet_version, packet_content)
            : extract_operator_packet_data(packet_version, packet_type_id, packet_content);
    }

    static std::string parse_value_packet_content(std::string packet_content) {
        std::string value_as_binary{};
        while(packet_content.length() >= 5) {
            std::string next_five_bits = packet_content.substr(0, 5);
            value_as_binary += next_five_bits.substr(1);
            packet_content = packet_content.substr(5);
            if(next_five_bits.at(0) == '0') {
                break;
            }
        }
        return value_as_binary;
    }

    static unsigned compute_value_packet_length(const std::string& packet_value_as_binary) {
        return PACKET_VERSION_LENGTH + PACKET_TYPE_ID_LENGTH + (packet_value_as_binary.length() / 4) * 5;
    }

    static PacketData extract_value_packet_data(unsigned packet_version, const std::string& packet_content) {
        std::string value_as_binary = parse_value_packet_content(packet_content);
        long long packet_value = converters::binary_to_long_decimal(value_as_binary);
        unsigned packet_length = compute_value_packet_length(value_as_binary);
        return { std::make_unique<ValuePacket>(packet_version, packet_value), packet_length };
    }

    template<typename... Args>
    static std::unique_ptr<OperatorPacket> operator_packet_by_type_id(unsigned packet_type_id, Args&&... args) {
        switch(packet_type_id) {
            case 0: return std::make_unique<SumPacket>(std::forward<Args>(args)...);
            case 1: return std::make_unique<ProductPacket>(std::forward<Args>(args)...);
            case 2: return std::make_unique<MinimumPacket>(std::forward<Args>(args)...);
            case 3: return std::make_unique<MaximumPacket>(std::forward<Args>(args)...);
            case 5: return std::make_unique<GreaterThanPacket>(std::forward<Args>(args)...);
            case 6: return std::make_unique<LessThanPacket>(std::forward<Args>(args)...);
            case 7: return std::make_unique<EqualToPacket>(std::forward<Args>(args)...);
            default: throw std::runtime_error{"Unsupported type of operator packet"};
        }
    }

    static PacketData extract_operator_packet_with_length_type_id_one_data(unsigned packet_version, unsigned packet_type_id, const std::string& packet_content) {
        std::vector<std::unique_ptr<Packet>> sub_packets{};
        unsigned number_of_sub_packets = converters::binary_to_decimal(packet_content.substr(PACKET_LENGTH_TYPE_ID_LENGTH, NUMBER_OF_SUB_PACKETS_LENGTH));
        unsigned total_packet_length = PACKET_LENGTH_TYPE_ID_LENGTH + NUMBER_OF_SUB_PACKETS_LENGTH;
        for(unsigned i = 0; i < number_of_sub_packets; ++i) {
            auto[packet, packet_length] = extract_packet_data(packet_content.substr(total_packet_length));
            sub_packets.emplace_back(std::move(packet));
            total_packet_length += packet_length;
        }
        total_packet_length += PACKET_VERSION_LENGTH + PACKET_TYPE_ID_LENGTH;
        return { operator_packet_by_type_id(packet_type_id, packet_version, std::move(sub_packets)), total_packet_length };
    }

    static PacketData extract_operator_packet_with_length_type_id_zero_data(unsigned packet_version, unsigned packet_type_id, const std::string& packet_content) {
        std::vector<std::unique_ptr<Packet>> sub_packets{};
        unsigned total_sub_packets_length = converters::binary_to_decimal(packet_content.substr(PACKET_LENGTH_TYPE_ID_LENGTH, CONTENT_SIZE_IN_BITS_LENGTH));
        std::string sub_packets_as_binary = packet_content.substr(PACKET_LENGTH_TYPE_ID_LENGTH + CONTENT_SIZE_IN_BITS_LENGTH, total_sub_packets_length);
        while(contains_not_only_zeros(sub_packets_as_binary)) {
            auto[packet, packet_length] = extract_packet_data(sub_packets_as_binary);
            sub_packets.emplace_back(std::move(packet));
            sub_packets_as_binary = sub_packets_as_binary.substr(packet_length);
        }
        unsigned total_packet_length = PACKET_VERSION_LENGTH + PACKET_TYPE_ID_LENGTH + PACKET_LENGTH_TYPE_ID_LENGTH + CONTENT_SIZE_IN_BITS_LENGTH + total_sub_packets_length;
        return { operator_packet_by_type_id(packet_type_id, packet_version, std::move(sub_packets)), total_packet_length };
    }

    static PacketData extract_operator_packet_data(unsigned packet_version, unsigned packet_type_id, const std::string& packet_content) {
        return packet_content.at(0) == '1'
            ? extract_operator_packet_with_length_type_id_one_data(packet_version, packet_type_id, packet_content)
            : extract_operator_packet_with_length_type_id_zero_data(packet_version, packet_type_id, packet_content);
    }

};
