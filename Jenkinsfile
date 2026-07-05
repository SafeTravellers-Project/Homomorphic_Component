// ---------------------------------------------------------------------------
// Jenkinsfile — SAFETravellers HE Web Service
//
// Pipeline stages:
//   1. Checkout        — clone from GitHub
//   2. Build Image     — docker build from Dockerfile
//   3. Test Health     — start container, hit /health, shut down
//   4. Push to Harbor  — tag and push to the SAFETravellers Harbor registry
//   5. Cleanup         — remove local image
//
// Required Jenkins credentials (configure in Jenkins → Manage Credentials):
//   harbor-credentials   : Username/Password for the Harbor registry
//
// Required Jenkins environment variables (configure in Jenkins → System):
//   HARBOR_REGISTRY      : Harbor registry host, e.g. harbor.safetravellers.example.com
//   HARBOR_PROJECT       : Harbor project name, e.g. safetravellers
//
// The pipeline tags the image with both the Git commit SHA and "latest".
// On the main branch it also tags with the version from the git tag if present.
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
    }

    options {
        timestamps()
        timeout(time: 60, unit: 'MINUTES')
        disableConcurrentBuilds()
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

        stage('Build TFHE') {
            steps {
                sh '''
                    set -eux

                    if [ ! -d all_libs/tfhe/src ]; then
                      rm -rf all_libs/tfhe
                      git clone https://github.com/tfhe/tfhe.git all_libs/tfhe
                    fi

                    cd all_libs/tfhe/src

                    cmake -S . -B ../build \
                      -DCMAKE_BUILD_TYPE=Release \
                      -DCMAKE_C_FLAGS="-march=native -mtune=native" \
                      -DCMAKE_CXX_FLAGS="-march=native -mtune=native"

                    cmake --build ../build -j$(nproc)

                    test -f ../build/libtfhe/libtfhe-spqlios-avx.so
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
            echo "Pushed: ${env.FULL_IMAGE}:${env.BUILD_NUMBER} and latest"
        }

        failure {
            echo "Build or push failed."
        }
    }
}
