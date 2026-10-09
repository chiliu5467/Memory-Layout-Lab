# Memory-Layout-Lab

A C++20 learning project exploring memory layout, object representation, endianness, and portable binary serialization.

This project demonstrates how C++ objects are represented in memory and how structured data can be safely encoded into and decoded from a custom binary protocol.

The main focus is on **data representation, portability, bounds validation, and defensive programming**.

## Features

### 1. Memory Layout Inspection

- Inspect object sizes using `sizeof`.
- Examine alignment requirements using `alignof`.
- Compare struct layouts and padding.
- Inspect member offsets using `offsetof`.
- Understand how member ordering can affect memory usage.

### 2. Object Representation

- Inspect raw object representations using `std::byte`.
- Access contiguous memory through `std::span`.
- Use `std::as_bytes` to obtain a read-only byte view.
- Observe how values are represented in memory.

### 3. Endianness

- Detect native byte order using `std::endian`.
- Encode 32-bit unsigned integers into big-endian byte sequences.
- Decode big-endian byte sequences into `std::uint32_t`.
- Understand why portable binary formats require an explicitly defined byte order.

### 4. Binary Record Serialization

Implement a simple binary protocol supporting:

- Magic bytes validation
- Protocol version validation
- Record type and sequence number
- Variable-length payloads
- Big-endian integer encoding
- Payload length validation
- Safe decoding with `std::span`
- Error reporting using `std::optional`

The encoder and decoder process individual fields instead of copying the raw memory representation of a C++ struct.

## Project Structure

```text
Memory-Layout-Lab/
├── Memory-Layout-Lab.sln
├── Memory-Layout-Lab/
│   ├── main.cpp
│   ├── LayoutTypes.h
│   ├── MemoryLayout.h
│   ├── EndianUtils.h
│   ├── BinaryRecord.h
│   └── Memory-Layout-Lab.vcxproj
└── .gitignore
```

### File Responsibilities

| File | Responsibility |
|---|---|
| `LayoutTypes.h` | Example structs for memory layout comparison |
| `MemoryLayout.h` | Type information and object byte inspection |
| `EndianUtils.h` | Native endianness detection and big-endian conversion |
| `BinaryRecord.h` | Binary record structure, encoding, decoding, and validation |
| `main.cpp` | Demonstrations and test cases |

## Binary Protocol Specification

The project defines a custom binary record format (Version 1).

All multi-byte integers are serialized in **big-endian byte order**, independent of the host machine's native endianness.

### Record Structure

```cpp
struct Record
{
    std::uint8_t type;
    std::uint32_t sequence;
    std::vector<std::byte> payload;
};
```

### Binary Format

| Offset | Size | Field | Description |
|---|---|---|---|
| 0 | 2 bytes | Magic | Fixed value `4D 4C` |
| 2 | 1 byte | Version | Protocol version `01` |
| 3 | 1 byte | Type | Record type |
| 4 | 4 bytes | Sequence | Big-endian uint32 |
| 8 | 2 bytes | Payload Length | Big-endian uint16 |
| 10 | Variable | Payload | Actual binary data |

**Header size:** 10 bytes

**Maximum payload size:** 65,535 bytes

### Example

Input:

```cpp
Record record{
    2,
    0x12345678,
    {
        std::byte{0xAA},
        std::byte{0xBB},
        std::byte{0xCC}
    }
};
```

Expected encoded output:

```text
4D 4C 01 02 12 34 56 78 00 03 AA BB CC
```

Explanation:

```text
4D 4C        Magic Bytes
01           Version
02           Type
12 34 56 78  Sequence
00 03        Payload Length
AA BB CC     Payload
```

Total serialized size: **13 bytes**

## Encoding and Decoding

### Encoding

`EncodeRecord()` converts a `Record` into a binary byte sequence.

```cpp
std::optional<std::vector<std::byte>>
EncodeRecord(const Record& record);
```

The encoder:

1. Validates the payload size.
2. Writes magic bytes and protocol version.
3. Encodes the record type.
4. Serializes the sequence number in big-endian order.
5. Encodes the payload length.
6. Appends the payload bytes.

Payloads exceeding the maximum supported size are rejected using `std::nullopt`.

### Decoding

`DecodeRecord()` reconstructs a `Record` from a byte buffer.

```cpp
std::optional<Record>
DecodeRecord(std::span<const std::byte> bytes);
```

The decoder:

1. Validates the minimum header size.
2. Verifies magic bytes and protocol version.
3. Decodes the record type and sequence number.
4. Reads the declared payload length.
5. Validates that the remaining buffer size exactly matches the declared payload length.
6. Creates a payload subview only after bounds validation.
7. Copies the payload into the resulting `Record`.

Malformed records return `std::nullopt`.

### Defensive Parsing

The decoder validates input sizes before accessing variable-length data.

```cpp
if (bytes.size() < BinaryFormat::HEADER_SIZE)
{
    return std::nullopt;
}

auto remaining =
    bytes.size() - BinaryFormat::HEADER_SIZE;

if (payloadLength != remaining)
{
    return std::nullopt;
}

auto payloadBytes =
    bytes.subspan(
        BinaryFormat::HEADER_SIZE,
        payloadLength
    );
```

This prevents out-of-bounds access caused by truncated or inconsistent binary records.

The current protocol expects exactly one complete record per input buffer and rejects unexpected trailing bytes.

## Testing

The project includes positive and negative test cases.

| Test | Expected Behavior |
|---|---|
| Valid record encoding | Produces expected binary output |
| Valid record decoding | Restores original record fields |
| Fixed expected bytes | Matches the protocol specification |
| Invalid header | Rejects truncated input |
| Wrong magic bytes | Rejects invalid magic values |
| Invalid payload length | Rejects inconsistent payload length |
| Empty payload | Successfully encodes and decodes |
| Unsupported version | Rejects unsupported protocol versions |
| Trailing bytes | Rejects unexpected extra data |
| Oversized payload | Rejects payloads larger than 65,535 bytes |

### Round-Trip Testing

A round-trip test verifies that a record can be encoded and decoded without losing field values.

```text
Original Record
      |
      v
 EncodeRecord()
      |
      v
  Binary Bytes
      |
      v
 DecodeRecord()
      |
      v
 Reconstructed Record
```

The reconstructed record is compared against the original type, sequence number, and payload.

### Fixed Specification Testing

Round-trip tests alone cannot guarantee that the serialized format matches the protocol specification.

For example, if both the encoder and decoder incorrectly use little-endian byte order, the round-trip test might still succeed.

Therefore, the project also compares the actual encoded bytes against a fixed expected byte sequence.

### Negative Testing

Malformed inputs are intentionally supplied to verify that invalid records are rejected.

Tests use a custom `Check()` helper that throws `std::runtime_error` when an expected condition is not met.

This distinguishes an expected parsing failure from an actual test failure.

## Build and Run

### Requirements

- C++20-compatible compiler
- Visual Studio 2022
- MSVC toolchain
- Windows

### Instructions

1. Clone the repository:

   ```bash
   git clone https://github.com/chiliu5467/Memory-Layout-Lab.git
   ```

2. Open `Memory-Layout-Lab.sln` in Visual Studio 2022.

3. Select the `Debug` or `Release` configuration.

4. Build the solution.

5. Run the console application.

The application displays memory layout information, byte representations, endianness demonstrations, and test results.

When all tests succeed, the program prints:

```text
All tests passed!
```

## Design Decisions

### Why Not Serialize Raw Struct Memory?

Directly copying the memory representation of a C++ struct can introduce portability problems due to:

- Compiler-dependent padding and alignment
- Native byte order differences
- Platform-dependent type sizes
- Pointer values and dynamically allocated resources
- Differences between in-memory layout and serialized representation

Field-by-field serialization provides explicit control over the binary format.

### Why Use `std::span`?

`std::span` provides a lightweight, non-owning view over contiguous memory.

It allows the decoder to access an input buffer without copying the entire buffer first.

However, `std::span` does not own the underlying memory. The caller must ensure the buffer remains valid during decoding.

### Why Use `std::optional`?

`std::optional` provides a simple way to represent whether encoding or decoding produced a valid result.

Malformed input is treated as an expected failure case rather than requiring an exception for every invalid record.

### Why Use Big-Endian?

The protocol specifies big-endian encoding to ensure multi-byte integer fields have a consistent representation across platforms.

The serialized data does not depend on the host machine's native byte order.

## Current Limitations

This is an educational binary protocol implementation, not a production-ready networking library.

Current limitations include:

- Only protocol Version 1 is supported.
- Input buffers must contain exactly one complete record.
- Payloads are limited to 65,535 bytes.
- No checksum or integrity verification is implemented.
- Parsing failures do not provide detailed error categories.
- The decoder does not support streaming or incremental parsing.

Possible future improvements include multi-version protocol support, structured error reporting, checksum validation, and fuzz testing.

## What I Learned

Through this project, I gained practical experience with:

- C++ object representation and memory alignment
- Struct padding and member ordering
- `std::byte` and `std::span`
- Endianness and fixed-width integer types
- Portable binary serialization
- Bounds checking and defensive parsing
- `std::optional` for error reporting
- Positive and negative testing
- Protocol validation and backward compatibility considerations
- Organizing C++ code into focused, reusable headers

The most important takeaway is that **an object's in-memory representation is not necessarily a portable serialization format**.

By explicitly defining the binary protocol and validating input before accessing it, the implementation becomes more predictable, portable, and robust.

## Tech Stack

- **Language:** C++20
- **IDE:** Visual Studio 2022
- **Compiler:** MSVC
- **Standard Library:** `std::vector`, `std::array`, `std::span`, `std::byte`, `std::optional`, `std::endian`

## Project Context

This project was developed as part of a 13-week C++ career development roadmap.

**Week 9 — Systems Programming Fundamentals**

The goal was to move beyond basic C++ syntax and explore concepts commonly used in system-level software development, including memory representation, binary formats, data portability, and safe parsing.
