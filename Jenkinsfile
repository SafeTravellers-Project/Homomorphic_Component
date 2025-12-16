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
        DOCKER_REG_CREDS = "harbor-creds"
    }
    stages {
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
                withCredentials([[$class: 'UsernamePasswordMultiBinding', credentialsId: "${DOCKER_REG_CREDS}", usernameVariable: 'USERNAME', passwordVariable: 'PASSWORD']]){
                    script {
                        echo "*** Logging in to Docker registry: ${DOCKER_REG} ***"
                        // Use withCredentials to avoid exposing password in logs
                        sh """
                            echo '${PASSWORD}' | docker login ${DOCKER_REG} -u '${USERNAME}' --password-stdin
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

