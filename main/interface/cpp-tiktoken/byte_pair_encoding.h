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
#pragma once

#include "encoding_utils.h"
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <memory>
#include "pcre2_regex.h"

class BytePairEncodingCore {
public:
    BytePairEncodingCore(const std::unordered_map<std::vector<uint8_t>, int, VectorHash>& byte_pair_ranks,
                         const std::unordered_map<std::string, int>& special_token_mappings,
                         const std::shared_ptr<PCRERegex>& pattern_string);

    template <typename T>
    std::vector<T> byte_pair_merge(const std::vector<uint8_t>& piece,
                                   const std::unordered_map<std::vector<uint8_t>, int, VectorHash>& ranks,
                                   std::function<T(int, int)> f);
    std::pair<std::vector<int>, std::vector<int>> encode_native(const std::string& line_to_encode,
                                                                const std::unordered_set<std::string>& allowed_special);
    std::string decode_native(const std::vector<int>& input_tokens_to_decode);

private:
    std::unordered_map<std::vector<uint8_t>, int, VectorHash> byte_pair_ranks_;
    std::unordered_map<std::string, int> special_token_mappings_;
    std::shared_ptr<PCRERegex> pattern_string_;
};
