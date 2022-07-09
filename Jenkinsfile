pipeline {
    agent any
    stages {
        stage('Multi-platform build and test') {
            matrix {
                agent {
                    node {
                        label ''
                        customWorkspace "workspace/$JOB_NAME/$PLATFORM"
                    }
                }
                axes {
                    axis {
                        name 'PLATFORM'
                        values 'lin', 'win'
                    }
                }
                stages {
                    stage('Build') {
                        steps {
                            sh "${makeCommand} dist PLATFORM=$PLATFORM"
                        }
                    }
                    stage('Test') {
                        steps {
                            sh "${makeCommand} check PLATFORM=$PLATFORM"
                        }
                    }
                    stage('Archive') {
                        steps {
                            archiveArtifacts artifacts: 'pksv*.zip', fingerprint: true, onlyIfSuccessful: true
                        }
                    }
                }
            }
        }
    }
}

def getMakeCommand() {
    if (env.PLATFORM == 'win') {
        return 'xvfb-run -- ./make-mingw 32'
    }
    return 'make'
}
