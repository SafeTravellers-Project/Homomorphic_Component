pipeline {
    agent {
        node {
            label 'safetravellershetzner1'
        }
    }
    environment {
        APP_NAME = "homomorphic-engine"
        DOCKER_TAG = env.BUILD_NUMBER ?: env.GIT_COMMIT?.take(8) ?: "latest"
        DOCKER_REG = "harbor.safetravellers.rid-intrasoft.eu"
        DOCKER_REPO = "/asc/"
        DOCKER_REG_CREDS = "harbor-creds"
        FULL_IMAGE_NAME = "${DOCKER_REG}${DOCKER_REPO}${APP_NAME}:${DOCKER_TAG}"
    }
    stages {
        stage('Build Docker Image') {
            steps {
                echo "*** Building Docker image: ${FULL_IMAGE_NAME} ***"
                script {
                    // Build the Docker image
                    sh """
                        docker build -t ${FULL_IMAGE_NAME} .
                    """
                    
                    // Also tag as 'latest' if not already
                    if (DOCKER_TAG != 'latest') {
                        sh """
                            docker tag ${FULL_IMAGE_NAME} ${DOCKER_REG}${DOCKER_REPO}${APP_NAME}:latest
                        """
                    }
                }
            }
        }
        
        stage('Login and Push Docker Image') {
            steps {
                withCredentials([[$class: 'UsernamePasswordMultiBinding', credentialsId: "${DOCKER_REG_CREDS}", usernameVariable: 'USERNAME', passwordVariable: 'PASSWORD']]){
                    echo "*** Logging in to Docker registry: ${DOCKER_REG} ***"
                    sh 'docker login ${DOCKER_REG} -u ${USERNAME} -p ${PASSWORD}'
                    
                    echo "*** Pushing Docker image: ${FULL_IMAGE_NAME} ***"
                    sh 'docker push ${FULL_IMAGE_NAME}'
                    
                    // Push 'latest' tag if it was created
                    if (DOCKER_TAG != 'latest') {
                        sh 'docker push ${DOCKER_REG}${DOCKER_REPO}${APP_NAME}:latest'
                    }
                }
            }
        }
    }
    
    post {
        always {
            echo "*** Build completed for image: ${FULL_IMAGE_NAME} ***"
            // Clean up: remove local images to save space (optional)
            script {
                sh """
                    docker rmi ${FULL_IMAGE_NAME} || true
                    if [ "${DOCKER_TAG}" != "latest" ]; then
                        docker rmi ${DOCKER_REG}${DOCKER_REPO}${APP_NAME}:latest || true
                    fi
                """
            }
        }
        success {
            echo "*** Successfully built and pushed: ${FULL_IMAGE_NAME} ***"
        }
        failure {
            echo "*** Failed to build or push Docker image ***"
        }
    }
}

