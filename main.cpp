#include <bits/stdc++.h>
#include <boost/multiprecision/cpp_int.hpp>
using namespace std;
using boost::multiprecision::cpp_int;

// Convert string from arbitrary base to cpp_int
cpp_int convertFromBase(const string& value, int base) {
    cpp_int result = 0;
    for (char digit : value) {
        int digitValue;
        if (digit >= '0' && digit <= '9') digitValue = digit - '0';
        else if (digit >= 'a' && digit <= 'z') digitValue = digit - 'a' + 10;
        else if (digit >= 'A' && digit <= 'Z') digitValue = digit - 'A' + 10;
        else throw invalid_argument("Invalid digit in number");

        if (digitValue >= base) throw invalid_argument("Digit exceeds base");
        result = result * base + digitValue;
    }
    return result;
}

// Lagrange interpolation constant term (at x=0)
cpp_int lagrangeInterpolationConstant(const vector<pair<int, cpp_int>>& points, int k) {
    cpp_int result = 0;
    vector<pair<int, cpp_int>> selected(points.begin(), points.begin() + k);

    for (int i = 0; i < k; i++) {
        cpp_int term = selected[i].second; // y_i
        cpp_int num = 1, den = 1;
        for (int j = 0; j < k; j++) {
            if (i != j) {
                num *= (0 - selected[j].first);
                den *= (selected[i].first - selected[j].first);
            }
        }
        term *= num;
        term /= den; // exact division
        result += term;
    }
    return result;
}

// Very simple JSON parser for this format
map<string, string> parseSimpleJSON(const string& jsonContent) {
    map<string, string> result;
    string cleaned;
    for (char c : jsonContent)
        if (!isspace(c)) cleaned += c;

    size_t pos = cleaned.find("\"keys\"");
    if (pos != string::npos) {
        size_t nPos = cleaned.find("\"n\":", pos);
        if (nPos != string::npos) {
            size_t start = cleaned.find(":", nPos) + 1;
            size_t end = cleaned.find(",", start);
            if (end == string::npos) end = cleaned.find("}", start);
            result["n"] = cleaned.substr(start, end - start);
        }
        size_t kPos = cleaned.find("\"k\":", pos);
        if (kPos != string::npos) {
            size_t start = cleaned.find(":", kPos) + 1;
            size_t end = cleaned.find("}", start);
            result["k"] = cleaned.substr(start, end - start);
        }
    }
    // extract points
    for (int i = 1; i <= 20; i++) {
        string key = "\"" + to_string(i) + "\"";
        size_t keyPos = cleaned.find(key + ":");
        if (keyPos != string::npos) {
            size_t basePos = cleaned.find("\"base\":", keyPos);
            if (basePos != string::npos) {
                size_t baseStart = cleaned.find("\"", basePos + 7) + 1;
                size_t baseEnd = cleaned.find("\"", baseStart);
                result[to_string(i) + "_base"] = cleaned.substr(baseStart, baseEnd - baseStart);
            }
            size_t valuePos = cleaned.find("\"value\":", keyPos);
            if (valuePos != string::npos) {
                size_t valueStart = cleaned.find("\"", valuePos + 8) + 1;
                size_t valueEnd = cleaned.find("\"", valueStart);
                result[to_string(i) + "_value"] = cleaned.substr(valueStart, valueEnd - valueStart);
            }
        }
    }
    return result;
}

void processTestCase(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error opening " << filename << endl;
        return;
    }
    string jsonContent((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();

    auto data = parseSimpleJSON(jsonContent);
    if (data.find("n") == data.end() || data.find("k") == data.end()) {
        cout << "Error: missing n or k in " << filename << endl;
        return;
    }
    int n = stoi(data["n"]);
    int k = stoi(data["k"]);

    cout << "\nProcessing " << filename << " ..." << endl;
    cout << "n=" << n << ", k=" << k << endl;

    vector<pair<int, cpp_int>> points;
    for (int i = 1; i <= n; i++) {
        string baseKey = to_string(i) + "_base";
        string valueKey = to_string(i) + "_value";
        if (data.find(baseKey) != data.end() && data.find(valueKey) != data.end()) {
            int base = stoi(data[baseKey]);
            string value = data[valueKey];
            try {
                cpp_int y = convertFromBase(value, base);
                points.push_back({i, y});
                cout << "Point " << i << ": (" << i << "," << y << ") [base " << base << ": " << value << "]" << endl;
            } catch (exception& e) {
                cout << "Error at point " << i << ": " << e.what() << endl;
            }
        }
    }

    if ((int)points.size() < k) {
        cout << "Not enough points to interpolate (need " << k << ", got " << points.size() << ")" << endl;
        return;
    }
    sort(points.begin(), points.end());
    cpp_int secret = lagrangeInterpolationConstant(points, k);
    cout << "Secret (constant term) = " << secret << endl;
}

int main() {
    cout << "Shamir's Secret Sharing - Constant Finder" << endl;
    processTestCase("testcase1.json");
    processTestCase("testcase2.json");
    return 0;
}
