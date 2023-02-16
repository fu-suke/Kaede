#!/bin/bash
test_kind="$1" # 何も指定がなかったら全体テスト

assert() {
  input="$1"
  expected="$2"

  # トークナイズテスト
  if [ "$test_kind" = "tokenize" ]; then
    echo "tokenize $input"
    ./unit_test "tokenize" "$input" 
    if [ !"$?" = 0 ]; then
    exit 1
    fi
  fi
  
  # 構文解析テスト
  if [ "$test_kind" = "parse" ]; then
    echo "parse $input"
    ./unit_test "parse" "$input" 
    if [ ! "$?" = 0 ]; then
    exit 1
    fi
  fi

  # 全体テスト
  if [ "$test_kind" = "" ]; then
  ./kaede "$input" > tmp.s
  cc -o tmp tmp.s print.o
  print_result=`./tmp`
  actual="$?"

    if [ "$actual" = "$expected" ]; then
      echo "$input => $actual"
      if [ "$print_result" != "" ]; then
        echo "print: "
        echo "$print_result"
      fi
    else
      echo "$input => $expected expected, but got $actual"
      exit 1
    fi
    echo ""
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

assert "a=1;" 1
assert "a=2;b=3;a+b;" 5
assert "hoge=2;honi=3;hoge+honi;" 5
assert "return 1;" 1
assert "foo = 100 ;bar = 10; return foo - bar;" 90
assert "returnx = 5;return returnx;" 5

assert "if (1==2) return 1; else return 2;" 2
assert "if (1==1) return 1; " 1
assert "x=0;while (x<10) x=x+1; return x;" 10

assert "if (1==1) {a=1;a=a+1;} return a;" 2
assert "{a=1;a=2;if(a==2) return 3;}" 3
assert "if (1==1) {a=1;a=a+1;} return a;" 2
assert "{a=1;a=2;}{a=3;a=4;}" 4
assert "{a=1; {a=a+1;} return a;}" 2

assert "y=0;for (x=1;x<4;x=x+1) y=y+x;return y;" 6
assert "y=0;x=1;for (;x<4;x=x+1) y=y+x;return y;" 6
assert "x=0;y=0;for(;y<=2;){{x=x+y;}y=y+1;}return x;" 3
assert "x=0;for(;x<4;x=x+1) {if(x==2) {1;}} return x;" 4
assert "x=0; if(x==2) 1; return x;" 0
assert "x=0; if(x==2) {1;} return x;" 0
assert "x=0; if(x==0) {x=2;} return x;" 2
assert "x=0; x==0;" 1
assert "x=0; if(x==0) return 1; else return 2;" 1
assert "x=0; if(x!=0) return 1; else return 2;" 2
assert "for(x=0;x<10;x=x+1) if(x==3)return x;" 3
assert "a=0;for(x=0;x<10;x=x+1) a = a+2; a;" 20
assert "foo();return 1;" 1
assert "for(x=0;x<10;x=x+1) foo(); x;" 10
assert "a=1;b=2;sum(a,b);return 1;" 1
assert "i=0;while(i<5){sum(2,i); i=i+1;} i;" 5
echo OK