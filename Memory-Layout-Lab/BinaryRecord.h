#pragma once
#include "EndianUtils.h"

#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <vector>

struct Record
{
    std::uint8_t type;
    std::uint32_t sequence;
    std::vector<std::byte> payload;
};

namespace BinaryFormat
{
    inline constexpr std::byte MAGIC_0{ 0x4D };
    inline constexpr std::byte MAGIC_1{ 0x4C };
    inline constexpr std::byte VERSION{ 0x01 };

    inline constexpr std::size_t HEADER_SIZE = 10;
}

inline constexpr std::size_t HEADER_SIZE = 10;

inline std::optional<std::vector<std::byte>>
EncodeRecord(const Record& record)
{
    if (record.payload.size() >
        std::numeric_limits<std::uint16_t>::max())
    {
        return std::nullopt;
    }

    std::vector<std::byte> result;
    result.reserve(HEADER_SIZE + record.payload.size());

    result.push_back(BinaryFormat::MAGIC_0);
    result.push_back(BinaryFormat::MAGIC_1);
    result.push_back(BinaryFormat::VERSION);

    std::byte type = static_cast<std::byte>(record.type);
    result.push_back(type);

    auto sequenceBytes = EncodeBigEndian(record.sequence);
    result.insert(result.end(), sequenceBytes.begin(), sequenceBytes.end());

    std::array<std::byte, 2> payloadLengthBytes
    {
        std::byte((record.payload.size() >> 8) & 0xFF),
        std::byte(record.payload.size() & 0xFF)
    };

    result.insert(result.end(), payloadLengthBytes.begin(), payloadLengthBytes.end());
    result.insert(result.end(), record.payload.begin(), record.payload.end());

    return result;
}

inline std::optional<Record>
DecodeRecord(std::span<const std::byte> bytes)
{
    Record result{};

    if (bytes.size() < HEADER_SIZE)
    {
        return std::nullopt;
    }

    if (bytes[0] != BinaryFormat::MAGIC_0 ||
        bytes[1] != BinaryFormat::MAGIC_1 ||
        bytes[2] != BinaryFormat::VERSION)
    {
        return std::nullopt;
    }

    result.type = std::to_integer<std::uint8_t>(bytes[3]);

    auto sequenceBytes =
        DecodeBigEndian(std::array<std::byte, 4>{
        bytes[4], bytes[5], bytes[6], bytes[7]});

    result.sequence = sequenceBytes;

    std::uint16_t payloadLength =
        (std::to_integer<std::uint16_t>(bytes[8]) << 8) |
        std::to_integer<std::uint16_t>(bytes[9]);

    auto remaining = bytes.size() - HEADER_SIZE;
    if (payloadLength != remaining)
    {
        return std::nullopt;
    }

    auto payloadBytes =
        bytes.subspan(HEADER_SIZE, payloadLength);

    result.payload.assign(
        payloadBytes.begin(),
        payloadBytes.end());

    return result;
}