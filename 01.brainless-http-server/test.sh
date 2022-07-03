#!/bin/bash -eu

cd $(dirname $0)

trap "pkill -P $$" EXIT

make
./a.out &

curl --silent -i http://localhost:3000 > .test-stdout
diff -u --strip-trailing-cr ./.test-stdout ./test-fixture
