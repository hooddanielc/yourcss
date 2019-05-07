pipeline {
  agent any

  stages {
    stage('Build') {
      agent { label 'ib' }
      steps {
        sh './scripts/build.sh'
      }
    }

    stage('Test') {
      agent { label 'ib' }
      steps {
        sh './scripts/test.sh'
      }
    }
  }
}
