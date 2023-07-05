#include <iostream>
  
extern "C" {
	  int foo(int);
	  char func(char);
}

int main(){
	std::cout << "foo test 1 is :" << foo(1) << std::endl;
	std::cout << "foo test 2 is :" << foo(2) << std::endl;
	std::cout << "func test 1 is :" << func('b') << std::endl;
	std::cout << "func test 2 is :" << func('4') << std::endl;
}

