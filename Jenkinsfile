pipeline {
  agent {
    label 'c++'
  }

  tools {
    jdk '1.8.latest'
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
          def phase = env.BRANCH_NAME == 'master' ? 'deploy' : 'verify'
          maven cmd: "clean ${phase}"
        }
        archiveArtifacts '*/target/*.zip'
       }
    }
  }
}
