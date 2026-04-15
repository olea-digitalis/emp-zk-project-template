#pragma once

#include <emp-zk/emp-zk.h>
#include <emp-tool/emp-tool.h>
#include <iostream>
#include <unordered_map>
#include <random>

using namespace emp;
using namespace std;


// placeholder for secure randomness generator
void random_bits(bool *out, size_t num_bits) {
    for (int i=0; i<num_bits; ++i) {
        out[i] = 1;
    }
}

bool is_equal(vector<Integer> & a, vector<Integer> & b) {
    if (a.size() != b.size()) {
        return 0;
    }
    size_t sz = a.size();
    for (int i=0; i<sz; ++i) {
        if (a[i].reveal<int>() != b[i].reveal<int>()) {
            return 0;
        }
    }
    return 1;
}


vector<Float> init_Float_vec(vector<double> xs) {
    vector<Float> out_vec;
    size_t sz = xs.size();
    for (int i=0; i<sz; ++i) {
        out_vec.push_back(Float(xs[i], ALICE));
    }
    return out_vec;
}

vector<Float> Float_zeros(size_t sz) {
    vector<Float> out_vec;
    for (int i=0; i<sz; ++i) {
        out_vec.push_back(Float(0.0, ALICE));
    }
    return out_vec;
}

vector<Float> Float_ones(size_t sz) {
    vector<Float> out_vec;
    for (int i=0; i<sz; ++i) {
        out_vec.push_back(Float(1.0, ALICE));
    }
    return out_vec;
}

void print_Float_vec(vector<Float> xs) {
    cout << "[ ";
    size_t sz = xs.size();
    for (int i=0; i<sz; ++i) {
        cout << " " << xs[i].reveal<double>() << " ";
    }
    cout << "]\n";
}

void print_Bit_vec(vector<Bit> xs) {
    cout << "[ ";
    size_t sz = xs.size();
    for (int i=0; i<sz; ++i) {
        cout << xs[i].reveal();
    }
    cout << " ]\n";
}

// dataset of all 1 vectors
vector< vector<Float> > init_dummy_D(size_t num_pts, size_t num_features) {
    vector< vector<Float> > D;
    for (int i=0; i<num_pts; ++i) {
        D.push_back(Float_ones(num_features));
    }
    return D;
}

// randomize labels via bernoulli dist
// p: probability of label 1
vector<Float> bernoulli_labels(size_t num_pts, double p) {
    vector<Float> Ys;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::bernoulli_distribution dist(p);
    for (int i=0; i<num_pts; ++i) {
        if (dist(gen)) {
            Ys.push_back(Float(1.0, ALICE));
        } else {
            Ys.push_back(Float(0.0, ALICE));
        }
    }
    return Ys;
}

// start inclusive, end NOT inclusive
vector<int> indices_range(size_t start, size_t end) {
    vector<int> ret;
    for (int i=start; i<end; ++i) {
        ret.push_back(i);
    }
    return ret;
}

vector<int> pseudo_indices_range(size_t start_sim, size_t end_sim) {
    vector<int> ret;
    for (int i=start_sim; i<end_sim; ++i) {
        ret.push_back(0);
    }
    return ret;
}

Float Float_vec_sum(vector<Float> xs) {
    Float sum(0.0, PUBLIC);
    size_t sz = xs.size();
    for (int i=0; i<sz; ++i) {
        sum = sum + xs[i];
    }
    return sum;
}

Float Float_vec_squared_sum(vector<Float> xs) {
    Float sum(0.0, PUBLIC);
    size_t sz = xs.size();
    for (int i=0; i<sz; ++i) {
        sum = sum + (xs[i] * xs[i]);
    }
    return sum;
}

vector<int> ex_node_names_01() {
    vector<int> nn;
    for (int i=1; i<=7; ++i) {
        nn.push_back(i);
    }
    return nn;
}

unordered_map<int, int> ex_leaf_vals_01() {
    unordered_map<int, int> lv = {
        {4, 2*4},
        {5, 2*5},
        {6, 2*6},
        {7, 2*7}
    };
    return lv;
}

// return an Integer with the same bit pattern as the input float
Integer float_word(Float x) {
    Integer ret(32, 0, PUBLIC);
    for (int i=0; i<32; ++i) {
        ret[i] = x[i];
    }
    return ret;
}



Integer int_relu(Integer x) {
    Integer ZERO(32, 0, PUBLIC);
    Bit b = (x < ZERO);
    return x.select(b, ZERO);
}

Integer find_msb(Integer x) {
    Integer msb(32, 30, PUBLIC);
    Integer Int_i(32, 30, PUBLIC);
    Integer ONE(32, 1, PUBLIC);
    Bit found(0,PUBLIC);
    Bit TRUE(1, PUBLIC);
    for (int i=30; i>=0; --i) {
        msb = msb.select(x[i] & (!found), Int_i); // if bit i of x is 1, and haven't found a 1 yet, msb = i
        found = found | x[i]; // when we find the first x[i]==1, set found to 1 forever
        Int_i = Int_i - ONE;
    }
    return msb;
}

// assumes size 32 integer
Float int_to_float(Integer x) {
    Bit sign_bit = x[32-1];
    x = x.abs();

    // find msb and gen mantissa
    Integer msb(32, 30, PUBLIC);
    Integer mantissa(32, 0, PUBLIC);
    Integer Int_i(32, 30, PUBLIC);
    Integer ONE(32, 1, PUBLIC);
    Integer ZERO(32, 0, PUBLIC);
    Bit FALSE(0, PUBLIC);
    Bit found(0,PUBLIC);
    Bit TRUE(1, PUBLIC);
    for (int i=30; i>=0; --i) {
        msb = msb.select(x[i] & (!found), Int_i); // if bit i of x is 1, and haven't found a 1 yet, msb = i
        mantissa[i] = x[i].select(!found, FALSE); // start copying after leading 1 is found
        found = found | x[i]; // when we find the first x[i]==1, set found to 1 forever
        Int_i = Int_i - ONE;
    }
    Integer exponent = msb + Integer(32, 127, PUBLIC); // bias

    // align mantissa bits to the 22nd position
    Integer TWENTY_TWO(32, 22, PUBLIC);
    Integer leftshift = TWENTY_TWO - (msb-ONE);
    leftshift = int_relu(leftshift);

    Integer rightshift = (msb-ONE) - TWENTY_TWO;
    rightshift = int_relu(rightshift);

    mantissa = mantissa << leftshift;
    mantissa = mantissa >> rightshift;

    exponent = exponent << 23;
    Integer int_out(32, 0, PUBLIC);
    int_out = int_out | exponent | mantissa;
    int_out[31] = sign_bit;
    int_out = int_out.select(x==ZERO, ZERO);
    Float out(0.0, PUBLIC);
    for (int i=0; i<32; ++i) {
        out[i] = int_out[i];
    }
    return out;
}


Integer float_to_int(Float x) {
    Integer ZERO(32, 0, PUBLIC);
    Integer w = float_word(x);
    Integer exp_mask = Integer(32, 0x7f800000, PUBLIC);
    Integer mantissa_mask = Integer(32, 0x007fffff, PUBLIC);
    // Extract exponent field. 
    Integer exponent_less_127 = ((w & exp_mask) >> 23) - Integer(32, 127, PUBLIC);
    Integer mantissa = w & mantissa_mask; 
    Integer leading_one(32, (1 << 23), PUBLIC);
    Integer out = mantissa | leading_one;

    Integer TWENTY_THREE(32, 23, PUBLIC);
    Integer THIRTY_TWO(32,32,PUBLIC);
    
    Integer rightscroll = int_relu(TWENTY_THREE - exponent_less_127);
    Integer leftscroll = int_relu(exponent_less_127 - TWENTY_THREE);
    Bit rightscroll_overflow = rightscroll >= THIRTY_TWO;
    Bit leftscroll_overflow = leftscroll >= THIRTY_TWO;
    //cout << "input: " << x.reveal<double>() << " " << "exponent_less_127: " << exponent_less_127.reveal<int>() << endl;
    //cout << "input: " << x.reveal<double>() << " " << "rightscroll: " << rightscroll.reveal<int>() << endl;
    //cout << "input: " << x.reveal<double>() << " " << "leftscroll: " << leftscroll.reveal<int>() << endl;
    //cout << "input: " << x.reveal<double>() << " " << "out (before scrolling): " << out.reveal<int>() << endl;
    out = out >> rightscroll;
    out = out.select(rightscroll_overflow, ZERO); 
    //cout << "input: " << x.reveal<double>() << " " << "out (after rightscrolling): " << out.reveal<int>() << endl;
    out = out << leftscroll;
    out = out.select(leftscroll_overflow, ZERO);
    //cout << "input: " << x.reveal<double>() << " " << "out (after leftscrolling): " << out.reveal<int>() << endl;

    Integer neg_out = out * Integer(32, -1, PUBLIC);
    out = out.select(out<Integer(32,0,PUBLIC), neg_out);
    //out = out.select(x.equal(Float(0.0, PUBLIC)), ZERO);
    return out;
}

Float fp_to_float(Integer x, size_t frac_bits=8) {
    Float ret = int_to_float(x);
    return ret * Float(exp2(-frac_bits), PUBLIC);
}


Integer float_to_fp(Float x, size_t frac_bits=8) {
    Float temp = x * Float((1<<frac_bits), PUBLIC);
    return float_to_int(temp);
}

template <typename T>
void print_vec(vector<T> xs) {
    cout << "[";
    size_t sz = xs.size();
    for (int i=0; i<sz; ++i) {
        cout << " " << xs[i] << " ";
    }
    cout << "]\n";
}

template <typename T1, typename T2>
void print_map(unordered_map<T1, T2> m) {
    cout << "{";
    for (auto i : m) {
        cout << " (" << i.first << ", " << i.second << ") ";
    }
    cout << "}\n";
}



uint64_t comm1(BoolIO<NetIO> *ios[], size_t threads) {
  uint64_t c = 0;
  for (int i = 0; i < threads; ++i)
    c += ios[i]->counter;
  return c;
}
uint64_t comm2(BoolIO<NetIO> *ios[], size_t threads) {
  uint64_t c = 0;
  for (int i = 0; i < threads; ++i)
    c += ios[i]->io->counter;
  return c;
}

uint64_t comm(BoolIO<NetIO> * ios[], size_t threads) {
    return comm1(ios, threads) + comm2(ios, threads);
}