#ifndef SPAN_HPP
# define SPAN_HPP

# include <vector>
# include <stdexcept> // std::out_of_range, std::logic_error
# include <string>

/*
** Span.hpp
**
** Span クラス: 最大 N 個の int を格納できる容器。
**   - addNumber(int): 1 個追加。すでに満杯なら例外。
**   - addRange(begin, end): イテレータ区間で一括追加（テンプレートメンバ）。
**   - shortestSpan(): 格納値の中で「最も近い 2 値の差」を返す。
**   - longestSpan():  格納値の中で「最も離れた 2 値の差」(= max - min) を返す。
**
** 内部実装には std::vector<int> を使う（生配列を手書きしない）。
**
** ----------------------------------------------------------------------
** ■ addRange がテンプレートメンバ関数である理由
**
**   「区間で一括追加」したいが、渡されるイテレータの型は呼ぶ側によって
**   変わる（vector の iterator, list の iterator, 生ポインタ など）。
**   どの型でも受けられるように、メンバ関数自身をテンプレート化する。
**   標準コンテナの assign(first, last) / insert(pos, first, last) /
**   コンストラクタ(first, last) が「InputIterator を取るテンプレート」
**   なのと同じ発想。
**
**   InputIterator とは「前から 1 回ずつ読みながら ++ で進めればよい」
**   という最小要件のイテレータ。これを要求しておけば、ほぼあらゆる
**   コンテナの区間を受け取れる。
*/

class Span
{
	private:
		unsigned int		_maxSize;	// 格納できる上限 N
		std::vector<int>	_numbers;	// 実際の格納先

	public:
		// --- 例外クラス（OCF とは別に、用途別に用意） ---

		// すでに満杯なのに addNumber/addRange しようとした
		class SpanFullException : public std::exception
		{
			public:
				virtual const char* what() const throw();
		};

		// 要素が 0 or 1 個で span を計算できない
		class NotEnoughNumbersException : public std::exception
		{
			public:
				virtual const char* what() const throw();
		};

		// --- OCF (Orthodox Canonical Form) ---
		Span(void);						// 既定では maxSize=0（あまり使わない）
		explicit Span(unsigned int n);	// 唯一の意味あるコンストラクタ
		Span(const Span& other);
		Span& operator=(const Span& other);
		~Span(void);

		// --- 公開 API ---
		void			addNumber(int n);
		unsigned int	shortestSpan(void) const;
		unsigned int	longestSpan(void) const;

		// 補助: 現在の格納数・上限を取得（テストや表示に便利）
		unsigned int	size(void) const;
		unsigned int	maxSize(void) const;

		/*
		** addRange: [begin, end) の整数をまとめて追加する。
		**
		** テンプレートメンバ関数なので、定義はヘッダに置く必要がある。
		** （非テンプレートメンバの実装は Span.cpp に書く、というルールは
		**   守りつつ、テンプレートだけは例外的にヘッダに置く。）
		**
		** 実装方針:
		**   1. まず追加件数 (end - begin ではなく std::distance) を数え、
		**      上限を超えるなら何も追加せず例外を投げる（all-or-nothing）。
		**   2. vector::insert(pos, first, last) で一括挿入する。
		**      これ自体が「区間挿入」の STL 機能。
		*/
		template <typename InputIterator>
		void	addRange(InputIterator begin, InputIterator end);
};

# include "Span.tpp"

#endif
