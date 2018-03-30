#!/usr/bin/env bash

LOGIN=$1

if [ -z ${LOGIN} ]; then
	echo "Musi byt zadany login"
	exit 1
fi

/bin/rm -rf kko.proj3.${LOGIN}
unzip kko.proj3.${LOGIN} -d .

cp run_tests.sh kko.proj3.${LOGIN}/.
cp -r dir_gif kko.proj3.${LOGIN}/.
cp -r dir_ref kko.proj3.${LOGIN}/.

mkdir kko.proj3.${LOGIN}/dir_bmp
mkdir kko.proj3.${LOGIN}/dir_log

cd kko.proj3.${LOGIN}
make -j`nproc`
./run_tests.sh ${LOGIN}
