#include <cstdio>
#include <fstream>
#include <string>
#include <vector>
#include <cstdint>
#include <cstdlib>
#include <exception>

#include "json.hpp"

namespace {

void serializeString(std::ostream& out, const std::string& s) {
    uint32_t n = s.size();
    out.write((char *)&n, sizeof(n));
    out.write(s.data(), n);
}
bool deserializeString(std::istream& in, std::string& s) {
    uint32_t n;
    if (!in.read((char *)&n, sizeof(n)).fail()) {
        s.resize(n);
        return !in.read((char *)s.data(), s.size()).fail();
    }
    return false;
}

struct Answers {
    std::vector<std::string> answers;
    std::vector<int>         labels;
    void serialize(std::ostream& out) const {
        if (answers.size() != labels.size()) {
            throw std::runtime_error("Inconsistent number of answers and labels");
        }
        uint32_t n = answers.size();
        out.write((char *)&n, sizeof(n));
        for (auto& a : answers) {
            serializeString(out, a);
        }
        out.write((char *)labels.data(), labels.size()*sizeof(int));
    }
    bool deserialize(std::istream& in) {
        int n;
        in.read((char *)&n, sizeof(n));
        if (in.fail() || n < 0) {
            return false;
        }
        answers.resize(n);
        labels.resize(n);
        for (auto& a : answers) {
            if (!deserializeString(in, a)) return false;
        }
        in.read((char *)labels.data(), n*sizeof(int));
        return !in.fail();
    }
    nlohmann::json toJson() const {
        nlohmann::json o = nlohmann::json::object();
        o["answers"] = answers;
        o["labels"]  = labels;
        return o;
    }
};

struct MultiplChoice {
    std::string question;
    Answers singleCorrect;
    Answers multipleCorrect;
    void serialize(std::ostream& out) const {
        serializeString(out, question);
        singleCorrect.serialize(out);
        multipleCorrect.serialize(out);
    }
    bool deserialize(std::istream& in) {
        if (!deserializeString(in, question)) return false;
        return singleCorrect.deserialize(in) && multipleCorrect.deserialize(in);
    }
    nlohmann::json toJson() const {
        nlohmann::json o = nlohmann::json::object();
        o["question"] = question;
        o["single_correct"  ]  = singleCorrect.toJson();
        o["multiple_correct"]  = multipleCorrect.toJson();
        return o;
    }
    static nlohmann::json toJson(const std::vector<MultiplChoice>& data) {
        nlohmann::json o = nlohmann::json::array();
        for (auto& d : data) o.push_back(d.toJson());
        return o;
    }
    static std::vector<MultiplChoice> loadFromStream(std::istream& in) {
        uint32_t n;
        if (in.read((char *)&n, sizeof(n)).fail()) {
            printf("%s: failed reading number of entries\n", __func__);
            return {};
        }
        in.seekg(n*sizeof(uint32_t), std::ios::cur); // skip positions
        std::vector<MultiplChoice> result(n);
        int i = 0;
        for (auto& r : result) {
            ++i;
            if (!r.deserialize(in)) {
                printf("%s: failed reading data at question %d\n", __func__, i);
                return {};
            }
        }
        return result;
    }
    static std::vector<MultiplChoice> loadFromFile(const char* fileName) {
        std::ifstream in(fileName, std::ios::binary);
        if (!in) {
            printf("%s: failed to open %s\n", __func__, fileName);
            return {};
        }
        return loadFromStream(in);
    }
    static void serialize(std::ostream& out, const std::vector<MultiplChoice>& data) {
        uint32_t n = data.size();
        out.write((char *)&n, sizeof(n));
        if (data.empty()) return;
        std::vector<uint32_t> pos(data.size(), 0);
        out.write((char *)pos.data(), pos.size()*sizeof(pos[0]));
        int i = 0;
        for (auto& d : data) {
            pos[i++] = out.tellp();
            d.serialize(out);
        }
        out.seekp(sizeof(n), std::ios::beg);
        out.write((char *)pos.data(), pos.size()*sizeof(pos[0]));
    }
    static void serialize(const char* fileName, const std::vector<MultiplChoice>& data) {
        std::ofstream out(fileName, std::ios::binary);
        if (!out) {
            printf("%s: failed to open %s for writing\n", __func__, fileName);
            return;
        }
        serialize(out, data);
    }
};
}


int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s file [output_file]\n", argv[0]);
        return 1;
    }

    auto data = MultiplChoice::loadFromFile(argv[1]);
    printf("Loaded %zu datasets from %s\n", data.size(), argv[1]);

    auto json = MultiplChoice::toJson(data);

    auto ofile = argc > 2 ? std::string{argv[2]} : std::string{argv[1]} + ".json";
    std::ofstream out(ofile.c_str());
    out << json << std::endl;

    return 0;
}
