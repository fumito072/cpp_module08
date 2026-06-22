#ifndef SPAN_TPP
# define SPAN_TPP

# include "Span.hpp"
# include <iterator>  // std::distance

/*
** Span.tpp
**
** テンプレートメンバ関数 addRange の定義のみを置く。
** （非テンプレートのメンバは Span.cpp 側に実装する。）
**
** std::distance(begin, end):
**   区間 [begin, end) の要素数を返す STL の関数。
**   ランダムアクセスイテレータなら end - begin で O(1)、
**   それ以外（list など）でも ++ を数えて O(n) で求めてくれる。
**   生のポインタ引き算より移植性が高い。
**
** vector::insert(pos, first, last):
**   位置 pos の直前に区間 [first, last) を挿入する。
**   ここでは末尾(end())に挿入＝末尾追加として使う。
**   1 件ずつ push_back するより、内部で必要量をまとめて確保できる。
*/
template <typename InputIterator>
void	Span::addRange(InputIterator begin, InputIterator end)
{
	// 追加しようとしている件数
	std::size_t	count = static_cast<std::size_t>(std::distance(begin, end));

	// 既存 + 追加が上限を超えるなら、何も足さずに例外（all-or-nothing）
	if (_numbers.size() + count > static_cast<std::size_t>(_maxSize))
		throw SpanFullException();

	// 区間を末尾へ一括挿入
	_numbers.insert(_numbers.end(), begin, end);
}

#endif
