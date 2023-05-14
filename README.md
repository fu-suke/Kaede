# Kaedeコンパイラ

Kaedeコンパイラは、C言語っぽいオリジナルのプログラミング言語"Kaede"をアセンブリ言語に変換するコンパイラです。
C言語で記述されており、トークナイズ、パース、そしてアセンブリ言語への変換という一般的なコンパイラの動作を行います。

## 主なファイルの説明

kaede.h: Kaedeコンパイラの主要なデータ構造と関数の宣言が含まれています。

tokenize.c: ソースコードをトークンに分割するための関数が含まれています。

parse.c: トークンを抽象構文木に変換するための関数が含まれています。

codegen.c: 抽象構文木をアセンブリ言語に変換するための関数が含まれています。

main.c: 上記の各コンポーネントを統合し、コマンドライン引数として与えられたソースコードをトークナイズ、パースし、そしてアセンブリ言語へ変換します。

## コードの書き方

大まかな部分はC言語と一致しています。しかし自習用に作成したものなので多くの制約があったりします。Kaedeでできることを記載していきます。

0. ITF()関数：Imagine The Future. 制作者たちが所属する某大学のキャッチフレーズ的なアレです。C言語のmain関数に相当します。プログラムのエントリーポイントとして使用されます。

1. 変数：変数を定義し、その値を取得したり新たな値を代入したりすることができます。変数には8byteの整数のみ代入できます。それ以上の値はうまく計算できません。また、C言語と異なり型の宣言は不要です。型の宣言をするとコンパイルエラーになります。
    ```c
    x = 3;
    y = x + 5;
    ```

2. 算術演算・比較演算：基本的な算術演算(+, -, *, /)と比較演算(==, !=, <, <=, >, >=)を使用することができます。また、インクリメント(++)や代入と算術演算を同時に行う演算(+=, -=, *=, /=)も行うことができます。

3. 制御構文：制御構文: if、while、forの制御構文を使用することができます。
    ```c
    i=0;if(i==0){while(i<5){i=i+1;}} // i=5
    for(x=0;x<10;x=x+1) if(x==3)return x; // 3
    ```

4. 関数定義：関数を定義することができます。ソースコード中で定義できる関数は「引数なし」の関数に限ります。引数ありの関数を定義したい場合は「func_def.c」にて定義することができます。引数は最大6つです。
    ```c
    func(){x=3; return x;} // 引数なしの関数
    ```
5. 関数呼び出し：定義した関数を呼び出すことができます。
    ```c
    func(){x=3; return x;} ITF(){ x = func(); return x;} // 3
    ```
6. return文：関数の実行結果をreturn文で返すことができます。


## 使い方

1. まず、makeコマンドを用いてKaedeコンパイラをビルドします。

    ```bash
    $ make kaede
    ```
2. 次に、コンパイラにソースコードを渡してアセンブリ言語を出力します。

    ```bash
    $ ./kaede "func(){x=3; return x;} ITF(){ x = 10; func(); return x;}" > tmp.s
    ```

3. gccを使ってアセンブリからバイナリを生成します。

    ```bash
    $ gcc tmp.s -o tmp
    ```

4. 最後に、バイナリを実行して結果を得ます。

    ```
    $ ./tmp
    $ echo $?
    3
    ```

    上記のコマンドを実行すると、プログラムの実行結果（ITF関数の最後にreturnされた値）が表示されます。もしreturnされた値がなければ、raxレジスタに最後に入っている値が表示されます。

## 参考にさせていただいたサイト

* 『低レイヤを知りたい人のためのCコンパイラ作成入門』
https://www.sigbus.info/compilerbook