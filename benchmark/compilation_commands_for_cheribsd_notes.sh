# set flags 
CC="./compile_for_cheri_cc riscv64-purecap"
# CC=gcc
FLAGS=" -D CHERIBSD_RESEARCH -D MULTIPLE_ALGORITHMS -I ../include -lm "

rm ./*.o
${CC} -c ../include/eembc_adaptation.c ${FLAGS}
${CC} -c algorithms/*.c ${FLAGS}
${CC} -c main_continuous.c ${FLAGS}
${CC} main_continuous.o a2time.o aifftr.o bitmnp.o idctrn.o puwmod.o rspeed.o tblook.o ttsprk.o eembc_adaptation.o ${FLAGS} -o multiple_algorithms_continuous_cheribsd
~/cheri/output/sdk/bin/riscv64-unknown-freebsd-objdump -sSD multiple_algorithms_continuous_cheribsd > multiple_algorithms_continuous_cheribsd.dump

rm ./*.o
FLAGS+=" -D INTRODUCE_ANOMALY "

${CC} -c ../include/eembc_adaptation.c ${FLAGS}
${CC} -c algorithms/*.c ${FLAGS}
${CC} -c main_continuous.c ${FLAGS}
${CC} main_continuous.o a2time.o aifftr.o bitmnp.o idctrn.o puwmod.o rspeed.o tblook.o ttsprk.o eembc_adaptation.o ${FLAGS} -o multiple_algorithms_continuous_anomalous_cheribsd
~/cheri/output/sdk/bin/riscv64-unknown-freebsd-objdump -sSD multiple_algorithms_continuous_anomalous_cheribsd > multiple_algorithms_continuous_anomalous_cheribsd.dump


tar -czvf multiple_algorithms_continuous_cheribsd.tar.gz multiple_algorithms_continuous_cheribsd multiple_algorithms_continuous_anomalous_cheribsd 
cp multiple_algorithms_continuous_cheribsd.tar.gz ~/cheri/extra-files-minimal/