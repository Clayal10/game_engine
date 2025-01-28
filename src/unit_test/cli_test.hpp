#ifndef CLI_TEST_HPP
#define CLI_TEST_HPP

#include<iostream>
#include<vector>
#include<utility>
#include<typeinfo>
/****Colored text macros****/
//	cli_test::unit_tests.push_back(std::make_pair(std::string("test3"), (std::vector<int>){CLI::is_true(true), CLI::is_equal(1, 1)}));
#define COLOR_RED "\x1b[31m"
#define COLOR_GREEN "\x1b[32m"
#define COLOR_RESET "\x1b[0m"

#define TEST(X, ...) \
	cli_test::unit_tests.push_back(std::make_pair(X, (std::vector<int>)__VA_ARGS__))

//For easy access in test.cpp
namespace cli_test{
	inline int testing_mode = 0;
	inline std::vector<std::pair<std::string, std::vector<int>>> unit_tests;
}


/****Testing object****/
class CLI{
public:
	//test_method is for printing
	void test_method(std::pair<std::string, std::vector<int>> parameters);

	//Basic asserts
	template <typename T>
	static int is_equal(T first, T second){
		return first == second;
	}
	template <typename T>
	static int is_not_equal(T first, T second){
		return !(first == second);
	}
	static int is_true(int condition){
		return condition;
	}
	static int is_not_true(int condition){
		return !condition;
	}
	//More niche asserts
	template <typename one, typename two>
	static int is_same_type(one first, two second){
		return typeid(first) == typeid(second);
	}
	template <typename one, typename two>
	static int is_not_same_type(one first, two second){
		return !(typeid(first) == typeid(second));
	}
	//Automatic margin is 1 (casted to T) if margin is entered as 0
	template <typename T>
	static int is_close(T first, T second, double margin){
		double  m_margin = margin;
		if(margin == 0){
			m_margin = 0.5;
		}
		if(first < second + m_margin && first > second - m_margin/2){
			return 1;
		}
		return 0;
	}
};


/****Function Prototypes****/
//Called at the start of main. All of these need to be included at the start of main()
inline void init_testing_mode(int argc, char** argv);
void test_main();
inline void run_tests();

/****Function Definitions****/ //may move to a seperate file eventually.
void init_testing_mode(int argc, char** argv){
	if(argc <= 1){
		return;
	}
	char test_flag[7] = "--test";
	for(int i=0; i<6; i++){
		if(argv[1][i] == '\0'){
			return;
		}
		if(argv[1][i] != test_flag[i]){
			return;
		}
	}
	//At this point we are in test mode.
	cli_test::testing_mode = 1;
}

void run_tests(){
	if(cli_test::testing_mode == 0){
		return;
	}	

	for(unsigned long int test_num = 0; test_num<cli_test::unit_tests.size(); test_num++){
		printf("CLI TEST: %s\n", cli_test::unit_tests[test_num].first.c_str());
		int passed = 1;
		for(unsigned long int assert_num = 0; assert_num<cli_test::unit_tests[test_num].second.size(); assert_num++){
			if(cli_test::unit_tests[test_num].second[assert_num] == 0){
				passed = 0;
			}
		}
		printf("Result: ");
		if(passed == 1){
			printf(COLOR_GREEN "PASSED" COLOR_RESET);
		}else{
			printf(COLOR_RED "FAILED" COLOR_RESET);
		}
		printf("\n\n");

	}
	

	//don't run everything else in main
	exit(0);
}

#endif
