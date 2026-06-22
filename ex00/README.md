# ex00 — easyfind

## 1. この演習でやること

「**整数を格納したコンテナ**」と「探したい整数値」を受け取り、
コンテナの中からその値が**最初に現れる位置**を探す関数テンプレート
`easyfind` を作ります。

- 見つかったら：その位置を指す**イテレータ**を返す
- 見つからなかったら：**例外を投げる**

vector でも list でも deque でも、**1 つのコードで**動くようにします。
連想コンテナ（map など）は対象外です。

```cpp
std::vector<int> v;
v.push_back(5); v.push_back(10); v.push_back(15);

std::vector<int>::iterator it = easyfind(v, 10); // 見つかる → 位置を返す
easyfind(v, 42); // 見つからない → 例外
```

---

## 2. 登場する新しい概念

### (a) コンテナとイテレータ

コンテナ（vector など）は要素を持つ「入れ物」、イテレータはその要素を
1 つ指す「矢印」です。どのコンテナも `begin()`（先頭）と `end()`
（末尾の 1 つ後ろ＝番兵）を返します。走査は次の形が基本です。

```cpp
for (std::vector<int>::iterator it = v.begin(); it != v.end(); ++it)
    std::cout << *it;  // *it で中身
```

### (b) `std::find`

`<algorithm>` のアルゴリズムです。

```cpp
std::find(first, last, value);
```

区間 `[first, last)` を先頭から走査し、`*it == value` となる**最初**の
イテレータを返します。見つからなければ `last` を返します。
`std::find` は**コンテナの種類を知りません**。`*it` / `++it` / `it != last`
というイテレータの操作だけで動くので、vector でも list でも使えます。

```cpp
std::vector<int>::iterator it = std::find(v.begin(), v.end(), 10);
if (it == v.end())
    std::cout << "not found";
else
    std::cout << "found at index " << (it - v.begin());
```

### (c) `typename` が必要な理由（依存名）

`easyfind` の戻り値の型は「コンテナ `T` のイテレータ型」、つまり
`T::iterator` です。ところがコンパイラはテンプレートを最初に読む段階では
`T` が何か分かりません。すると `T::iterator` が

- **型の名前**（`std::vector<int>::iterator` のような型）なのか
- `T` の中の**静的メンバ変数（値）**なのか

を区別できません。C++ のルールでは、こういう「`T` に依存して決まる名前
（＝**依存名**）」は、何も書かないと**値**だと解釈されます。
そこで「これは型だ」と明示するために `typename` を付けます。

```cpp
template <typename T>
typename T::iterator easyfind(T& container, int value);
//  ^^^^^^^^ これがないと「型」と認識されずコンパイルエラー
```

---

## 3. ファイル構成と各ファイルの役割

```
ex00/
├── Makefile        … ビルド設定（NAME=easyfind）
├── easyfind.hpp    … easyfind の宣言＋例外クラス。末尾で .tpp を include
├── easyfind.tpp    … easyfind の定義（テンプレートなのでヘッダ側に置く）
├── main.cpp        … テスト（見つかる/見つからない × vector/list/deque）
└── README.md       … このファイル
```

なぜ `.tpp` に分けるのか：テンプレートは「定義そのもの」が見えていないと
実体化できないので `.cpp` に分離できません。とはいえヘッダが長くなるので、
**宣言を `.hpp`、定義を `.tpp`** に分け、`.hpp` の末尾で `.tpp` を
`#include` するスタイルにしています（`.tpp` もヘッダの一部です）。

---

## 4. ビルド・実行方法

```bash
make
./easyfind
```

期待される出力（要約）：

```
=== ex00: easyfind ===
vector = [ 5 10 15 20 10 ]
found 10 in vector at index 1          ← 10 は 2 回あるが「最初」の位置(=1)
after rewriting that element: vector = [ 5 99 15 20 10 ]
exception (expected): easyfind: value not found in container

list   = [ 1 2 3 ]
found 2 in list, *it = 2
exception (expected): easyfind: value not found in container

deque  = [ 100 200 300 ]
found 300 in deque, *it = 300

found 15 in const vector at index 2
```

`make` のターゲット：`make` / `make clean` / `make fclean` / `make re`。

---

## 5. コードの詳細解説

### easyfind の中身

```cpp
template <typename T>
typename T::iterator easyfind(T& container, int value)
{
    typename T::iterator it;
    it = std::find(container.begin(), container.end(), value);
    if (it == container.end())     // end() が返った＝見つからなかった
        throw EasyFindException();
    return (it);                   // 見つかった位置を返す
}
```

ポイント：

1. `std::find` に `begin()` と `end()` を渡すだけ。検索の中身は STL に任せる。
2. 戻り値が `end()` と等しいかで「見つからなかった」を判定する。
   これは STL の検索系関数の**標準的な作法**（`end()` が「失敗」を表す）。
3. 失敗時は例外を投げる（課題の「見つからなければエラー」を満たす）。

### const 版も用意している理由

```cpp
template <typename T>
typename T::const_iterator easyfind(const T& container, int value);
```

`const` なコンテナを渡すと `begin()`/`end()` は `const_iterator` を返すため、
受け取り側も `const_iterator` でなければなりません。書き換え可能な
コンテナ用（`iterator` を返す版）と、読み取り専用用（`const_iterator` を
返す版）の 2 つを**オーバーロード**しておくと、どちらを渡しても
正しい方が自動で選ばれます。

---

## 6. つまずきポイント / 評価で訊かれること（Q&A）

**Q. なぜ `typename` が必要なの？**
A. `T::iterator` は `T` に依存して決まる名前（依存名）で、コンパイラは
それが「型」か「値」か判断できません。デフォルトでは「値」と解釈される
ので、「型だ」と教えるために `typename` を付けます。

**Q. なぜ戻り値はイテレータ？ 値やインデックスではダメ？**
A. STL の検索系（`std::find` など）は「位置＝イテレータ」を返すのが流儀
だからです。イテレータを返せば、呼び出し側はそこから中身を読んだり
（`*it`）、書き換えたり（`*it = ...`）、位置を計算したり
（`it - begin()`）と柔軟に使えます。`end()` を返すことで「見つからない」
も表現できます。

**Q. 値が複数あったらどれを返す？**
A. `std::find` は**最初に現れた**位置を返します。
本演習のテストでも 10 を 2 個入れ、index 0 ではなく最初の出現 index 1 が
返ることを確認しています。

**Q. なぜテンプレートにするの？**
A. vector / list / deque それぞれに別々の `easyfind` を書くのは無駄だから
です。型 `T` をパラメータにすれば、使われた型ごとにコンパイラが実体を
自動生成してくれます（コンテナとアルゴリズムの分離）。

**Q. map など連想コンテナは？**
A. 課題の対象外です。map は「キーで探す」ための `find()` メンバを持ち、
`std::find`（値の線形探索）とは別物なので、ここでは扱いません。

**Q. `it == container.end()` の `end()` って最後の要素？**
A. いいえ。`end()` は「最後の要素の **1 つ後ろ**」を指す番兵です。だから
`begin()` から `end()` の手前まで `++` で進むのが走査の基本形です。
