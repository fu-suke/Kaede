#!/bin/bash
assert() {
  input="$1"
  expected="$2"

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
}

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


assert "a=2;b=3;a+b;" 5;

echo OK