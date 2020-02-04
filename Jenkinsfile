pipeline {
  agent {
    label 'c++'
  }

  tools {
    jdk 'openjdk-8'
    maven '3.6'
  }

  options {
    buildDiscarder(logRotator(numToKeepStr: '60'))
  }

  triggers {
    cron '@midnight'
  }

  stages {
    stage('build') {
      steps {
        script {
          def phase = env.BRANCH_NAME == 'release/8.0' ? 'deploy' : 'verify'
          maven cmd: "clean ${phase}"
        }
        archiveArtifacts '*/target/*.zip'
       }
    }
  }
}
