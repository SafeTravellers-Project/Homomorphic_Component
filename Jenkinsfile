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

    agent any

    environment {
        IMAGE_NAME     = "safetravellers-api"
        HARBOR_REGISTRY = "${env.HARBOR_REGISTRY ?: 'harbor.safetravellers.example.com'}"
        HARBOR_PROJECT  = "${env.HARBOR_PROJECT  ?: 'safetravellers'}"
        FULL_IMAGE      = "${HARBOR_REGISTRY}/${HARBOR_PROJECT}/${IMAGE_NAME}"
        GIT_SHORT_SHA   = ""   // populated in Checkout stage
    }

    options {
        timestamps()
        timeout(time: 60, unit: 'MINUTES')
        disableConcurrentBuilds()
    }

    stages {

        // -------------------------------------------------------------------
        stage('Checkout') {
        // -------------------------------------------------------------------
            steps {
                checkout scm
                script {
                    env.GIT_SHORT_SHA = sh(
                        script: "git rev-parse --short HEAD",
                        returnStdout: true
                    ).trim()
                    echo "Building commit: ${env.GIT_SHORT_SHA}"
                }
            }
        }

        // -------------------------------------------------------------------
        stage('Build Image') {
        // -------------------------------------------------------------------
            steps {
                script {
                    docker.build(
                        "${env.FULL_IMAGE}:${env.GIT_SHORT_SHA}",
                        "--pull ."
                    )
                }
            }
        }

        // -------------------------------------------------------------------
        stage('Test Health') {
        // -------------------------------------------------------------------
            steps {
                script {
                    // Start the container with a throw-away API key
                    def containerId = sh(
                        script: """
                            docker run -d --rm \
                              -p 18080:8080 \
                              -e SAFE_API_KEY=ci-test-key \
                              --name safetravellers-ci-test \
                              ${env.FULL_IMAGE}:${env.GIT_SHORT_SHA}
                        """,
                        returnStdout: true
                    ).trim()

                    try {
                        // Wait for the API to become ready (up to 30 s)
                        timeout(time: 30, unit: 'SECONDS') {
                            waitUntil {
                                def status = sh(
                                    script: "curl -s -o /dev/null -w '%{http_code}' http://localhost:18080/health",
                                    returnStdout: true
                                ).trim()
                                return status == '200'
                            }
                        }
                        echo "Health check passed."
                    } finally {
                        sh "docker stop safetravellers-ci-test || true"
                    }
                }
            }
        }

        // -------------------------------------------------------------------
        stage('Push to Harbor') {
        // -------------------------------------------------------------------
            steps {
                script {
                    docker.withRegistry("https://${env.HARBOR_REGISTRY}", 'harbor-credentials') {
                        def img = docker.image("${env.FULL_IMAGE}:${env.GIT_SHORT_SHA}")

                        // Always push the commit-SHA tag
                        img.push("${env.GIT_SHORT_SHA}")

                        // Push "latest" only from the default branch
                        if (env.BRANCH_NAME == 'main' || env.BRANCH_NAME == 'master') {
                            img.push("latest")
                        }

                        // Push a semver tag if a Git tag is present on this commit
                        def gitTag = sh(
                            script: "git tag --points-at HEAD | grep -E '^v[0-9]' | head -1 || true",
                            returnStdout: true
                        ).trim()
                        if (gitTag) {
                            img.push(gitTag)
                            echo "Also pushed tag: ${gitTag}"
                        }
                    }
                }
            }
        }

        // -------------------------------------------------------------------
        stage('Cleanup') {
        // -------------------------------------------------------------------
            steps {
                sh "docker rmi ${env.FULL_IMAGE}:${env.GIT_SHORT_SHA} || true"
                sh "docker image prune -f || true"
            }
        }
    }

    post {
        success {
            echo "Image pushed successfully: ${env.FULL_IMAGE}:${env.GIT_SHORT_SHA}"
        }
        failure {
            echo "Pipeline failed. Check the logs above."
        }
        always {
            // Ensure the test container is not left running on any failure
            sh "docker stop safetravellers-ci-test || true"
        }
    }
}
