# FROM ubuntu:24.04

# WORKDIR /app


# COPY data /app/data


# COPY bin/ /app/bin/


# COPY all_libs/tfhe-4096/build/libtfhe/libtfhe-spqlios-avx.so /app/libs/
# COPY all_libs/tfhe-operators-master/more_operations/build/lib/libmoretfheoperations.so /app/libs/
# COPY all_libs/tfhe-operators-master/new_tfhe/build/lib/libnewtfhe.so /app/libs/
# COPY all_libs/openssl-3.0.14/install/lib64/libcrypto.so /app/libs/


# ENV LD_LIBRARY_PATH="/app/libs:$LD_LIBRARY_PATH"


# RUN chmod +x /app/bin/*

# CMD ["/bin/bash"]
# --- STAGE 1: The Donor (Grab Glibc 2.39 from Ubuntu) ---
# --- STAGE 1: The Donor (Grab Glibc 2.39 from Ubuntu) ---
# --- STAGE 1: The Donor (Grab Glibc 2.39 from Ubuntu) ---

###########################################################################################3

# FROM ubuntu:24.04 AS glibc-donor


# FROM alpine:3.20

# # 1. Install gcompat (we don't need apk libstdc++ anymore, we will use the Ubuntu one)
# RUN apk add --no-cache gcompat
# ## This creates the folder structure (/lib64) and 
# ## sets up the basics so Alpine doesn't freak out when we add foreign Ubuntu files later.

# ## the directory where we will place the Glibc files
# WORKDIR /usr/glibc-compat/lib


# ## We copy the essential Glibc files from the donor Ubuntu image to our Alpine image.
# COPY --from=glibc-donor /usr/lib/x86_64-linux-gnu/libc.so.6 .
# COPY --from=glibc-donor /usr/lib/x86_64-linux-gnu/libm.so.6 .
# COPY --from=glibc-donor /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2 .


# # We grab these from Ubuntu so they have the version info your app needs
# COPY --from=glibc-donor /usr/lib/x86_64-linux-gnu/libstdc++.so.6 .
# COPY --from=glibc-donor /usr/lib/x86_64-linux-gnu/libgcc_s.so.1 .


# # We force the links and create the config
# ## mkdir -p: Creates necessary directories if they don't exist.
# ##ln -sf: Creates a shortcut so when your app asks for /lib64/ld-linux..., it gets redirected to our new file.
# ## echo: We create a config file that tells the dynamic linker where to find the Glibc libraries.
# ## Finally, we run a command to initialize Glibc and ensure everything is set up correctly

# RUN mkdir -p /lib64 /etc/ld.so.conf.d && \
#     ln -sf /usr/glibc-compat/lib/ld-linux-x86-64.so.2 /lib64/ld-linux-x86-64.so.2 && \ 
#     echo "/usr/glibc-compat/lib" > /etc/ld.so.conf.d/glibc.conf && \
#     /usr/glibc-compat/lib/ld-linux-x86-64.so.2 --library-path /usr/glibc-compat/lib /usr/glibc-compat/lib/libc.so.6 && \
#     echo 'hosts: files dns' > /etc/nsswitch.conf

# WORKDIR /app


# COPY data /app/data
# COPY bin/ /app/bin/

# # Copying your custom libraries
# RUN mkdir -p /app/libs
# COPY all_libs/tfhe-4096/build/libtfhe/libtfhe-spqlios-avx.so /app/libs/
# COPY all_libs/tfhe-operators-master/more_operations/build/lib/libmoretfheoperations.so /app/libs/
# COPY all_libs/tfhe-operators-master/new_tfhe/build/lib/libnewtfhe.so /app/libs/
# COPY all_libs/openssl-3.0.14/install/lib64/libcrypto.so /app/libs/


# ENV LD_LIBRARY_PATH="/app/libs:/usr/glibc-compat/lib:$LD_LIBRARY_PATH"


# RUN chmod +x /app/bin/*

# CMD ["/bin/sh"]




#########################################

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

# copying custom libraries
RUN mkdir -p /app/libs
COPY all_libs/tfhe-4096/build/libtfhe/libtfhe-spqlios-avx.so /app/libs/
COPY all_libs/tfhe-operators-master/more_operations/build/lib/libmoretfheoperations.so /app/libs/
COPY all_libs/tfhe-operators-master/new_tfhe/build/lib/libnewtfhe.so /app/libs/
COPY all_libs/openssl-3.0.14/install/lib64/libcrypto.so /app/libs/



ENV LD_LIBRARY_PATH="/app/libs:/usr/glibc-compat/lib:$LD_LIBRARY_PATH"

WORKDIR /app/bin
RUN chmod +x /app/bin/*

CMD ["/bin/sh"]