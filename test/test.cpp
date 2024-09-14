#include "TApplication.h"
#include "TCanvas.h"
#include "TText.h"

#include "Parser.h"
using namespace LFEA;

#include <iostream>
#include <iomanip>
#include <string>
#include <map>
#include <filesystem>
#include <chrono>

namespace fs = std::filesystem;
#define PATH_HERE(line) std::string(fs::relative(fs::path(FULL_FILE_PATH) / __FILE__, fs::current_path())) << ":" << line


// STUFF
template <int line, typename A>
inline void Assert(A assertion)
{
    if( !assertion ) {
        std::ostringstream oss;
        oss << "false assert in " << PATH_HERE(line);
        throw std::runtime_error(oss.str());
    }
}

template <int line, typename A>
inline void AssertEqual(A a, A b)
{
    if(!(a == b)) {
        std::ostringstream oss;
        // oss << "false assert in line " + std::std::to_string(line) + ":\t";
        oss << "false assert in " << PATH_HERE(line) << ":\t";
        oss << a << " != " << b;
        throw std::runtime_error(oss.str());
    }
}

template <int line, int places>
inline void AssertEqualApprox(double a, double b)
{
    double delta = pow(10, -places);
    if(std::abs(a-b) > delta) {
        std::ostringstream oss;
        oss << "false assert in " << PATH_HERE(line) << ":\t";
        oss << a << " != " << b << " (delta = " << delta << ")";
        throw std::runtime_error(oss.str());
    }
}

template <int line, typename A>
inline void AssertUnequal(A a, A b)
{
    if(!(a != b)) {
        std::ostringstream oss;
        oss << "false assert in " << PATH_HERE(line) << ":\t";
        oss << a << " == " << b;
        throw std::runtime_error(oss.str());    
    }
}

void TestFunction(std::function<void(void)> func, std::string name, unsigned long long &bitmask, bool mute = true){
    std::string err;

    bitmask <<= 1;
    if(mute) std::cout.setstate(std::ios_base::failbit);

    auto start = std::chrono::high_resolution_clock::now();
    try {
        func();
    } catch(std::runtime_error e){
        bitmask |= 1;
        err = e.what();
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    if(mute) std::cout.clear();


    // std::cout << " · Testing " << testStack.back() << ":\r\t\t\t\t";
    std::cout << " · Testing " << name << ":\r\t\t\t\t";

    if(bitmask & 1) std::cout << "\033[1;31m✘ Failed\033[0m";
    else std::cout << "\033[1;32m✔ Passed\033[0m";


    // Time
    std::vector<std::string> units = {"s", "ms", "μs", "ns"};
    int unitCount = 0;
    double elapsedTime = elapsed.count();
    while(elapsedTime < 1) {
        elapsedTime *= 1e3;
        unitCount++;
    }
    std::cout << "\t\033[2m🕐";
    std::cout << std::setw(9) << std::setprecision(3) << std::right << std::fixed << elapsedTime;
    std::cout << " " << units[unitCount] << "\033[0m";

    std::cout << "\n";


    // Print error
    if(bitmask & 1) std::cout << "   \033[90m- " << err << "\033[0m\n";
}

#define RUN_TEST(name,...) TestFunction(name,#name,__VA_ARGS__);


// TESTS

void test_base() {
    const int argc = 5;
    char* argv[] = {
        (char*)"program_name", 
        (char*)"--fast", 
        (char*)"--good", 
        (char*)"input.txt", 
        (char*)"--type=ERROR"
    };

    Parser parser(argc, argv);

    Parser::Flag fastFlag = Parser::Flag("--fast", "-f");
    Parser::Flag goodFlag = Parser::Flag("--good", "-g");
    Parser::Positional fileArg = Parser::Positional();
    Parser::Option typeOpt = Parser::Option("--type", "-t", "DEBUG");

    // Adding flags, options, and positional arguments to the parser
    parser += &fastFlag;
    parser += &goodFlag;
    parser += &fileArg;
    parser += &typeOpt;

    bool success = parser.Process();

    std::cout << fastFlag << "\n";
    std::cout << goodFlag << "\n";
    std::cout << fileArg.val << "\n";
    std::cout << typeOpt.val << "\n";

    Assert<__LINE__>(success);
    Assert<__LINE__>(fastFlag);  // fastFlag should be set
    Assert<__LINE__>(goodFlag);  // goodFlag should be set
    AssertEqual<__LINE__>(fileArg.val, std::string("input.txt"));  // Correct positional argument value
    AssertEqual<__LINE__>(typeOpt.val, std::string("ERROR"));  // Correct option value
}

// Test case: Missing optional flags
void test_optional(){
    const int argc = 2;
    char* argv[] = {
        (char*)"program_name", 
        (char*)"input.txt"
    };

    Parser parser(argc, argv);

    Parser::Flag fastFlag = Parser::Flag("--fast", "-f");
    Parser::Flag goodFlag = Parser::Flag("--good", "-g");
    Parser::Positional fileArg = Parser::Positional();
    Parser::Option typeOpt = Parser::Option("--type", "-t", "DEBUG");

    parser += &fastFlag;
    parser += &goodFlag;
    parser += &fileArg;
    parser += &typeOpt;

    bool success = parser.Process();

    Assert<__LINE__>(success);
    Assert<__LINE__>(!fastFlag.val);  // fastFlag2 should not be set
    Assert<__LINE__>(!goodFlag.val);  // goodFlag2 should not be set
    AssertEqual<__LINE__>(typeOpt.val, std::string("DEBUG"));  // typeOpt2 should have default value
    AssertEqual<__LINE__>(fileArg.val, std::string("input.txt"));  // Correct positional argument value
}

// Test case: Invalid inputs
void test_invalid(){
    const int argc = 3;
    char* argv[] = {
        (char*)"program_name", 
        (char*)"--invalidflag", 
        (char*)"-xyz"
    };

    Parser parser(argc, argv);

    Parser::Flag fastFlag = Parser::Flag("--fast", "-f");
    Parser::Flag goodFlag = Parser::Flag("--good", "-g");
    Parser::Positional fileArg = Parser::Positional();
    Parser::Option typeOpt = Parser::Option("--type", "-t", "DEBUG");

    parser += &fastFlag;
    parser += &goodFlag;
    parser += &fileArg;
    parser += &typeOpt;

    bool success = parser.Process();
    Assert<__LINE__>(!success);
}

// Test case: Combined short flags
void test_combine(){
    const int argc = 3;
    char* argv[] = {
        (char*)"program_name", 
        (char*)"-fg",   // Combined short flags
        (char*)"input.txt"
    };

    Parser parser(argc, argv);

    Parser::Flag fastFlag = Parser::Flag("--fast", "-f");
    Parser::Flag goodFlag = Parser::Flag("--good", "-g");
    Parser::Positional fileArg = Parser::Positional();
    Parser::Option typeOpt = Parser::Option("--type", "-t", "DEBUG");

    parser += &fastFlag;
    parser += &goodFlag;
    parser += &fileArg;
    parser += &typeOpt;

    bool success = parser.Process();

    Assert<__LINE__>(success);
    Assert<__LINE__>(fastFlag.val);  // fastFlag4 should be set
    Assert<__LINE__>(goodFlag.val);  // goodFlag4 should be set
    AssertEqual<__LINE__>(fileArg.val, std::string("input.txt"));  // Correct positional argument value
}

// Test case: Missing required positional argument
void test_positional(){
    const int argc = 2;
    char* argv[] = {
        (char*)"program_name", 
        (char*)"--fast"
    };

    Parser parser(argc, argv);

    Parser::Flag fastFlag = Parser::Flag("--fast", "-f");
    Parser::Flag goodFlag = Parser::Flag("--good", "-g");
    Parser::Positional fileArg = Parser::Positional();
    Parser::Option typeOpt = Parser::Option("--type", "-t", "DEBUG");

    parser += &fastFlag;
    parser += &goodFlag;
    parser += &fileArg;
    parser += &typeOpt;

    bool success = parser.Process();
    Assert<__LINE__>(!success);
}

// Test case: Too many positional arguments
void test_toomuch(){
    const int argc = 3;
    char* argv[] = {
        (char*)"program_name", 
        (char*)"input.txt",
        (char*)"output.txt"
    };

    Parser parser(argc, argv);

    Parser::Flag fastFlag = Parser::Flag("--fast", "-f");
    Parser::Flag goodFlag = Parser::Flag("--good", "-g");
    Parser::Positional fileArg = Parser::Positional();
    Parser::Option typeOpt = Parser::Option("--type", "-t", "DEBUG");

    parser += &fastFlag;
    parser += &goodFlag;
    parser += &fileArg;
    parser += &typeOpt;

    bool success = parser.Process();
    Assert<__LINE__>(!success);
}

int main(int argc, char** argv){
    if(argc >= 2 && (std::string(argv[1]) == "--help" || std::string(argv[1]) == "--h")){
        std::cout << "Usage: ./Test [OPTIONS]\n\nOptions:\n";
        std::cout << "  --h, --help\tShow this help message and exit.\n";
        std::cout << "  --d, --debug\tEnable std::cout output in tests\n";
        return 0;
    }
    bool mute = !(argc >= 2 && (std::string(argv[1]) == "--debug" || std::string(argv[1]) == "--d"));
    

    std::cout << "\n\033[1mLFEA-Utils Test Suite\033[0m\n\n";

    unsigned long long bitmask = 1;

    RUN_TEST(test_base, bitmask, mute);
    RUN_TEST(test_combine, bitmask, mute);
    RUN_TEST(test_invalid, bitmask, mute);
    RUN_TEST(test_optional, bitmask, mute);
    RUN_TEST(test_positional, bitmask, mute);
    RUN_TEST(test_toomuch, bitmask, mute);


    int count = 63-__builtin_clzll(bitmask);
    int failed = __builtin_popcount(bitmask)-1;
    int passed = count - failed;
    std::cout << "\n\nPassed tests: " << passed << "\n";
    std::cout << "Failed tests: " << failed << "\n";

    return !!failed;
}