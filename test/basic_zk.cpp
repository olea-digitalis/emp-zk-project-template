#include <emp-zk/emp-zk.h>
#include <emp-tool/emp-tool.h>
#include <iostream>
#include "source/utils.cpp"

using namespace emp;
using namespace std;

int port, party;
const int threads = 1;


void hello_world_zk(BoolIO<NetIO> *ios[threads], int party) {
    setup_zk_bool<BoolIO<NetIO>>(ios, threads, party);

    Bit a(0, ALICE);
    Bit b(1, ALICE);

    cout << "hello world! revealing a^b: " << (a^b).reveal() << endl;

    bool cheat = finalize_zk_bool<BoolIO<NetIO>>();
    if (cheat)error("cheat!\n");
}

void test_utils(BoolIO<NetIO> *ios[threads], int party) {
    setup_zk_bool<BoolIO<NetIO>>(ios, threads, party);

    vector<Float> ones = Float_ones(4);
    print_Float_vec(ones);

    bool cheat = finalize_zk_bool<BoolIO<NetIO>>();
    if (cheat)error("cheat!\n");
}


void test_bit_operations_zk(BoolIO<NetIO> *ios[threads], int party) {
    setup_zk_bool<BoolIO<NetIO>>(ios, threads, party);
    Bit a(0, ALICE); // commits to a private bit a=0
    Bit b(1, PUBLIC); // commits to a public bit b=1
    
    cout << "Reveal -----------------------------------------------" << endl;
    cout << "reveal operator -- a: " << a.reveal<bool>() << "   b: " << b.reveal<bool>() << endl;
    cout << endl;
    cout << "Logical Operators ------------------------------------" << endl;
    cout << "AND operator -- a & b: " << (a & b).reveal() << endl;
    cout << "XOR operator -- a ^ b: " << (a ^ b).reveal() << endl;
    cout << "OR operator -- a | b: " << (a | b).reveal() << endl;
    cout << "NOT operator -- !a: " << (!a).reveal() << "    !b: " << (!b).reveal() << endl;
    cout << endl;
    cout << "Comparison Operators -----------------------------------" << endl;
    cout << "a==b: " << (a == b).reveal() << endl;
    cout << "a!=b: " << (a != b).reveal() << endl;
    cout << "--------------------------------------------------------" << endl;

    // composing operations:
    Bit c(0,PUBLIC);
    c = a & b ^ a;


    bool cheat = finalize_zk_bool<BoolIO<NetIO>>();
    if (cheat)error("cheat!\n");
}

// Integers are collections of Bits
// operations on integers are compiled into collections of ZK bitwise operations
void test_int_operations_zk(BoolIO<NetIO> *ios[threads], int party) {
    setup_zk_bool<BoolIO<NetIO>>(ios, threads, party);

    Integer a(32, 5, ALICE); // size in bits, value, party
    Integer b(32, 2, ALICE);
    Integer c(32, -3, ALICE);

    cout << "Reveal -----------------------------------------------" << endl;
    cout << "reveal operator -- a: " << a.reveal<int>() << "   b: " << b.reveal<int>() << "   c: " << c.reveal<int>() << endl;
    cout << endl;

    cout << "Arithmetic Operators ------------------------------------" << endl;
    cout << "Addition -- a+b: " << (a+b).reveal<int>() << endl;
    cout << "Subtraction -- a-b: " << (a-b).reveal<int>() << endl;
    cout << "Multiplication -- a*b: " << (a*b).reveal<int>() << endl;
    cout << "Division -- a/b: " << (a/b).reveal<int>() << endl;
    cout << "Modulus -- a%b: " << (a%b).reveal<int>() << endl;
    cout << endl;

    cout << "Comparison Operators -----------------------------------" << endl;
    cout << "a==b: " << (a==b).reveal() << endl;
    cout << "a!=b: " << (a!=b).reveal() << endl;
    cout << "a>b: " << (a>b).reveal() << endl;
    cout << "a>=b: " << (a>=b).reveal() << endl;
    cout << "a<b: " << (a<b).reveal() << endl;
    cout << "a<=b: " << (a<=b).reveal() << endl;
    cout << endl;

    cout << "(Integer) Bitwise Operators ------------------------------------" << endl;
    cout << "AND operator -- a & b: " << (a & b).reveal<int>() << endl;
    cout << "XOR operator -- a ^ b: " << (a ^ b).reveal<int>() << endl;
    cout << "OR operator -- a | b: " << (a | b).reveal<int>() << endl;
    cout << "(Left/Right) Shift by public value -- a << 2: " << (a<<2).reveal<int>() << endl;
    cout << "(Left/Right) Shift by private value -- a >> b: " << (a>>b).reveal<int>() << endl;
    cout << endl;

    bool cheat = finalize_zk_bool<BoolIO<NetIO>>();
    if (cheat)error("cheat!\n");
}


void test_control_flow_zk(BoolIO<NetIO> *ios[threads], int party) {
    setup_zk_bool<BoolIO<NetIO>>(ios, threads, party);

    Integer a(32, 5, ALICE); // size in bits, value, party
    Integer b(32, 2, ALICE);

    // if statements?
    // compiler error (why?)
    /*
    if (a>b) {
        cout << "program branch 1" << endl;
    } else {
        cout << "program branch 2" << endl;
    }
    */
    
    // Bob can observe the behavior of the program to get information about the hidden values
    

    // this won't throw an error, but it leaks information explicitly
    // only want to do this if we *intend* to output info about the comparison between a & b
    Bit comp = a>b;
    if (comp.reveal()) {
        cout << "program branch 1" << endl;
    } else {
        cout << "program branch 2" << endl;
    }

    // we can get conditional behavior while still retaining a 'straight line' control flow using the select operator

    // the select operator allows us to propagate the result of a comparison
    // to other values in the program *without* leaking information
    Integer x(32, 5, PUBLIC);
    Integer y(32, -5, PUBLIC);
    comp = a<b;

    Integer result = x.select(comp, y);
    // result = x if comp = 0,   y if comp = 1   (1 'performs' the select, 0 does not)

    // and now the value result may be used downstream
    // in this instance, we will reveal it just to show you the result.

    cout << "result: " << result.reveal<int>() << endl;

    bool cheat = finalize_zk_bool<BoolIO<NetIO>>();
    if (cheat)error("cheat!\n");
}

// emp implements IEEE single precision floating point numbers, built from collections of Bits
// as with Integers, Float operations are compiled into collections of ZK bitwise operations
void test_float_operations_zk(BoolIO<NetIO> *ios[threads], int party) {
    setup_zk_bool<BoolIO<NetIO>>(ios, threads, party);
    
    Float a(2.5, ALICE);
    Float b(1.5, ALICE);

    cout << "reveal operator -- a: " << a.reveal<double>() << "    b: " << b.reveal<double>() << endl;

    cout << "Float Arithmetic Operators ------------------------------------" << endl;
    cout << "Addition -- a+b: " << (a+b).reveal<double>() << endl;
    cout << "Subtraction -- a-b: " << (a-b).reveal<double>() << endl;
    cout << "Multiplication -- a*b: " << (a*b).reveal<double>() << endl;
    cout << "Division -- a/b: " << (a/b).reveal<double>() << endl;
    cout << "Exponentiation e^a -- a.exp(): " << (a.exp()).reveal<double>() << endl;
    cout << "Exponentiation 2^a -- a.exp2(): " << (a.exp2()).reveal<double>() << endl;
    cout << "Absolute Value |a| -- a.abs() : " << a.abs().reveal<double>() << endl;
    cout << "Square Root a^(1/2) -- a.sqrt() : " << a.sqrt().reveal<double>() << endl;
    cout << "Cosine cos(a) -- a.cos() : " << a.cos().reveal<double>() << endl;
    cout << "Ln and Log2 -- a.ln() : " << a.ln().reveal<double>() << "   a.log2() : " << a.log2().reveal<double>() << endl;
    cout << endl;

    cout << "Float Comparison Operators ------------------------------------" << endl;
    cout << "Equality -- a.equal(b) : " << a.equal(b).reveal() << endl;
    cout << "Less Than a<b -- a.less_than(b) : " << a.less_than(b).reveal() << endl;
    cout << "Less Than or Equal a<=b -- a.less_equal(b) : " << a.less_equal(b).reveal() << endl;
    cout << endl;
    
    

    bool cheat = finalize_zk_bool<BoolIO<NetIO>>();
    if (cheat)error("cheat!\n");
}


int main(int argc, char **argv) {
    parse_party_and_port(argv, &party, &port);
    BoolIO<NetIO> *ios[threads];
    for (int i = 0; i < threads; ++i)
        ios[i] = new BoolIO<NetIO>(new NetIO(party == ALICE ? nullptr : "127.0.0.1", port + i), party == ALICE);

    // benchmarking 
    uint64_t c_start = comm(ios, threads); // measure communication at start
    auto start = clock_start(); // timepoint at start

    hello_world_zk(ios,party);

    int t = time_from(start); // timepoint at end (microseconds)
    uint64_t c_end = comm(ios, threads); // measure communication at end (bytes)

    cout << "communication from party " << party << " (bytes): " << c_end - c_start << endl;
    cout << "time from party " << party << " (microsec): " << t << endl;


    /*
    cout << "\n\n";

    test_bit_operations_zk(ios, party);

    cout << "\n\n";

    test_int_operations_zk(ios, party);

    cout << "\n\n";

    test_float_operations_zk(ios, party);

    cout << "\n\n";

    test_control_flow_zk(ios, party);
    */

    for (int i = 0; i < threads; ++i) {
        delete ios[i]->io;
        delete ios[i];
    }
    return 0;
}