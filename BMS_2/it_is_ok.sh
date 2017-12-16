#!/usr/bin/env bash

LOGIN=$1

if [ -z ${LOGIN} ]; then
	echo "Musi byt zadany login"
	exit 1
fi

/bin/rm -rf ${LOGIN}
unzip ${LOGIN} -d ${LOGIN}

cp run_tests.sh ${LOGIN}/.
cp -r dir_wav ${LOGIN}/.
cp -r dir_txt ${LOGIN}/.
cp libsndfile.a ${LOGIN}/.

cd ${LOGIN}
make -j`nproc`
#pushd ${LGIN}
./run_tests.sh
#popd



