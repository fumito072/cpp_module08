#ifndef EASYFIND_TPP
# define EASYFIND_TPP

# include "easyfind.hpp"

/*
** easyfind.tpp
**
** easyfind の「定義（実装）」を書く。テンプレートなので、宣言した
** easyfind.hpp の末尾から include される（=ヘッダの一部）。
**
** ----------------------------------------------------------------------
** ■ std::find の仕組み（このモジュールの主役のひとつ）
**
**   std::find(first, last, value) は、区間 [first, last) を先頭から
**   走査し、*it == value となる最初のイテレータ it を返す。
**   見つからなければ last（=末尾の「次」を指す番兵）を返す。
**
**   重要なのは、std::find は「コンテナの種類」を一切知らないこと。
**   知っているのは「イテレータ」だけ。イテレータは
**     *it      要素を読む
**     ++it     次へ進む
**     it != x  比較する
**   という共通の操作を提供するので、vector でも list でも deque でも、
**   同じ 1 つの std::find が動く。これが STL の「アルゴリズムは
**   イテレータを介してコンテナから独立している」という設計。
**
** ----------------------------------------------------------------------
** ■ なぜ typename が必要か（依存名の解決）
**
**   T が std::vector<int> のとき T::iterator はその型。
**   しかしコンパイラはテンプレートを最初に読む段階では T が何か
**   分からない。すると「T::iterator」が
**     - 型の名前なのか
**     - （T の中の）静的メンバ変数などの値なのか
**   を判断できない。デフォルトでは「値」と解釈されてしまう。
**   そこで「これは型だ」と明示するために typename を付ける。
*/

/*
** 非 const 版: 書き換え可能なコンテナを受け取り iterator を返す。
*/
template <typename T>
typename T::iterator easyfind(T& container, int value)
{
	typename T::iterator it;

	it = std::find(container.begin(), container.end(), value);
	if (it == container.end())
		throw EasyFindException();
	return (it);
}

/*
** const 版: 読み取り専用のコンテナを受け取り const_iterator を返す。
** const なコンテナでは begin()/end() が const_iterator を返すため、
** 受け皿も const_iterator にする必要がある。
*/
template <typename T>
typename T::const_iterator easyfind(const T& container, int value)
{
	typename T::const_iterator it;

	it = std::find(container.begin(), container.end(), value);
	if (it == container.end())
		throw EasyFindException();
	return (it);
}

#endif
