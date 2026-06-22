#ifndef MUTANTSTACK_HPP
# define MUTANTSTACK_HPP

# include <stack>
# include <deque>

/*
** MutantStack.hpp
**
** std::stack を「反復(iterate)可能」にした版。
**
** ----------------------------------------------------------------------
** ■ そもそも std::stack はなぜ反復不可なのか
**
**   std::stack は「コンテナアダプタ」。自前で要素を持つのではなく、
**   内部に別のコンテナ（既定は std::deque<T>）を抱え、その一部の操作
**   だけを LIFO（後入れ先出し）のインターフェースとして公開している。
**     push() / pop() / top() / size() / empty() ...
**   公開しているのは「てっぺん」を触る操作だけで、begin()/end() や
**   iterator を一切提供しない。だから for で全要素をなめられない。
**
** ----------------------------------------------------------------------
** ■ 内部コンテナ c へのアクセス
**
**   std::stack は内部コンテナを protected メンバ c として持っている
**   （規格で決まっている。型は container_type = 既定で std::deque<T>）。
**   protected なので「外部」からは触れないが、「派生クラス」からは
**   触れる。そこで std::stack を public 継承し、派生クラス側で this->c
**   経由でその begin()/end() を呼び出せば、内部コンテナのイテレータを
**   そのまま外へ公開できる。これが MutantStack の核心。
**
** ----------------------------------------------------------------------
** ■ typename が必要な理由（依存名）
**
**   iterator の型は
**     std::stack<T>::container_type::iterator
**   と書くが、これは T に依存して決まる型（依存名）。コンパイラは
**   テンプレートを最初に読む時点ではこれが「型」か「値」か判断
**   できないので、typename を付けて「型だ」と教える必要がある。
*/

template <typename T, typename Container = std::deque<T> >
class MutantStack : public std::stack<T, Container>
{
	public:
		/*
		** 内部コンテナのイテレータ型を、自分の型名として typedef する。
		** std::stack<T, Container>::container_type は Container と同じ。
		** その iterator / const_iterator / reverse 版を引き継ぐ。
		*/
		typedef typename Container::iterator				iterator;
		typedef typename Container::const_iterator			const_iterator;
		typedef typename Container::reverse_iterator		reverse_iterator;
		typedef typename Container::const_reverse_iterator	const_reverse_iterator;

		// --- OCF ---
		MutantStack(void) {}
		MutantStack(const MutantStack& other) : std::stack<T, Container>(other) {}
		MutantStack& operator=(const MutantStack& other)
		{
			std::stack<T, Container>::operator=(other);
			return (*this);
		}
		~MutantStack(void) {}

		/*
		** begin()/end(): 内部コンテナ c の begin()/end() をそのまま返す。
		** this-> を付ける理由:
		**   c はテンプレート基底クラスのメンバ。テンプレート基底のメンバは
		**   「依存名」なので、this-> を付けないと名前探索で見つからない
		**   （C++ の two-phase lookup の仕様）。this-> を付けると「基底に
		**   あるはず」と探してくれる。
		*/
		iterator		begin(void)			{ return (this->c.begin()); }
		iterator		end(void)			{ return (this->c.end()); }

		const_iterator	begin(void) const	{ return (this->c.begin()); }
		const_iterator	end(void) const		{ return (this->c.end()); }

		// 逆順イテレータも提供（あると便利）
		reverse_iterator		rbegin(void)		{ return (this->c.rbegin()); }
		reverse_iterator		rend(void)			{ return (this->c.rend()); }
		const_reverse_iterator	rbegin(void) const	{ return (this->c.rbegin()); }
		const_reverse_iterator	rend(void) const	{ return (this->c.rend()); }
};

#endif
