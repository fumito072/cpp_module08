# ex02 — MutantStack

## 1. この演習でやること

`std::stack` は「反復（iterate）できない」コンテナです。for ループで
全要素をなめることができません。これを継承して、**イテレータを持たせて
反復できるようにした** `MutantStack<T>` を作ります。

- `std::stack` の全機能（`push` / `top` / `pop` / `size` / `empty` …）はそのまま使える
- 追加で `iterator` / `const_iterator` と `begin()` / `end()` を提供する
- PDF の main 例（push/top/pop/size、begin/end でのイテレート、
  `std::stack<int> s(mstack);` でのコピー）が動く
- 内部コンテナを `std::list` に差し替えても同じ出力になることも確認する

```cpp
MutantStack<int> mstack;
mstack.push(5);
mstack.push(17);
std::cout << mstack.top();          // 17 （stack の機能）

for (MutantStack<int>::iterator it = mstack.begin();
     it != mstack.end(); ++it)      // ← stack 単体ではできない反復
    std::cout << *it;
```

---

## 2. 登場する新しい概念

### (a) コンテナアダプタ stack の内部構造

`std::stack` は「**コンテナアダプタ**」です。自分でデータを持たず、
内部に別のコンテナ（既定は `std::deque<T>`）を抱え、その操作の一部だけを
LIFO（後入れ先出し）のインターフェースとして見せています。

```
   std::stack<int>
   ┌───────────────────────────┐
   │ 公開API: push/top/pop/...  │  ← てっぺんを触る操作だけ
   │  ┌──────────────────────┐  │
   │  │ protected: c          │  │  ← 内部コンテナ（既定 std::deque<int>）
   │  │  [5][3][5][737][0]    │  │     ここに実データが入っている
   │  └──────────────────────┘  │
   └───────────────────────────┘
```

`std::stack` は `begin()` / `end()` を提供しないので、内部の `c` を直接
なめる手段が**外からは**ありません。だから反復できないのです。

### (b) protected メンバ `c`

規格上、`std::stack` は内部コンテナを **protected メンバ `c`** として
持っています（型は `container_type`、既定で `std::deque<T>`）。

- `protected` なので「外部」からはアクセスできない
- でも「**派生クラス**」からはアクセスできる

そこで `std::stack` を **public 継承**し、派生クラスの中で `this->c` を
使えば、内部コンテナの `begin()` / `end()` を呼べます。

### (c) 継承でイテレータを露出する仕組み

```cpp
template <typename T, typename Container = std::deque<T> >
class MutantStack : public std::stack<T, Container>
{
public:
    typedef typename Container::iterator iterator;   // 内部コンテナの型を借りる

    iterator begin() { return this->c.begin(); }     // c の begin をそのまま返す
    iterator end()   { return this->c.end(); }
};
```

`MutantStack` のイテレータは「**内部コンテナのイテレータそのもの**」です。
`begin()` は内部コンテナ `c` の `begin()` をただ返すだけ。これだけで
`MutantStack` が反復可能になります。

### (d) `typename` が必要な理由（依存名）

`Container::iterator` は、テンプレート引数 `Container` に依存して決まる
型（依存名）です。コンパイラはテンプレートを最初に読む時点でこれが
「型」か「値」か判断できないので、`typename` で「型だ」と明示します。

```cpp
typedef typename Container::iterator iterator;
//      ^^^^^^^^ これがないとコンパイルエラー
```

### (e) `this->` が必要な理由

`begin()` の中で `c.begin()` ではなく `this->c.begin()` と書いています。
`c` は**テンプレート基底クラス**（`std::stack<T, Container>`）のメンバです。
C++ の名前解決（two-phase lookup）の仕様上、テンプレート基底のメンバは
そのままでは見つけてもらえません。`this->` を付けると「基底クラスに
あるはず」と探してくれます。

---

## 3. ファイル構成と各ファイルの役割

```
ex02/
├── Makefile          … ビルド設定（NAME=mutantstack）
├── MutantStack.hpp   … MutantStack テンプレート（全実装。ヘッダのみ）
├── main.cpp          … テスト（PDF 例 / stack 機能 / list 差し替え / コピー）
└── README.md         … このファイル
```

`MutantStack` はテンプレートクラスなので、実装はすべてヘッダに書きます
（`.cpp` には分離できません）。クラス全体がテンプレートなので、メンバ実装を
ヘッダに書いてもルール違反になりません。

---

## 4. ビルド・実行方法

```bash
make
./mutantstack
```

期待される出力（要約）：

```
=== (1) PDF example (deque-backed) ===
top  = 17
size = 1
iterate: 5 3 5 737 0
copied to std::stack, top = 0

=== (2) still behaves like std::stack ===
empty? no
size = 3
top  = 3
reverse iterate: 3 2 1

=== (3a) reference output using std::list ===
... iterate: 5 3 5 737 0

=== (3b) same code on MutantStack backed by std::list ===
... iterate: 5 3 5 737 0      ← (3a) と完全に一致する
```

`(3b)` では `MutantStack<int, std::list<int> >` のように内部コンテナを
`std::list` に差し替えています。コードは `(1)` と同じ手順なのに、純粋な
`std::list` を直接使った `(3a)` と**同じ出力 `5 3 5 737 0`** になります。
これは「stack のロジックを変えず、土台のコンテナだけ差し替えられる」という
アダプタの利点を示しています。

---

## 5. コードの詳細解説

### イテレータ typedef

```cpp
typedef typename Container::iterator               iterator;
typedef typename Container::const_iterator         const_iterator;
typedef typename Container::reverse_iterator       reverse_iterator;
typedef typename Container::const_reverse_iterator const_reverse_iterator;
```

`MutantStack` 自身は新しいイテレータを定義しません。内部コンテナ
（`std::deque` や `std::list`）が持っているイテレータ型を**そのまま借りて
typedef する**だけです。これにより、利用者は
`MutantStack<int>::iterator` のように自然な名前で使えます。

> 補足：`std::stack<T, Container>::container_type` も `Container` と同じ
> 型です。どちらを書いても等価ですが、ここでは分かりやすさのため
> `Container::iterator` を直接使っています。

### begin() / end()（内部コンテナへのアクセス）

```cpp
iterator begin()       { return (this->c.begin()); }
iterator end()         { return (this->c.end()); }
const_iterator begin() const { return (this->c.begin()); }
const_iterator end()   const { return (this->c.end()); }
```

- `this->c` で protected な内部コンテナにアクセス（派生クラスなので可能）。
- その `begin()` / `end()` をそのまま返すだけ。
- `const` 版も用意して、`const MutantStack&` でも読み取り走査できる
  ようにしています。

逆順イテレータ（`rbegin()` / `rend()`）も同様に提供しており、てっぺんから
底へ向かって走査できます。

### OCF

```cpp
MutantStack() {}
MutantStack(const MutantStack& other) : std::stack<T, Container>(other) {}
MutantStack& operator=(const MutantStack& other) {
    std::stack<T, Container>::operator=(other);
    return *this;
}
~MutantStack() {}
```

コピーや代入は、基底クラス `std::stack` のコピー／代入に委ねるだけで
十分です（内部コンテナ `c` がまるごと複製されます）。

---

## 6. つまずきポイント / 評価で訊かれること（Q&A）

**Q. なぜ `std::stack` はそのままでは反復できないの？**
A. `std::stack` は「コンテナアダプタ」で、LIFO の操作（push/top/pop…）
だけを公開しています。`begin()`/`end()`/`iterator` を一切提供しないので、
for ループで全要素をなめる手段がありません。

**Q. では `MutantStack` はどうやって反復可能にしたの？**
A. `std::stack` が protected メンバ `c`（内部コンテナ、既定 deque）を
持っていることを利用します。public 継承して派生クラスから `this->c` に
アクセスし、その `begin()`/`end()` を露出させました。

**Q. `this->c` の `this->` は省略できない？**
A. できません。`c` はテンプレート基底クラスのメンバなので、C++ の名前解決
（two-phase lookup）の仕様上 `this->` を付けないと見つかりません。
`this->` を付けることで「基底クラスにある依存名」として探してくれます。

**Q. なぜ `typename Container::iterator` の `typename` が要るの？**
A. `Container::iterator` は `Container` に依存して決まる型（依存名）で、
コンパイラはそれが「型」か「値」か判断できません。`typename` で「型だ」と
明示する必要があります。

**Q. `std::stack<int> s(mstack);` がコンパイルできるのはなぜ？**
A. `MutantStack` は `std::stack` を public 継承しているので、`MutantStack`
は「`std::stack` の一種」として扱えます（is-a 関係）。よって
`std::stack` のコピーコンストラクタに `MutantStack` を渡せます。

**Q. 内部コンテナを list にすると何が変わる？**
A. データの持ち方（連続メモリ → リンクリスト）は変わりますが、stack
としての振る舞い（LIFO）と反復の結果は変わりません。`(3a)` と `(3b)` が
同じ `5 3 5 737 0` を出力することがその証拠です。これがアダプタの柔軟さ
です。

**Q. `MutantStack` のイテレータの正体は？**
A. 内部コンテナのイテレータそのものです。deque 版なら
`std::deque<int>::iterator`、list 版なら `std::list<int>::iterator` を
typedef して使っています。
