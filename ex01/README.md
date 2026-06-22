# ex01 — Span

## 1. この演習でやること

最大 N 個の `int` を格納できる `Span` クラスを作ります。

- `Span(unsigned int n)` … 上限 N を決めて作る
- `addNumber(int)`        … 1 個追加（満杯なら例外）
- `addRange(begin, end)`  … **イテレータ区間で一括追加**（テンプレートメンバ）
- `shortestSpan()`        … 格納値のうち「最も近い 2 値の差」を返す
- `longestSpan()`         … 格納値のうち「最も離れた 2 値の差」（= 最大 − 最小）を返す
- 要素が 0 個か 1 個のときに span を求めようとすると例外

PDF 指定の最小テストで `2` と `14` が出ることを確認します。
さらに**1 万個以上**の大量データでも動作することを確認します。

```cpp
Span sp = Span(5);
sp.addNumber(6); sp.addNumber(3); sp.addNumber(17);
sp.addNumber(9); sp.addNumber(11);
std::cout << sp.shortestSpan() << std::endl; // 2
std::cout << sp.longestSpan()  << std::endl; // 14
```

---

## 2. 登場する新しい概念

### (a) `std::vector` の使い方

`std::vector<int>` は「動的配列」です。要素を末尾に足したり、サイズを
取ったり、添字でアクセスできます。

```cpp
std::vector<int> v;
v.push_back(10);          // 末尾に追加
v.push_back(20);
std::cout << v.size();    // 要素数 → 2
std::cout << v[0];        // 添字アクセス → 10
v.reserve(1000);          // 1000 個ぶん前もって確保（後述）
```

本演習では `Span` の内部実装に `std::vector<int>` を使い、生配列を
手書きしません。

### (b) `std::sort` と隣接差（最短スパンの肝）

「最も近い 2 値」を素朴に求めると、全ペアを比べる O(n²) になります。
しかし**値をソートすると、最も近い 2 値は必ず隣り合います**。

```
6 3 17 9 11  ──ソート──▶  3 6 9 11 17
                隣接差:    3 3  2  6
                          最小は 2 ← これが shortestSpan
```

これにより「ソート O(n log n) → 隣り合う差の最小 O(n)」で求まります。
隣接差の計算には `std::adjacent_difference` を使います。

```cpp
std::sort(v.begin(), v.end());        // 並べ替え
std::adjacent_difference(v.begin(), v.end(), diffs.begin());
// diffs[0] = v[0] (先頭値そのまま), diffs[i>=1] = v[i] - v[i-1]
```

### (c) 範囲を取るテンプレートメンバ関数 / InputIterator

`addRange(begin, end)` は「区間で一括追加」しますが、渡されるイテレータの
型は呼ぶ側によって変わります（vector の iterator, list の iterator,
生ポインタ `int*` …）。どれでも受けられるように、**メンバ関数自身を
テンプレート化**します。

```cpp
template <typename InputIterator>
void addRange(InputIterator begin, InputIterator end);
```

`InputIterator` は「前から 1 回ずつ読みながら `++` で進めればよい」という
最小要件のイテレータです。これを要求しておけば、ほぼあらゆるコンテナの
区間を受け取れます。標準コンテナの `assign(first, last)` や
`insert(pos, first, last)` も、まさにこの形のテンプレートメンバです。

---

## 3. ファイル構成と各ファイルの役割

```
ex01/
├── Makefile     … ビルド設定（NAME=span）
├── Span.hpp     … Span クラスと例外クラスの宣言。末尾で .tpp を include
├── Span.tpp     … テンプレートメンバ addRange の定義（ヘッダ側に必要）
├── Span.cpp     … 非テンプレートメンバの実装（OCF, addNumber, span 計算）
├── main.cpp     … テスト（PDF 例 / 例外 / addRange / 2 万個 / コピー）
└── README.md    … このファイル
```

ルール「テンプレート以外の実装はヘッダに書かない」を守るため、
`addRange`（テンプレート）だけ `.tpp` に、それ以外は `.cpp` に置いています。

---

## 4. ビルド・実行方法

```bash
make
./span
```

期待される出力（要約）：

```
--- (1) PDF example ---
shortestSpan = 2
longestSpan  = 14

--- (2) exceptions ---
add to full (expected): Span: container is already full
shortest on empty (expected): Span: need at least 2 numbers to compute a span
longest on size=1 (expected): Span: need at least 2 numbers to compute a span

--- (3) addRange ---
after addRange(vector): size = 6
shortestSpan = 1
longestSpan  = 38
... （list / 生ポインタ / 上限超え例外）

--- (4) large test (10000+ numbers) ---
filled 20000 numbers (addNumber)
shortestSpan = ...
longestSpan  = ...
filled 20000 numbers (addRange)
...
```

### 1 万個テストについて

`main.cpp` の `(4)` では `N = 20000` 個（1 万個より多い）を 2 通りで詰めて
います。

- `addNumber` を 20000 回呼ぶ（1 個ずつ）
- `addRange` で 20000 個を一括追加

どちらでも `shortestSpan()` / `longestSpan()` が高速に計算できることを
確認しています（ソートと min/max は STL 任せなので一瞬で終わります）。
値の範囲は `0..999999` にしてあるので、`longestSpan`（最大−最小）で
`int` のオーバーフローは起きません。

---

## 5. コードの詳細解説

### shortestSpan（なぜソートして隣接差なのか）

```cpp
unsigned int Span::shortestSpan(void) const
{
    if (_numbers.size() < 2)
        throw NotEnoughNumbersException();   // 0,1 個では差が定義できない

    std::vector<int> sorted(_numbers);       // 元を壊さないようコピー
    std::sort(sorted.begin(), sorted.end()); // 昇順に並べ替え

    std::vector<int> diffs(sorted.size());
    std::adjacent_difference(sorted.begin(), sorted.end(), diffs.begin());
    // diffs[0] は先頭値そのもの（スパンではない）→ 無視する
    // diffs[i>=1] = sorted[i] - sorted[i-1] = 隣り合う差

    std::vector<int>::iterator minIt =
        std::min_element(diffs.begin() + 1, diffs.end()); // 1 番目以降の最小
    return (static_cast<unsigned int>(*minIt));
}
```

**なぜ正しいのか**：最短スパンは「すべてのペアの差の最小」。ソート後の
配列で、いちばん近い 2 値は必ず隣同士になります（間に別の値があれば、
そちらの方が近いはずなので矛盾）。だから「隣接差の最小」を見れば十分です。

`std::adjacent_difference` の `diffs[0]` は仕様上「先頭要素そのまま」なので、
スパンとしては意味がありません。よって `diffs.begin() + 1` から
`std::min_element` で最小を取ります。ソート済みなので隣接差は必ず 0 以上
で、`unsigned int` に安全にキャストできます。

### longestSpan（最大 − 最小）

```cpp
unsigned int Span::longestSpan(void) const
{
    if (_numbers.size() < 2)
        throw NotEnoughNumbersException();

    std::vector<int>::const_iterator minIt =
        std::min_element(_numbers.begin(), _numbers.end());
    std::vector<int>::const_iterator maxIt =
        std::max_element(_numbers.begin(), _numbers.end());
    return (static_cast<unsigned int>(*maxIt - *minIt));
}
```

最も離れた 2 値は「最小値」と「最大値」です。`std::min_element` /
`std::max_element` で一発で取れます。ソートは不要です（両端だけ分かれば
よいので O(n)）。

### addRange（範囲一括追加）

```cpp
template <typename InputIterator>
void Span::addRange(InputIterator begin, InputIterator end)
{
    std::size_t count = static_cast<std::size_t>(std::distance(begin, end));

    if (_numbers.size() + count > static_cast<std::size_t>(_maxSize))
        throw SpanFullException();           // 入りきらないなら何もせず例外

    _numbers.insert(_numbers.end(), begin, end); // 末尾へ一括挿入
}
```

- `std::distance(begin, end)` … 区間の要素数を返す STL 関数。ランダム
  アクセスなら `end - begin` で O(1)、list などでも `++` を数えて求めて
  くれるので、生ポインタの引き算より移植性が高いです。
- まず件数を数え、**上限を超えるなら 1 個も足さずに例外**を投げます
  （all-or-nothing。中途半端に足してから失敗しない）。
- `vector::insert(pos, first, last)` で区間を末尾へ一括挿入します。
  1 個ずつ `push_back` するより、必要量をまとめて確保できるので効率的です。

### reserve の効果

```cpp
Span::Span(unsigned int n) : _maxSize(n), _numbers()
{
    _numbers.reserve(n);   // 最初に n 個分のメモリを確保
}
```

`reserve(n)` は「これから最大 n 個入る」と vector に前もって伝え、内部
メモリを一括確保します。これをしないと、`push_back` を繰り返すうちに
容量が足りなくなるたびに**より大きい領域を確保し直し、既存要素を全部
コピー（移動）**します。`reserve` で最初に確保しておけば、この再確保が
起きず、特に 1 万個追加するときに効きます。

---

## 6. つまずきポイント / 評価で訊かれること（Q&A）

**Q. なぜ最短スパンはソートして隣接差なの？ 全ペア比較じゃダメ？**
A. 全ペア比較は O(n²) で 1 万個だと 1 億回。ソート後の隣接差なら
O(n log n) です。「最も近い 2 値はソートすると必ず隣り合う」ので、
隣接差の最小だけ見れば正しい答えになります。

**Q. `addRange` で上限を超えたらどうなる？**
A. **1 個も追加せず**に例外を投げます（all-or-nothing）。先に
`std::distance` で件数を数え、入りきるか確認してから挿入するためです。

**Q. `reserve` を呼ぶ意味は？**
A. メモリの再確保とそれに伴う全要素コピーを防ぎ、大量追加を高速化します。
`reserve` は容量（capacity）を増やすだけで、`size()`（要素数）は変えません。

**Q. イテレータ無効化とは？**
A. vector に要素を追加して内部メモリが再確保されると、それ以前に取って
おいた**イテレータやポインタが無効になる**現象です。本実装では span 計算の
たびにその場でイテレータを取得し、保持し続けないので問題は起きません。
また `reserve(N)` で最初に容量を確保しているため、N 個までの `addNumber`
では再確保自体が起きません。

**Q. `longestSpan` で `*maxIt - *minIt` がオーバーフローしない？**
A. 値の範囲によっては理論上あり得ます（例：`INT_MAX - INT_MIN`）。本演習の
テストでは値域を `0..999999` に絞り、差が `int` に収まるようにしています。

**Q. `addRange` はなぜテンプレートメンバなの？**
A. 渡されるイテレータの型（vector の iterator / list の iterator / 生
ポインタ …）が呼ぶ側で変わるからです。テンプレートにすれば、どの型の
イテレータ区間でも 1 つの実装で受け取れます。標準コンテナの
`insert(pos, first, last)` と同じ設計です。

**Q. なぜ `Span(unsigned int)` に `explicit` を付けた？**
A. `Span s = 5;` のような「int から Span への暗黙変換」を禁止するためです。
上限という重要な値を、うっかり別の数値から作ってしまう事故を防ぎます。
