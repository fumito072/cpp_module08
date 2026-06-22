#include "MutantStack.hpp"
#include <iostream>
#include <stack>
#include <list>
#include <vector>

/*
** main.cpp (ex02: MutantStack)
**
** 1) PDF 指定のテスト（push/top/pop/size、begin/end でのイテレート、
**    std::stack<int> s(mstack); でコピーできること）
** 2) MutantStack が std::stack の機能を全部持つことの確認
** 3) 内部コンテナを std::list に差し替えても同じ出力になることの確認
*/

int main(void)
{
	/* ---------------------------------------------------------------- */
	/* 1) PDF 指定のテスト                                              */
	/* ---------------------------------------------------------------- */
	std::cout << "=== (1) PDF example (deque-backed) ===" << std::endl;
	{
		MutantStack<int> mstack;

		mstack.push(5);
		mstack.push(17);

		std::cout << "top  = " << mstack.top() << std::endl; // 17

		mstack.pop();

		std::cout << "size = " << mstack.size() << std::endl; // 1

		mstack.push(3);
		mstack.push(5);
		mstack.push(737);
		//[...]
		mstack.push(0);

		// イテレータで全要素を走査（stack 本来は不可能なことができる）
		MutantStack<int>::iterator it = mstack.begin();
		MutantStack<int>::iterator ite = mstack.end();

		++it;
		--it;

		std::cout << "iterate: ";
		while (it != ite)
		{
			std::cout << *it << " ";
			++it;
		}
		std::cout << std::endl;

		// MutantStack から std::stack をコピー構築できる
		// （MutantStack は std::stack を public 継承しているから）
		std::stack<int> s(mstack);
		std::cout << "copied to std::stack, top = " << s.top() << std::endl;
	}

	/* ---------------------------------------------------------------- */
	/* 2) std::stack の機能を全部持つことの確認                         */
	/* ---------------------------------------------------------------- */
	std::cout << "\n=== (2) still behaves like std::stack ===" << std::endl;
	{
		MutantStack<int> ms;
		ms.push(1);
		ms.push(2);
		ms.push(3);
		std::cout << "empty? " << (ms.empty() ? "yes" : "no") << std::endl; // no
		std::cout << "size = " << ms.size() << std::endl; // 3
		std::cout << "top  = " << ms.top() << std::endl;  // 3

		// 逆順イテレータ（てっぺんから底へ）
		std::cout << "reverse iterate: ";
		for (MutantStack<int>::reverse_iterator rit = ms.rbegin();
			 rit != ms.rend(); ++rit)
			std::cout << *rit << " ";
		std::cout << std::endl;
	}

	/* ---------------------------------------------------------------- */
	/* 3) 同じ手順を std::list に差し替えて同じ出力になることを確認     */
	/*    （まず通常の std::list で参照出力を作り、それと一致するか）   */
	/* ---------------------------------------------------------------- */
	std::cout << "\n=== (3a) reference output using std::list ===" << std::endl;
	{
		std::list<int> lst;
		lst.push_back(5);
		lst.push_back(17);
		std::cout << "back = " << lst.back() << std::endl; // 17
		lst.pop_back();
		std::cout << "size = " << lst.size() << std::endl; // 1
		lst.push_back(3);
		lst.push_back(5);
		lst.push_back(737);
		lst.push_back(0);

		std::list<int>::iterator it = lst.begin();
		std::list<int>::iterator ite = lst.end();
		++it;
		--it;
		std::cout << "iterate: ";
		while (it != ite)
		{
			std::cout << *it << " ";
			++it;
		}
		std::cout << std::endl;
	}

	std::cout << "\n=== (3b) same code on MutantStack backed by std::list ===" << std::endl;
	{
		// 内部コンテナを std::list に差し替えた MutantStack
		MutantStack<int, std::list<int> > mstack;

		mstack.push(5);
		mstack.push(17);
		std::cout << "top  = " << mstack.top() << std::endl; // 17
		mstack.pop();
		std::cout << "size = " << mstack.size() << std::endl; // 1
		mstack.push(3);
		mstack.push(5);
		mstack.push(737);
		mstack.push(0);

		MutantStack<int, std::list<int> >::iterator it = mstack.begin();
		MutantStack<int, std::list<int> >::iterator ite = mstack.end();
		++it;
		--it;
		std::cout << "iterate: ";
		while (it != ite)
		{
			std::cout << *it << " ";
			++it;
		}
		std::cout << std::endl;
		// → (3a) と同じ "5 3 5 737 0" になるはず
	}

	/* ---------------------------------------------------------------- */
	/* 4) const / コピーの確認                                          */
	/* ---------------------------------------------------------------- */
	std::cout << "\n=== (4) copy & const_iterator ===" << std::endl;
	{
		MutantStack<int> a;
		a.push(10);
		a.push(20);
		a.push(30);

		MutantStack<int> b(a);   // コピーコンストラクタ
		MutantStack<int> c;
		c = a;                   // コピー代入

		const MutantStack<int>& cref = a;
		std::cout << "const iterate a: ";
		for (MutantStack<int>::const_iterator it = cref.begin();
			 it != cref.end(); ++it)
			std::cout << *it << " ";
		std::cout << std::endl;

		std::cout << "b.top = " << b.top() << ", c.top = " << c.top() << std::endl;
	}

	return (0);
}
