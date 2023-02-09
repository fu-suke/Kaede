#!/bin/bash
test_kind="$1" # 何も指定がなかったら全体テスト

assert() {
  input="$1"
  expected="$2"

  # トークナイズテスト
  if [ "$test_kind" = "tokenize" ]; then
    echo "tokenize $input"
    ./unit_test "tokenize" "$input"
  fi
  
  # 構文解析テスト
  if [ "$test_kind" = "parse" ]; then
    echo "parse $input"
    ./unit_test "parse" "$input"
  fi

  # 全体テスト
  if [ "$test_kind" = "" ]; then
  ./kaede "$input" > tmp.s
  cc -o tmp tmp.s
  ./tmp
  actual="$?"

    if [ "$actual" = "$expected" ]; then
      echo "$input => $actual"
    else
      echo "$input => $expected expected, but got $actual"
      exit 1
    fi
  fi
}

# ビルドは一回のみ
if [ "$test_kind" = "tokenize" ]; then
  echo "tokenize test"
  make unit_test > /dev/null # ビルドの結果は出力しない
fi

if [ "$test_kind" = "parse" ]; then
  echo "parse test"
  make unit_test > /dev/null 
fi

if [ "$test_kind" = "" ]; then
  echo "all test"
  make > /dev/null
fi

assert "0;" 0
assert "42;" 42
assert "5+20-4;" 21
assert ' 12 + 34 - 5 ;' 41
assert '5+6*7;' 47
assert '5*(9-6);' 15
assert '(3+5)/2;' 4
assert '-10+20;' 10
assert '- -10;' 10
assert '- - +10;' 10

assert '0==1;' 0
assert '42==42;' 1
assert '0!=1;' 1
assert '42!=42;' 0

assert '0<1;' 1
assert '1<1;' 0
assert '2<1;' 0
assert '0<=1;' 1
assert '1<=1;' 1
assert '2<=1;' 0

assert '1>0;' 1
assert '1>1;' 0
assert '1>2;' 0
assert '1>=0;' 1
assert '1>=1;' 1
assert '1>=2;' 0


assert "a=2;b=3;a+b;" 5
assert "hoge=2;honi=3;hoge+honi;" 5
assert "return 1;" 1
assert "foo = 100 ;bar = 10; return foo - bar;" 90
assert "returnx = 5;return returnx;" 5

assert "if (1==2) return 1; else return 2;" 2
assert "if (1==1) return 1; " 1
assert "x=0;while (x<10) x=x+1; return x;" 10
# assert "y=0;for (x=1;x<4;x=x+1) y=y+x;return y;" 6

assert "if (1==1) {a=1;a=a+1;} return a;" 2
assert "if (1==1) {} a=1; return a;" 1
assert "{a=1;a=2;if(a==2) return 3;}" 3
assert "if (1==1) {a=1;a=a+1;} return a;" 2
assert "{a=1;a=2;}{a=3;a=4;}" 4

echo OK