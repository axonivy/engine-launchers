pipeline {
  agent {
    label 'windows'
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
          maven cmd: "-f pom-install-visual.xml generate-resources"

          def phase = env.BRANCH_NAME == 'master' ? 'deploy' : 'verify'
          maven cmd: "clean ${phase} -Dtools.microsoft.visualstudio.8='${env.WORKSPACE}/visual"
        }
        archiveArtifacts '*/target/*.zip'
       }
    }
  }
}
