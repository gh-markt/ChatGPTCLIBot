/*
 * Copyright (c) 2023 by Mark Tarrabain All rights reserved. Redistribution and use in source and binary forms,
 * with or without modification, are permitted provided that the following conditions are met:
 * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
 * disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following
 * disclaimer in the documentation and/or other materials provided with the distribution.
 * Neither the name of the nor the names of its contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "emdedded_resource_reader.h"
#include "encoding_utils.h"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <filesystem>

std::vector<std::string> EmbeddedResourceReader::readEmbeddedResourceAsLines(const std::string& resourceName) {
    std::filesystem::path resource_path = std::filesystem::path("tokenizers") / resourceName;
    std::ifstream file(resource_path);
    if (!file.is_open()) {
        throw std::runtime_error("Embedded resource '" + resource_path.string() + "' not found.");
    }

    std::string line;
    std::vector<std::string> lines;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }

    return lines;
}

std::unordered_map<std::vector<uint8_t>, int, VectorHash> EmbeddedResourceReader::loadTokenBytePairEncoding(const std::string& dataSourceName) {
    auto lines = readEmbeddedResourceAsLines(dataSourceName);
    std::unordered_map<std::vector<uint8_t>, int, VectorHash> token_byte_pair_encoding;

    for (const auto& line : lines) {
        if (!line.empty()) {
            std::istringstream iss(line);
            std::string base64_string;
            int rank;

            iss >> base64_string >> rank;
            auto decoded = base64::decode(base64_string);
            std::vector<uint8_t> decoded_bytes(decoded.begin(), decoded.end());

            token_byte_pair_encoding[decoded_bytes] = rank;
        }
    }

    return token_byte_pair_encoding;
}
