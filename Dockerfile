FROM ubuntu:24.04

WORKDIR /app


COPY data /app/data


COPY bin/ /app/bin/


COPY all_libs/tfhe-4096/build/libtfhe/libtfhe-spqlios-avx.so /app/libs/
COPY all_libs/tfhe-operators-master/more_operations/build/lib/libmoretfheoperations.so /app/libs/
COPY all_libs/tfhe-operators-master/new_tfhe/build/lib/libnewtfhe.so /app/libs/
COPY all_libs/openssl-3.0.14/install/lib64/libcrypto.so /app/libs/


ENV LD_LIBRARY_PATH="/app/libs:$LD_LIBRARY_PATH"


RUN chmod +x /app/bin/*

CMD ["/bin/bash"]
