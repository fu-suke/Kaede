input="$1"

./kaede "$input" > tmp.s
cc -o tmp tmp.s
./tmp

echo $?