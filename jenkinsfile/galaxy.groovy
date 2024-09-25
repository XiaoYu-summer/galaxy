def url = 'git@gitlab.gz.cvte.cn:1602/client/aoip/galaxy.git'
def branch = "${BRANCH}"
def tag = ""
def build_image = 'registry.gz.cvte.cn/1602/aoip-ubuntu:1.1.1'
def docker_registry = 'https://registry.gz.cvte.cn'

pipeline {
    agent {
        label "1602-sfu"
    }
    stages {
        stage('Checkout') {
            steps {
                cleanWs()
                checkout([$class: 'GitSCM', branches: [[name: branch]], userRemoteConfigs: [[credentialsId: 'mh1602', url: url]]])
            }
        }

        stage('Build') {
            steps {
                script {
                    echo "docker构建开始：${branch}"
                    def count = sh(script: 'git rev-list --count HEAD --no-merges', returnStdout: true).trim()
                    def hash = sh(script: 'git rev-parse --short HEAD', returnStdout: true).trim()
                    def branchx = branch.replaceAll("/", "_")
                    branchx = branchx.replaceAll("refs_heads_", "")
                    sh """
                        echo "#pragma once
#include <string>

namespace VersionInfo {
const std::string version = \\"${branchx}\\";
const std::string hash = \\"${hash}\\";
const std::string count = \\"${count}\\";
}  // namespace VersionInfo" > include/Version.h
                    """
                    withDockerRegistry([credentialsId: '6e5c1650-13f9-435e-ad7e-c0a20d0774a1', url: "${docker_registry}"]) {
                        docker.image(build_image).inside("-u root") {
                            sh '''
                                export PATH=/firefly_buildroot_rk3308_release/rk3308_linux_release_v1.5.0a_20221212/buildroot/output/firefly_rk3308_release/host/bin:$PATH
                                mkdir build
                                cd build
                                conan install .. -pr=rc-build-root --build missing -s build_type=Release
                                cmake .. -DCMAKE_BUILD_TYPE=Release --preset conan-release
                                cd Release
                                make
                            '''
                        }
                    }
                    echo "docker构建结束：${branch}"
                    // 创建构建完成标记
                    sh "touch ${WORKSPACE}/build/.build_complete"
                }
            }
        }

        stage('Package and Upload') {
            steps {
                script {
                    wrap([$class: 'BuildUser'])
                    {
                        def buildDir = "${WORKSPACE}/build/Release"
                        def buildCompleteMarker = "${WORKSPACE}/build/.build_complete"
                        
                        // 检查构建是否完成
                        if (!fileExists(buildCompleteMarker)) {
                            error "构建完成标记文件不存在，构建可能未正常完成"
                        }
                        
                        // 检查构建目录是否存在
                        if (!fileExists(buildDir)) {
                            error "构建目录 ${buildDir} 不存在，构建可能失败"
                        }
                        
                        // 检查构建产物是否存在（假设主要产物名为 'galaxy'）
                        if (!fileExists("${buildDir}/galaxy")) {
                            error "构建产物 'galaxy' 不存在，构建可能失败"
                        }

                        def timestamp = new Date().format('yyyyMMddHHmmss')
                        // 删除 ${buildDir} 下的  Makefile generators CMakeFiles CMakeCache.txt cmake_install.cmake
                        sh "rm -rf ${buildDir}/Makefile ${buildDir}/generators ${buildDir}/CMakeFiles ${buildDir}/CMakeCache.txt ${buildDir}/cmake_install.cmake"
                        def count = sh(script: 'git rev-list --count HEAD --no-merges', returnStdout: true).trim()
                        def hash = sh(script: 'git rev-parse --short HEAD', returnStdout: true).trim()
                        def branchx = branch.replaceAll("/", "_")
                        branchx = branchx.replaceAll("refs_heads_", "")
                        // 获取当前分支的hash 和 count
                        def tar_name = "galaxy_${branchx}_${hash}_${count}.tar.gz"
                        // 打包 Release 目录
                        sh "cp -r ${buildDir} ${buildDir}/../galaxy_${branchx}_${hash}_${count}"
                        sh "echo 'galaxy_${branchx}_${hash}_${count}' > ./galaxy"
                        sh "tar -czvf ${tar_name} ${buildDir}/../galaxy_${branchx}_${hash}_${count} ./galaxy"
                        sh "curl -uzhangyong1924:AP8genobRHGk28aMVufLNonDeCuZVQXr2gwk1Z -T  ./$tar_name  \"https://artifactory.gz.cvte.cn/artifactory/binaries/1602/private-be/aoip/$timestamp/$tar_name\"".replace("\n","")
                        sh """
                            curl -O https://artifactory.gz.cvte.cn:443/artifactory/npm-local/send_message/message.py
                            python3 message.py -u "${BUILD_USER_ID}" -t 'aoip-server' -c "aoip-server:https://artifactory.gz.cvte.cn/artifactory/binaries/1602/private-be/aoip/$timestamp/$tar_name" 
                            curl "http://message-board.gz.mindlinker.cn/send-message?message=aoip-server:https://artifactory.gz.cvte.cn/artifactory/binaries/1602/private-be/aoip/$timestamp/$tar_name"
                        """
                        // 清理临时文件
                        sh "rm ${tar_name}"

                        env.Gaxaly_URL = "https://artifactory.gz.cvte.cn/artifactory/binaries/1602/private-be/aoip/$timestamp/$tar_name"
                    }
                    
                }
            }
        }
    }

    // post {
    //     always {
    //         cleanWs()
    //     }
    // }
}