//
// Created by v2ray on 2023/4/9.
//

#include "TokenUtils.h"

namespace util {
    unordered_map<LanguageModel, shared_ptr<GptEncoding>> tokenizer_cache;

    //class max_tokens_exceeded start:
    max_tokens_exceeded::max_tokens_exceeded() : max_tokens_exceeded("Max tokens exceeded.") {}
    max_tokens_exceeded::max_tokens_exceeded(string message) : message(std::move(message)) {}
    max_tokens_exceeded::~max_tokens_exceeded() = default;

    const char* max_tokens_exceeded::what() const noexcept {
        return message.c_str();
    }
    //class max_tokens_exceeded end.

    shared_ptr<GptEncoding> get_enc_cache(LanguageModel model) {
        if (tokenizer_cache.contains(model)) {
            return tokenizer_cache[model];
        }
        auto encoding = GptEncoding::get_encoding(model);
        tokenizer_cache[model] = encoding;
        return encoding;
    }

    unsigned int get_token_count(const json& messages, const string& model_name) {
        if (!messages.is_array()) {
            throw invalid_argument("Messages must be an json array.");
        }
        auto encoder = get_enc_cache(get_tokenizer(model_name));
        int8_t tokens_per_message = 5; //Every message follows: <|start|>{role/name}\n{content}<|end|>\n
        int8_t tokens_per_name = -1; //If there's a name, the role is omitted.
        if (boost::starts_with(model_name, "gpt-4")) {
            tokens_per_message = 4;
            tokens_per_name = 1;
        }
        unsigned int token_count = 3; //Chat completion models will always have 3 tokens at the end.
        for (const auto& message : messages) {
            if (!message.is_object()) {
                throw invalid_argument("Message must be an json object.");
            }
            if (!message.contains("content") || !message["content"].is_string()) {
                throw invalid_argument("Message must contain a valid content field.");
            }
            token_count += encoder->encode(message["content"].get<string>()).size();
            if (message.contains("name") && message["name"].is_string()) {
                token_count += encoder->encode(message["name"].get<string>()).size() + tokens_per_name;
            }
            token_count += tokens_per_message;
        }
        return token_count;
    }

    unsigned int get_token_count(const string& text, const string& model_name) {
        return get_enc_cache(get_tokenizer(model_name))->encode(text).size();
    }

    LanguageModel get_tokenizer(const string& model_name) {
        static const vector<pair<string, LanguageModel>> prefix_to_tokenizer = {
                //Chat completion models:
                {"gpt-4", LanguageModel::CL100K_BASE},
                {"gpt-3.5-turbo", LanguageModel::CL100K_BASE},
                //Text completion models:
                {"text-davinci-003", LanguageModel::P50K_BASE},
                {"text-davinci-002", LanguageModel::P50K_BASE},
                {"text-davinci-001", LanguageModel::R50K_BASE},
                {"text-curie-001", LanguageModel::R50K_BASE},
                {"text-babbage-001", LanguageModel::R50K_BASE},
                {"text-ada-001", LanguageModel::R50K_BASE},
                {"davinci", LanguageModel::R50K_BASE},
                {"curie", LanguageModel::R50K_BASE},
                {"babbage", LanguageModel::R50K_BASE},
                {"ada", LanguageModel::R50K_BASE},
                //Code completion models:
                {"code-davinci-00", LanguageModel::P50K_BASE},
                {"code-cushman-00", LanguageModel::P50K_BASE},
                //Text edit models:
                {"text-davinci-edit-001", LanguageModel::P50K_EDIT},
                {"code-davinci-edit-001", LanguageModel::P50K_EDIT},
                //Embedding models:
                {"text-embedding-ada-002", LanguageModel::CL100K_BASE},
                //Old embedding models:
                {"text-similarity-", LanguageModel::R50K_BASE},
                {"text-search-", LanguageModel::R50K_BASE},
                {"code-search-", LanguageModel::R50K_BASE}};
        for (const auto& [prefix, tokenizer] : prefix_to_tokenizer) {
            if (boost::starts_with(model_name, prefix)) {
                return tokenizer;
            }
        }
        throw invalid_argument("Invalid model name: " + model_name);
    }

    unsigned int get_max_tokens(const string& model_name) {
        static const vector<pair<string, unsigned int>> prefix_to_max_tokens = {
                //Chat completion models:
                {"gpt-4-32k", 32767},
                {"gpt-4", 8191},
                {"gpt-3.5-turbo", 4095},
                //Text completion models:
                {"text-davinci-003", 4095},
                {"text-davinci-002", 4095},
                {"text-davinci-001", 2047},
                {"text-curie-001", 2047},
                {"text-babbage-001", 2047},
                {"text-ada-001", 2047},
                {"davinci", 2047},
                {"curie", 2047},
                {"babbage", 2047},
                {"ada", 2047},
                //Code completion models:
                {"code-davinci-00", 8000},
                {"code-cushman-00", 2047},
                //Text edit models:
                {"text-davinci-edit-001", 2047},
                {"code-davinci-edit-001", 2047},
                //Embedding models:
                {"text-embedding-ada-002", 8190},
                //Old embedding models:
                {"text-similarity-", 2045},
                {"text-search-", 2045},
                {"code-search-", 2045}};
        for (const auto& [prefix, max_tokens] : prefix_to_max_tokens) {
            if (boost::starts_with(model_name, prefix)) {
                return max_tokens;
            }
        }
        throw invalid_argument("Invalid model name: " + model_name);
    }
} // util