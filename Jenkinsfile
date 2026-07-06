// ---------------------------------------------------------------------------
// Jenkinsfile — SAFETravellers HE Web Service
//
// Flow:
//   1. Checkout        — Jenkins pulls HE_module_v4 from GitHub job SCM config
//   2. Clean           — remove stale local build artifacts
//   3. Build SEAL      — build Microsoft SEAL locally inside workspace
//   4. Build TFHE      — clone/build TFHE locally if needed
//   5. Validate libs   — verify required libraries exist
//   6. Build binaries  — build HESysInit, Register, EncBio, Verify
//   7. Docker build    — build image from Dockerfile
//   8. Push Harbor     — push BUILD_NUMBER and latest tags
//
// Required Jenkins credentials:
//   harbor-creds : Username/Password for Harbor registry
//
// Jenkins job config:
//   Repo   : https://github.com/SafeTravellers-Project/Homomorphic_Component.git
//   Branch : */HE_module_v4
//   Script : Jenkinsfile
//
// ---------------------------------------------------------------------------

pipeline {
    agent {
        node {
            label 'safe-tr-tumbano'
        }
    }

    environment {
        IMAGE_NAME       = "safetravellers-api"
        HARBOR_REGISTRY = "harbor.safetravellers.rid-intrasoft.eu"
        HARBOR_PROJECT  = "security"
        FULL_IMAGE      = "${HARBOR_REGISTRY}/${HARBOR_PROJECT}/${IMAGE_NAME}"
        HARBOR_CREDS    = credentials('harbor-creds')

        BUILD_FLAGS     = "-march=x86-64-v3 -mtune=generic"
    }

    options {
        timestamps()
        timeout(time: 60, unit: 'MINUTES')
        disableConcurrentBuilds()
        buildDiscarder(logRotator(numToKeepStr: '20'))
    }

    stages {
        stage('Checkout') {
            steps {
                checkout scm
                sh '''
                    set -eux
                    git rev-parse --short HEAD
                    ls -la
                '''
            }
        }

        stage('Clean Build Artifacts') {
            steps {
                sh '''
                    set -eux
                    rm -rf build
                    rm -rf all_libs/SEAL/build
                    rm -rf all_libs/tfhe/build
                    mkdir -p bin
                '''
            }
        }

        stage('Build SEAL') {
            steps {
                sh '''
                    set -eux

                    test -d all_libs/SEAL

                    cmake -S all_libs/SEAL -B all_libs/SEAL/build \
                      -DCMAKE_BUILD_TYPE=Release \
                      -DCMAKE_C_FLAGS="$BUILD_FLAGS" \
                      -DCMAKE_CXX_FLAGS="$BUILD_FLAGS" \
                      -DSEAL_BUILD_EXAMPLES=OFF \
                      -DSEAL_BUILD_TESTS=OFF

                    cmake --build all_libs/SEAL/build -j$(nproc)

                    test -f all_libs/SEAL/build/lib/libseal-4.1.a
                    test -d all_libs/SEAL/build/cmake
                '''
            }
        }

        stage('Build TFHE') {
            steps {
                sh '''
                    set -eux

                    if [ ! -d all_libs/tfhe/src ]; then
                      rm -rf all_libs/tfhe
                      git clone https://github.com/tfhe/tfhe.git all_libs/tfhe
                    fi

                    cmake -S all_libs/tfhe/src -B all_libs/tfhe/build \
                      -DCMAKE_BUILD_TYPE=Release \
                      -DCMAKE_C_FLAGS="$BUILD_FLAGS" \
                      -DCMAKE_CXX_FLAGS="$BUILD_FLAGS"

                    cmake --build all_libs/tfhe/build -j$(nproc)

                    test -f all_libs/tfhe/src/include/tfhe.h
                    test -f all_libs/tfhe/build/libtfhe/libtfhe-spqlios-avx.so
                '''
            }
        }

        stage('Validate Required Libraries') {
            steps {
                sh '''
                    set -eux

                    test -f all_libs/tfhe/build/libtfhe/libtfhe-spqlios-avx.so
                    test -f all_libs/tfhe-operators-master/more_operations/build/lib/libmoretfheoperations.so
                    test -f all_libs/tfhe-operators-master/new_tfhe/build/lib/libnewtfhe.so
                    test -f all_libs/openssl-3.0.14/install/lib64/libcrypto.so
                '''
            }
        }

        stage('Build Main Binaries') {
            steps {
                sh '''
                    set -eux

                    cmake -S . -B build \
                      -DCMAKE_BUILD_TYPE=Release \
                      -DCMAKE_C_FLAGS="$BUILD_FLAGS" \
                      -DCMAKE_CXX_FLAGS="$BUILD_FLAGS"

                    cmake --build build -j$(nproc)

                    test -x bin/HESysInit
                    test -x bin/Register
                    test -x bin/EncBio
                    test -x bin/Verify
                '''
            }
        }

        stage('Docker Build') {
            steps {
                sh '''
                    set -eux

                    docker build --pull --no-cache \
                      -t "$FULL_IMAGE:$BUILD_NUMBER" \
                      -t "$FULL_IMAGE:latest" \
                      .
                '''
            }
        }

        stage('Push to Harbor') {
            steps {
                sh '''
                    set -eux

                    echo "$HARBOR_CREDS_PSW" | docker login "$HARBOR_REGISTRY" \
                      -u "$HARBOR_CREDS_USR" \
                      --password-stdin

                    docker push "$FULL_IMAGE:$BUILD_NUMBER"
                    docker push "$FULL_IMAGE:latest"
                '''
            }
        }
    }

    post {
        always {
            sh '''
                docker logout "$HARBOR_REGISTRY" || true
                docker image prune -f || true
            '''
        }

        success {
            echo "Pushed: ${env.FULL_IMAGE}:${env.BUILD_NUMBER} and ${env.FULL_IMAGE}:latest"
        }

        failure {
            echo "Build or push failed."
        }
    }
}
