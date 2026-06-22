#include "Span.hpp"
#include <algorithm>  // std::sort, std::min_element, std::max_element, std::adjacent_difference
#include <numeric>    // std::adjacent_difference
#include <limits>     // std::numeric_limits

/*
** Span.cpp
**
** Span の非テンプレートメンバの実装。
** （テンプレートメンバ addRange は Span.tpp 側にある。）
*/

/* ------------------------------------------------------------------ */
/* 例外クラスの what()                                                 */
/* ------------------------------------------------------------------ */

const char* Span::SpanFullException::what() const throw()
{
	return ("Span: container is already full");
}

const char* Span::NotEnoughNumbersException::what() const throw()
{
	return ("Span: need at least 2 numbers to compute a span");
}

/* ------------------------------------------------------------------ */
/* OCF                                                                 */
/* ------------------------------------------------------------------ */

/*
** 既定コンストラクタ: maxSize = 0。
** 何も入れられない Span。基本は使わないが OCF として用意する。
*/
Span::Span(void) : _maxSize(0), _numbers()
{
}

/*
** 主コンストラクタ: 上限 N を受け取る。
** explicit を付けて「Span s = 5;」のような暗黙変換を禁止する。
**
** reserve(n):
**   vector に「これから最大 n 個入る」と前もって伝え、内部メモリを
**   一括確保しておく。こうすると addNumber を繰り返しても途中で
**   再確保（=全要素のコピー移動）が起きず、効率が良い。
*/
Span::Span(unsigned int n) : _maxSize(n), _numbers()
{
	_numbers.reserve(n);
}

/*
** コピーコンストラクタ: vector はそのままコピーすれば中身も複製される。
*/
Span::Span(const Span& other) : _maxSize(other._maxSize), _numbers(other._numbers)
{
}

/*
** コピー代入演算子: 自己代入チェックの後、両メンバを複製する。
*/
Span& Span::operator=(const Span& other)
{
	if (this != &other)
	{
		_maxSize = other._maxSize;
		_numbers = other._numbers;
	}
	return (*this);
}

Span::~Span(void)
{
}

/* ------------------------------------------------------------------ */
/* 公開 API                                                            */
/* ------------------------------------------------------------------ */

/*
** addNumber: 1 個追加。すでに満杯なら例外。
*/
void	Span::addNumber(int n)
{
	if (_numbers.size() >= static_cast<std::size_t>(_maxSize))
		throw SpanFullException();
	_numbers.push_back(n);
}

/*
** shortestSpan: 「最も近い 2 つの値の差」を返す。
**
** ■ アルゴリズム（なぜソートして隣接差を見るのか）
**
**   最短スパンとは「すべてのペア (i, j) の |a[i] - a[j]| の最小値」。
**   素朴にやると全ペア O(n^2)。だが値をソートすると、
**   最も近い 2 値は必ず「ソート後に隣り合う」ことが保証される。
**     例: 6 3 17 9 11 → ソート → 3 6 9 11 17
**         隣接差:        3 3  2  6
**         最小は 2  ← これが shortestSpan
**   よって「ソート O(n log n) → 隣接差の最小 O(n)」で求まる。
**
**   隣接差の計算には std::adjacent_difference を使う。
**   これは out[0]=in[0], out[i]=in[i]-in[i-1] を計算する STL 関数。
**   out[0] は単なる先頭値なのでスパンとしては無視し、out[1..] の
**   最小を std::min_element で取る。
*/
unsigned int	Span::shortestSpan(void) const
{
	if (_numbers.size() < 2)
		throw NotEnoughNumbersException();

	// 元データを壊さないようコピーしてソートする
	std::vector<int> sorted(_numbers);
	std::sort(sorted.begin(), sorted.end());

	// 隣接差を計算: diffs[0] は先頭値、diffs[i>=1] が a[i]-a[i-1]
	std::vector<int> diffs(sorted.size());
	std::adjacent_difference(sorted.begin(), sorted.end(), diffs.begin());

	// diffs[1..end) の最小値が最短スパン。
	// ソート済みなので隣接差は必ず 0 以上 → そのまま unsigned へ。
	std::vector<int>::iterator minIt =
		std::min_element(diffs.begin() + 1, diffs.end());

	return (static_cast<unsigned int>(*minIt));
}

/*
** longestSpan: 「最も離れた 2 値の差」= 最大値 - 最小値。
**
** std::min_element / std::max_element で最小・最大を一発で取る。
** ソートは不要（端の 2 つだけ分かればよい）。
*/
unsigned int	Span::longestSpan(void) const
{
	if (_numbers.size() < 2)
		throw NotEnoughNumbersException();

	std::vector<int>::const_iterator minIt =
		std::min_element(_numbers.begin(), _numbers.end());
	std::vector<int>::const_iterator maxIt =
		std::max_element(_numbers.begin(), _numbers.end());

	// max >= min なので差は必ず 0 以上
	return (static_cast<unsigned int>(*maxIt - *minIt));
}

unsigned int	Span::size(void) const
{
	return (static_cast<unsigned int>(_numbers.size()));
}

unsigned int	Span::maxSize(void) const
{
	return (_maxSize);
}
