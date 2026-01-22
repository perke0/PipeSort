#include "pipe_sort.h"
#include <bits/stdc++.h>
using namespace std;

struct BigInt {
    vector<uint64_t> limbs;
    bool operator<(const BigInt& other) const {
        for (size_t i = 0; i < limbs.size(); i++) {
            if (limbs[i] != other.limbs[i]) return limbs[i] < other.limbs[i];
        }
        return false;
    }
};

// Generate random numbers
void generate_random(vector<BigInt>& data, mt19937_64& rng) {
    for (auto &x : data) {
        for (auto &limb : x.limbs)
            limb = rng();
    }
}

// Generate skewed numbers (most high bits same)
void generate_skewed(vector<BigInt>& data, mt19937_64& rng) {
    uint64_t high = rng() & 0xFFFF'FFFF'0000'0000ULL; // fixed high bits
    for (auto &x : data) {
        x.limbs[0] = high;
        for (size_t i = 1; i < x.limbs.size(); i++)
            x.limbs[i] = rng();
    }
}

void benchmark(size_t N, size_t LIMBS, bool skewed = false) {
    cout << "\n=== Benchmark N=" << N << " LIMBS=" << LIMBS
         << (skewed ? " (skewed)" : " (random)") << " ===\n";

    vector<BigInt> data(N);
    for (auto &x : data) x.limbs.resize(LIMBS);

    mt19937_64 rng(1234);
    if (skewed) generate_skewed(data, rng);
    else generate_random(data, rng);

    // Flatten array
    vector<uint64_t> arr(N * LIMBS);
    for (size_t i = 0; i < N; i++)
        copy(data[i].limbs.begin(), data[i].limbs.end(), arr.begin() + i * LIMBS);

    int max_bit = get_max_bit(arr.data(), N, LIMBS);

    auto t1 = chrono::high_resolution_clock::now();
    pipe_sort(arr.data(), N, LIMBS, max_bit);
    auto t2 = chrono::high_resolution_clock::now();

    vector<BigInt> data_sorted = data;
    auto t3 = chrono::high_resolution_clock::now();
    sort(data_sorted.begin(), data_sorted.end());
    auto t4 = chrono::high_resolution_clock::now();

    double pipe_time = chrono::duration<double>(t2 - t1).count();
    double std_time = chrono::duration<double>(t4 - t3).count();

    cout << "Pipe Sort time: " << pipe_time << " s\n";
    cout << "std::sort time: " << std_time << " s\n";
    cout << "Speedup (std/pipe): " << std_time / pipe_time << "x\n";

    // Verify correctness
    bool correct = true;
    for (size_t i = 0; i < N; i++) {
        for (size_t j = 0; j < LIMBS; j++) {
            if (arr[i * LIMBS + j] != data_sorted[i].limbs[j]) {
                correct = false;
                break;
            }
        }
        if (!correct) break;
    }
    cout << "Correct: " << (correct ? "YES" : "NO") << "\n";
}

int main() {
    vector<size_t> Ns = {10000, 50000, 100000, 500000, 1000000};
    vector<size_t> LIMBS_list = {2, 4}; // 128-bit, 256-bit

    for (auto LIMBS : LIMBS_list) {
        for (auto N : Ns) {
            benchmark(N, LIMBS, false); // random
            benchmark(N, LIMBS, true);  // skewed
        }
    }
}

