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
                    git rev-parse --abbrev-ref HEAD || true
                    git rev-parse --short HEAD || true
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

        stage('Test Health') {
            steps {
                sh '''
                    set -eux

                    docker rm -f safetravellers-ci-test || true

                    docker run -d \
                      -p 18080:8080 \
                      -e SAFE_API_KEY=ci-test-key \
                      --name safetravellers-ci-test \
                      "$FULL_IMAGE:$BUILD_NUMBER"

                    for i in $(seq 1 30); do
                      status=$(curl -s -o /dev/null -w '%{http_code}' http://localhost:18080/health || true)
                      if [ "$status" = "200" ]; then
                        echo "Health check passed."
                        exit 0
                      fi
                      sleep 1
                    done

                    docker logs safetravellers-ci-test || true
                    exit 1
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
                docker rm -f safetravellers-ci-test || true
                docker logout "$HARBOR_REGISTRY" || true
                docker rmi "$FULL_IMAGE:$BUILD_NUMBER" || true
                docker rmi "$FULL_IMAGE:latest" || true
            '''
        }

        success {
            echo "Pushed: ${env.FULL_IMAGE}:${env.BUILD_NUMBER} and latest"
        }

        failure {
            echo "Pipeline failed."
        }
    }
}
