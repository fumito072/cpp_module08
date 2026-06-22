#include <iostream>
#include <vector>
#include <list>
#include <deque>
#include "easyfind.hpp"

/*
** main.cpp (ex00: easyfind)
**
** easyfind を「見つかる/見つからない」両方のケースで、複数のコンテナ
** （vector / list / deque）に対してテストする。
*/

/*
** コンテナの中身を表示する小さなヘルパ（テンプレート）。
** どのコンテナでも const_iterator で走査できる。
*/
template <typename T>
static void printContainer(const std::string& name, const T& c)
{
	std::cout << name << " = [ ";
	for (typename T::const_iterator it = c.begin(); it != c.end(); ++it)
		std::cout << *it << " ";
	std::cout << "]" << std::endl;
}

int main(void)
{
	std::cout << "=== ex00: easyfind ===" << std::endl;

	// --- std::vector<int> ---
	std::vector<int> vec;
	vec.push_back(5);
	vec.push_back(10);
	vec.push_back(15);
	vec.push_back(20);
	vec.push_back(10); // 10 が 2 回ある: easyfind は「最初の」位置を返す
	printContainer("vector", vec);

	// (1) 見つかるケース
	try
	{
		std::vector<int>::iterator it = easyfind(vec, 10);
		// begin() からの距離を出すと「何番目で見つかったか」が分かる
		std::cout << "found 10 in vector at index "
				  << (it - vec.begin()) << std::endl; // 1 番目（最初の10）
		// 非 const 版なので書き換えも可能（イテレータ経由）
		*it = 99;
		std::cout << "after rewriting that element: ";
		printContainer("vector", vec);
		*it = 10; // 元に戻しておく
	}
	catch (const std::exception& e)
	{
		std::cout << "exception: " << e.what() << std::endl;
	}

	// (2) 見つからないケース → 例外
	try
	{
		easyfind(vec, 42);
		std::cout << "(should not reach here)" << std::endl;
	}
	catch (const std::exception& e)
	{
		std::cout << "exception (expected): " << e.what() << std::endl;
	}

	std::cout << std::endl;

	// --- std::list<int> ---
	std::list<int> lst;
	lst.push_back(1);
	lst.push_back(2);
	lst.push_back(3);
	printContainer("list  ", lst);
	try
	{
		std::list<int>::iterator it = easyfind(lst, 2);
		std::cout << "found 2 in list, *it = " << *it << std::endl;
	}
	catch (const std::exception& e)
	{
		std::cout << "exception: " << e.what() << std::endl;
	}
	try
	{
		easyfind(lst, 100);
	}
	catch (const std::exception& e)
	{
		std::cout << "exception (expected): " << e.what() << std::endl;
	}

	std::cout << std::endl;

	// --- std::deque<int> ---
	std::deque<int> deq;
	deq.push_back(100);
	deq.push_back(200);
	deq.push_back(300);
	printContainer("deque ", deq);
	try
	{
		std::deque<int>::iterator it = easyfind(deq, 300);
		std::cout << "found 300 in deque, *it = " << *it << std::endl;
	}
	catch (const std::exception& e)
	{
		std::cout << "exception: " << e.what() << std::endl;
	}

	std::cout << std::endl;

	// --- const コンテナで const_iterator 版が呼ばれることの確認 ---
	const std::vector<int> cvec(vec);
	try
	{
		std::vector<int>::const_iterator it = easyfind(cvec, 15);
		std::cout << "found 15 in const vector at index "
				  << (it - cvec.begin()) << std::endl;
	}
	catch (const std::exception& e)
	{
		std::cout << "exception: " << e.what() << std::endl;
	}

	return (0);
}
