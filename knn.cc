#include "common.cc"
#include <algorithm>
#include <functional>
#include <ostream>
#include <stdexcept>
#include <zlib.h>

string deflate_string(string &input) {
    z_stream zs;
    memset(&zs, 0, sizeof(zs));
    if (deflateInit(&zs, Z_BEST_COMPRESSION) != Z_OK) {
        throw std::runtime_error("deflate init failed");
    }
    zs.next_in = (Bytef*) input.data();
    zs.avail_in = (uInt) input.size();
    int ret;
    char buffer[32768];
    string result;
    do {
        zs.next_out = (Bytef*) buffer;
        zs.avail_out = sizeof(buffer);

        ret = deflate(&zs, Z_FINISH);
        if(result.size() < zs.total_out) {
            result.append(buffer, zs.total_out - result.size());
        }

    } while(ret == Z_OK);
    deflateEnd(&zs);
    if (ret != Z_STREAM_END) {
        throw std::runtime_error("Error while compressing data");
    }

    return result;
}

string inflate_string(string& input) {
    z_stream zs;
    memset(&zs, 0, sizeof(zs));
    if (inflateInit(&zs) != Z_OK) {
        throw std::runtime_error("inflate init failed");
    }
    zs.next_in = (Bytef*) input.data();
    zs.avail_in = (uInt) input.size();
    int ret;
    char buffer[32768];
    string result;
    do {
        zs.next_out = (Bytef*) buffer;
        zs.avail_out = sizeof(buffer);

        ret = inflate(&zs, 0);
        if(result.size() < zs.total_out) {
            result.append(buffer, zs.total_out - result.size());
        }

    } while(ret == Z_OK);
    inflateEnd(&zs);
    if (ret != Z_STREAM_END) {
        throw std::runtime_error("Error while decmpressing data");
    }

    return result;

}

struct Sample {
    int klazz;
    string text;
    string ztext;
};
struct NCD {
    int klazz;
    float distance;
};
#define loop(i, n) for(int i = 0 ; i < n; i++)
array<string,5> klazz_names = {"null", "World", "Sport", "Business", "Sci/Tech"};

ostream& operator<< (ostream& os,
                     const NCD & ncd) {

    cout << klazz_names[ncd.klazz] << ": " << ncd.distance;
    return os;
}

ostream& operator<< (ostream& os,
                     const Sample& sample) {

    cout << klazz_names[sample.klazz] << ": " << sample.text;
    return os;
}

bool operator< (NCD& lhs, NCD& rhs) {
    return lhs.distance < rhs.distance;
}

float ncd(Sample &a, string &b) {
    string ab= a.text + b;
    float al = a.ztext.length();
    float bl = deflate_string(b).length();
    float abl = deflate_string(ab).length();
    float ret = (abl - fmin(al, bl))/fmax(al, bl);
    return ret;
}

int klassify(vector<Sample> &samples, string text, int k) {
    vector<NCD> ncds;
    for (auto sample: samples) {
        NCD item = {
        .klazz = sample.klazz,
        .distance = ncd(sample, text)
        };
        ncds.push_back(item);
    }
    std::sort(ncds.begin(), ncds.end());
    int klazz[5] = {0};
    loop(i, k) {
        klazz[ncds[i].klazz]++;
        cout << ncds[i] << endl;
    }

    int ret = 0;
    int max = 0;
    loop(i, k) {
        if (klazz[i] > max) {
            max = klazz[i];
            ret = i;
        }
//        cout << klazz[i] << endl;
    }
    assert (ret > 0);
    return ret;

}

void load_samples(vector<Sample> & samples, vector<vector<string>> &table) {

    for (int i = 0; i < table.size(); i++) {
        table[i][0]= table[i][0].substr(9);
        size_t cat = std::stoi(table[i][0]);
        Sample sample;
        sample.text = table[i][1];
        sample.klazz= cat;
        sample.ztext = deflate_string(sample.text);
        samples.push_back(sample);
    }
}

int main(int argc, char **argv) {
    auto train = readcsv("data/ag_news.train", 2);
    size_t klazz[5] = {0};

    vector<Sample> train_set;
    load_samples(train_set, train);
//    string b = "blah blah blah zzzzzz";
//    cout << ncd(train_set[0], b) << endl;

    // read 50 samples
    auto test = readcsv("data/ag_news.test",2,50);

    vector<Sample> test_set;
    load_samples(test_set, test);
    cout << train_set.size() << endl;
    loop(i, 8) {
        string &test_text = test_set[i].text;
        int pred_clz = klassify(train_set, test_text, 5);
        int actual_clz = test_set[i].klazz;

        cout << "predicted: " << klazz_names[pred_clz] << "-----> actual: " << klazz_names[actual_clz] << endl;
        cout << test_set[i].text << endl << endl;
    }
//    cout << "orig: " << hello << endl;
//    auto zipped = deflate_string(hello);
//    cout << "zipped: " << zipped << "  == " << zipped.length() << endl;
//    cout << "back: " << inflate_string(zipped) << "  == "<< hello.length() << endl;

}
