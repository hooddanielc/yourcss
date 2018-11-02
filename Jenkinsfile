pipeline {
    agent any

    stages {
        stage('Build') {
            steps {
                checkout scm
                sh '/bin/bash scripts/build_in_container.sh'
            }
        }
        stage('Test') {
            steps {
                sh '/bin/bash scripts/test_in_container.sh'
            }
        }
    }
}
