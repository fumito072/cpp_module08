#include "Span.hpp"
#include <iostream>
#include <vector>
#include <list>
#include <cstdlib> // std::rand, std::srand
#include <ctime>   // std::time

/*
** main.cpp (ex01: Span)
**
** 1) PDF 指定の基本テスト（2 と 14 が出ること）
** 2) 例外テスト（満杯への追加 / 要素 0,1 個での span 要求）
** 3) addRange（イテレータ区間での一括追加）テスト
** 4) 大量データ（10000 個 以上）でのテスト
*/

int main(void)
{
	std::cout << "=== ex01: Span ===" << std::endl;

	/* ---------------------------------------------------------------- */
	/* 1) PDF 指定のテスト: 2 と 14 を出力する                          */
	/* ---------------------------------------------------------------- */
	std::cout << "\n--- (1) PDF example ---" << std::endl;
	{
		Span sp = Span(5);
		sp.addNumber(6);
		sp.addNumber(3);
		sp.addNumber(17);
		sp.addNumber(9);
		sp.addNumber(11);
		std::cout << "shortestSpan = " << sp.shortestSpan() << std::endl; // 2
		std::cout << "longestSpan  = " << sp.longestSpan()  << std::endl; // 14
	}

	/* ---------------------------------------------------------------- */
	/* 2) 例外テスト                                                    */
	/* ---------------------------------------------------------------- */
	std::cout << "\n--- (2) exceptions ---" << std::endl;
	{
		// 満杯への追加
		Span small(2);
		small.addNumber(1);
		small.addNumber(2);
		try
		{
			small.addNumber(3); // もう満杯 → 例外
		}
		catch (const std::exception& e)
		{
			std::cout << "add to full (expected): " << e.what() << std::endl;
		}

		// 要素 0 個での span 要求
		Span empty(5);
		try
		{
			empty.shortestSpan();
		}
		catch (const std::exception& e)
		{
			std::cout << "shortest on empty (expected): " << e.what() << std::endl;
		}

		// 要素 1 個での span 要求
		Span one(5);
		one.addNumber(42);
		try
		{
			one.longestSpan();
		}
		catch (const std::exception& e)
		{
			std::cout << "longest on size=1 (expected): " << e.what() << std::endl;
		}
	}

	/* ---------------------------------------------------------------- */
	/* 3) addRange: イテレータ区間で一括追加                            */
	/* ---------------------------------------------------------------- */
	std::cout << "\n--- (3) addRange ---" << std::endl;
	{
		// vector の区間から追加
		int raw[] = {4, 8, 15, 16, 23, 42};
		std::vector<int> src(raw, raw + 6);

		Span sp(10);
		sp.addRange(src.begin(), src.end()); // vector::iterator を渡す
		std::cout << "after addRange(vector): size = " << sp.size() << std::endl;
		std::cout << "shortestSpan = " << sp.shortestSpan() << std::endl; // 1 (16-15)
		std::cout << "longestSpan  = " << sp.longestSpan()  << std::endl; // 38 (42-4)

		// list の区間からも追加できる（別のイテレータ型でも OK）
		int raw2[] = {100, 1, 50};
		std::list<int> src2(raw2, raw2 + 3);
		sp.addRange(src2.begin(), src2.end()); // list::iterator を渡す
		std::cout << "after addRange(list): size = " << sp.size() << std::endl;
		std::cout << "longestSpan  = " << sp.longestSpan()  << std::endl; // 99 (100-1)

		// 生ポインタ（=最も基本的なイテレータ）でも OK
		Span sp2(3);
		sp2.addRange(raw, raw + 3); // int* を渡す → {4, 8, 15}
		std::cout << "addRange(raw ptr): size = " << sp2.size() << std::endl;

		// 上限超えの addRange は何も追加せず例外（all-or-nothing）
		try
		{
			sp2.addRange(raw, raw + 6); // 残り 0 枠なのに 6 個 → 例外
		}
		catch (const std::exception& e)
		{
			std::cout << "addRange overflow (expected): " << e.what() << std::endl;
			std::cout << "size unchanged = " << sp2.size() << std::endl; // 3 のまま
		}
	}

	/* ---------------------------------------------------------------- */
	/* 4) 大量データ（10000 個以上）                                    */
	/* ---------------------------------------------------------------- */
	std::cout << "\n--- (4) large test (10000+ numbers) ---" << std::endl;
	{
		const unsigned int N = 20000; // 1 万個以上で確認
		std::srand(static_cast<unsigned int>(std::time(NULL)));

		// addNumber で 1 個ずつ
		Span big(N);
		for (unsigned int i = 0; i < N; ++i)
			big.addNumber(std::rand() % 1000000); // 0..999999 の範囲
		std::cout << "filled " << big.size() << " numbers (addNumber)" << std::endl;
		std::cout << "shortestSpan = " << big.shortestSpan() << std::endl;
		std::cout << "longestSpan  = " << big.longestSpan()  << std::endl;

		// addRange で一括（こちらも 1 万個以上）
		std::vector<int> data;
		data.reserve(N);
		for (unsigned int i = 0; i < N; ++i)
			data.push_back(std::rand() % 1000000);

		Span big2(N);
		big2.addRange(data.begin(), data.end());
		std::cout << "filled " << big2.size() << " numbers (addRange)" << std::endl;
		std::cout << "shortestSpan = " << big2.shortestSpan() << std::endl;
		std::cout << "longestSpan  = " << big2.longestSpan()  << std::endl;
	}

	/* ---------------------------------------------------------------- */
	/* 5) OCF の確認（コピーが独立しているか）                          */
	/* ---------------------------------------------------------------- */
	std::cout << "\n--- (5) copy semantics ---" << std::endl;
	{
		Span a(5);
		a.addNumber(1);
		a.addNumber(100);
		Span b(a);       // コピーコンストラクタ
		Span c(5);
		c = a;           // コピー代入
		std::cout << "a.longestSpan = " << a.longestSpan() << std::endl; // 99
		std::cout << "b.longestSpan = " << b.longestSpan() << std::endl; // 99
		std::cout << "c.longestSpan = " << c.longestSpan() << std::endl; // 99
	}

	return (0);
}
