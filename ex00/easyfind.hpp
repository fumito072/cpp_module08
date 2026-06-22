#ifndef EASYFIND_HPP
# define EASYFIND_HPP

# include <algorithm> // std::find
# include <stdexcept> // std::runtime_error

/*
** easyfind.hpp
**
** 関数テンプレート easyfind を宣言する。
**
** 仕様:
**   - 型 T は「整数(int)を格納したコンテナ」を想定する
**     （例: std::vector<int>, std::list<int>, std::deque<int> ...）。
**   - 第1引数: そのコンテナ（T 型、const 参照で受け取る）。
**   - 第2引数: 探したい整数値（int）。
**   - コンテナの中から、その値が「最初に現れる位置」を探す。
**   - 見つかればその位置を指すイテレータを返す。
**   - 見つからなければ例外 (EasyFindException) を投げる。
**
** なぜテンプレートか:
**   vector でも list でも deque でも、同じ「中身から値を探す」処理を
**   1 つのコードで書きたいから。T を変えるだけで、各コンテナ用の
**   実体をコンパイラが自動生成してくれる。
**
** なぜ定義をヘッダ(.tpp)に書くか:
**   テンプレートは「どの型 T で使うか」が決まった時点で初めて実体化
**   される。実体化にはテンプレートの「定義そのもの」が見えている
**   必要があるため、宣言と定義をヘッダ側に置く（.cpp に分離できない）。
**   見やすさのため、宣言は .hpp、定義は .tpp に分け、末尾で include する。
*/

# include <exception>

/*
** 値が見つからなかったときに投げる例外クラス。
** std::exception を継承し、what() で説明文を返す。
*/
class EasyFindException : public std::exception
{
	public:
		virtual const char* what() const throw()
		{
			return ("easyfind: value not found in container");
		}
};

/*
** const 版と非 const 版の 2 つを用意する。
**
** - 非 const のコンテナを渡したとき:    iterator を返す（要素を書き換え可）
** - const のコンテナを渡したとき:       const_iterator を返す（読み取り専用）
**
** 「typename T::iterator」のように、テンプレート引数 T に依存して決まる型
** （依存名）の前には typename が必要。理由は .tpp / README で詳述。
*/
template <typename T>
typename T::iterator easyfind(T& container, int value);

template <typename T>
typename T::const_iterator easyfind(const T& container, int value);

# include "easyfind.tpp"

#endif
