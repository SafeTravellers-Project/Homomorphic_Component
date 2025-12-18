FROM alpine:3.20
 
#  Installing 'zstd' (REQUIRED for Ubuntu 24.04 packages)
RUN apk add --no-cache binutils wget gcompat grep zstd
 
WORKDIR /tmp
 
## glibc is needed beacuse
RUN BASE_URL="http://archive.ubuntu.com/ubuntu/pool/main/g/glibc/" && \
    FILE=$(wget -qO- $BASE_URL | grep -o 'libc6_2.39-[0-9a-zA-Z\.\-]*_amd64.deb' | head -n 1) && \
    echo "Downloading $FILE..." && \
    wget "$BASE_URL$FILE" -O libc6.deb
 
RUN BASE_URL="http://archive.ubuntu.com/ubuntu/pool/main/g/gcc-14/" && \
    FILE=$(wget -qO- $BASE_URL | grep -o 'libstdc++6_14[0-9\.\-]*ubuntu[0-9\.\~]*_amd64.deb' | head -n 1) && \
    echo "Downloading $FILE..." && \
    wget "$BASE_URL$FILE" -O libstdc++.deb
 
RUN BASE_URL="http://archive.ubuntu.com/ubuntu/pool/main/g/gcc-14/" && \
    FILE=$(wget -qO- $BASE_URL | grep -o 'libgcc-s1_14[0-9\.\-]*ubuntu[0-9\.\~]*_amd64.deb' | head -n 1) && \
    echo "Downloading $FILE..." && \
    wget "$BASE_URL$FILE" -O libgcc.deb
 
 
 
# extract libc6 (FIXED: Added mkdir)
RUN ar x libc6.deb && \
    zstd -d data.tar.zst && tar -xf data.tar && \
    mkdir -p /usr/glibc-compat/lib && \
    cp usr/lib/x86_64-linux-gnu/*.so* /usr/glibc-compat/lib/ && \
    rm -rf usr data.tar* control.tar.zst debian-binary libc6.deb
 
# extract libstdc++
RUN ar x libstdc++.deb && \
    zstd -d data.tar.zst && tar -xf data.tar && \
    cp usr/lib/x86_64-linux-gnu/libstdc++.so.6 /usr/glibc-compat/lib/ && \
    rm -rf usr data.tar* control.tar.zst debian-binary libstdc++.deb
 
# extract libgcc
RUN ar x libgcc.deb && \
    zstd -d data.tar.zst && tar -xf data.tar && \
    cp usr/lib/x86_64-linux-gnu/libgcc_s.so.1 /usr/glibc-compat/lib/ && \
    rm -rf usr data.tar* control.tar.zst debian-binary libgcc.deb
 
 
RUN mkdir -p /lib64 /etc/ld.so.conf.d && \
    ln -sf /usr/glibc-compat/lib/ld-linux-x86-64.so.2 /lib64/ld-linux-x86-64.so.2 && \
    echo "/usr/glibc-compat/lib" > /etc/ld.so.conf.d/glibc.conf && \
    /usr/glibc-compat/lib/ld-linux-x86-64.so.2 --library-path /usr/glibc-compat/lib /usr/glibc-compat/lib/libc.so.6 && \
    echo 'hosts: files dns' > /etc/nsswitch.conf
 
WORKDIR /app
 
COPY data /app/data
COPY bin/ /app/bin/
 
RUN mkdir -p /app/data/System_Parameters
RUN mkdir -p /app/data/CountryDB/Reg_Biometrics_Enc
RUN mkdir -p /app/data/E-Gate/Test_Biometrics_Enc
# copying custom libraries
RUN mkdir -p /app/libs
COPY all_libs/tfhe-4096/build/libtfhe/libtfhe-spqlios-avx.so /app/libs/
COPY all_libs/tfhe-operators-master/more_operations/build/lib/libmoretfheoperations.so /app/libs/
COPY all_libs/tfhe-operators-master/new_tfhe/build/lib/libnewtfhe.so /app/libs/
COPY all_libs/openssl-3.0.14/install/lib64/libcrypto.so /app/libs/
 
ENV LD_LIBRARY_PATH="/app/libs:/usr/glibc-compat/lib:$LD_LIBRARY_PATH"
 
WORKDIR /app/bin
RUN chmod +x /app/bin/*
 
RUN ./HESysInit
 
CMD ["/bin/sh"]
