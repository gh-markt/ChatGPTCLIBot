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
#include "modelparams.h"
#include "emdedded_resource_reader.h"
#include <stdexcept>

// ModelParams constructor and member functions

ModelParams::ModelParams(int explicit_n_vocab,
                         const std::string& pat_str,
                         const std::unordered_map<std::vector<uint8_t>, int, VectorHash>& mergeable_ranks,
                         const std::unordered_map<std::string, int>& special_tokens)
        : explicit_n_vocab_(explicit_n_vocab),
          pat_str_(pat_str),
          mergeable_ranks_(mergeable_ranks),
          special_tokens_(special_tokens) {}

int ModelParams::explicit_n_vocab() const { return explicit_n_vocab_; }

std::string ModelParams::pat_str() const { return pat_str_; }

std::unordered_map<std::vector<uint8_t>, int, VectorHash> ModelParams::mergeable_ranks() const {
    return mergeable_ranks_;
}

std::unordered_map<std::string, int> ModelParams::special_tokens() const { return special_tokens_; }

// ModelParamsGenerator member functions

ModelParams ModelParamsGenerator::get_model_params(LanguageModel model) {
    switch(model) {
        case LanguageModel::CL100K_BASE:
            return cl100k_base();
        case LanguageModel::P50K_BASE:
            return p50k_base();
        case LanguageModel::P50K_EDIT:
            return p50k_edit();
        case LanguageModel::R50K_BASE:
            return r50k_base();
    }
    throw std::runtime_error("Invalid argument to get_model_params");
}

#if 0
// latin alphabet only
static auto constexpr cl100k_pattern = "(?:'s|'t|'re|'ve|'m|'ll|'d)|[^\\r\\na-zA-Z0-9]?[a-zA-Z]+|[0-9]{1,3}| ?[^\\sa-zA-Z0-9]+[\\r\\n]*|\\s*[\\r\\n]+|\\s+(?!\\S)|\\s+";
static auto constexpr p50k_pattern = "'s|'t|'re|'ve|'m|'ll|'d| ?[a-zA-Z]+| ?[0-9]+| ?[^\\sa-zA-Z0-9]+|\\s+(?!\\S)|\\s+";
#else
// original regexes don't work with std::regex.  Need to come up with another solution to support other languages.
static auto constexpr cl100k_pattern = "(?i:'s|'t|'re|'ve|'m|'ll|'d)|[^\\r\\n\\p{L}\\p{N}]?\\p{L}+|\\p{N}{1,3}| ?[^\\s\\p{L}\\p{N}]+[\\r\\n]*|\\s*[\\r\\n]+|\\s+(?!\\S)|\\s+";
static auto constexpr p50k_pattern = "'s|'t|'re|'ve|'m|'ll|'d| ?\\p{L}+| ?\\p{N}+| ?[^\\s\\p{L}\\p{N}]+|\\s+(?!\\S)|\\s+";
#endif
ModelParams ModelParamsGenerator::r50k_base() {
    auto mergeableRanks = EmbeddedResourceReader::loadTokenBytePairEncoding("r50k_base.tiktoken");

    return ModelParams(
            50257,
            p50k_pattern,
            mergeableRanks,
            {{EndOfText, 50256}}
    );
}

ModelParams ModelParamsGenerator::p50k_base() {
    auto mergeableRanks = EmbeddedResourceReader::loadTokenBytePairEncoding("p50k_base.tiktoken");

    return ModelParams(
            50281,
            p50k_pattern,
            mergeableRanks,
            {{EndOfText, 50256}}
    );
}

ModelParams ModelParamsGenerator::p50k_edit() {
    auto mergeableRanks = EmbeddedResourceReader::loadTokenBytePairEncoding("p50k_base.tiktoken");

    std::unordered_map<std::string, int> specialTokens = {
            {EndOfText, 50256},
            {FimPrefix, 50281},
            {FimMiddle, 50282},
            {FimSuffix, 50283}
    };

    return ModelParams(
            0,
            p50k_pattern,
            mergeableRanks,
            specialTokens
    );
}

ModelParams ModelParamsGenerator::cl100k_base() {
    auto mergeableRanks = EmbeddedResourceReader::loadTokenBytePairEncoding("cl100k_base.tiktoken");
    std::unordered_map<std::string, int> specialTokens = {
            {EndOfText, 100257},
            {FimPrefix, 100258},
            {FimMiddle, 100259},
            {FimSuffix, 100260},
            {EndOfPrompt, 100276}
    };

    return ModelParams(
            0,
            cl100k_pattern,
            mergeableRanks,
            specialTokens
    );
}
