pipeline {
    agent {
        node {
            label 'safetravellershetzner1'
        }
    }
    environment {
        APP_NAME = "homomorphic-component"
        DOCKER_REG = "harbor.safetravellers.rid-intrasoft.eu"
        DOCKER_REPO = "/security/"
        HARBOR_CREDS = credentials('harbor-creds')
    }
    stages {
        stage('Checkout SCM and Git LFS Pull') {
            steps {
                script {
                    echo "*** Checking out source code from SCM with Git LFS support ***"
                    checkout([
                        $class: 'GitSCM',
                        branches: scm.branches,
                        extensions: [
                            [$class: 'CheckoutOption', timeout: 10],
                            [$class: 'CloneOption', 
                             depth: 0, 
                             noTags: false, 
                             reference: '', 
                             shallow: false,
                             timeout: 10],
                            [$class: 'GitLFSPull']
                        ],
                        userRemoteConfigs: scm.userRemoteConfigs
                    ])
                }
            }
        }
        
        stage('Build Docker Image') {
            steps {
                script {
                    // Calculate Docker tag
                    def dockerTag = env.BUILD_NUMBER ?: (env.GIT_COMMIT ? env.GIT_COMMIT.take(8) : "latest")
                    env.DOCKER_TAG = dockerTag
                    env.FULL_IMAGE_NAME = "${DOCKER_REG}${DOCKER_REPO}${APP_NAME}:${dockerTag}"
                    
                    echo "*** Building Docker image: ${env.FULL_IMAGE_NAME} ***"
                    
                    // Build the Docker image
                    sh """
                        docker build -t ${env.FULL_IMAGE_NAME} .
                    """
                    
                    // Also tag as 'latest' if not already
                    if (dockerTag != 'latest') {
                        sh """
                            docker tag ${env.FULL_IMAGE_NAME} ${DOCKER_REG}${DOCKER_REPO}${APP_NAME}:latest
                        """
                    }
                }
            }
        }
        
        stage('Login and Push Docker Image') {
            steps {
                script {
                    echo "*** Logging in to Docker registry: ${DOCKER_REG} ***"
                    sh """
                        echo '${HARBOR_CREDS_PSW}' | docker login ${DOCKER_REG} -u '${HARBOR_CREDS_USR}' --password-stdin
                    """
                    
                    echo "*** Pushing Docker image: ${env.FULL_IMAGE_NAME} ***"
                    sh "docker push ${env.FULL_IMAGE_NAME}"
                    
                    // Push 'latest' tag if it was created
                    if (env.DOCKER_TAG != 'latest') {
                        sh "docker push ${DOCKER_REG}${DOCKER_REPO}${APP_NAME}:latest"
                    }
                }
            }
        }
    }
    
    post {
        always {
            script {
                echo "*** Build completed for image: ${env.FULL_IMAGE_NAME} ***"
                // Clean up: remove local images to save space (optional)
                sh """
                    docker rmi ${env.FULL_IMAGE_NAME} || true
                    if [ "${env.DOCKER_TAG}" != "latest" ]; then
                        docker rmi ${DOCKER_REG}${DOCKER_REPO}${APP_NAME}:latest || true
                    fi
                """
            }
        }
        success {
            script {
                echo "*** Successfully built and pushed: ${env.FULL_IMAGE_NAME} ***"
            }
        }
        failure {
            echo "*** Failed to build or push Docker image ***"
        }
    }
}

