pipeline {
  agent {
    dockerfile true
  }

  options {
    buildDiscarder(logRotator(artifactNumToKeepStr: '20'))
  }

  triggers {
    cron '@midnight'
    pollSCM 'H/5 * * * *'
  }

  stages {
    stage('build') {
      steps {
      	script {
          sh 'make'          
        }
        archiveArtifacts '**/*.dll, **/*.exe'
      }
    }
  }
}