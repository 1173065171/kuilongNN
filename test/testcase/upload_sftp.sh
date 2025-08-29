#!/bin/sh
# 用法: ./sftp_upload.sh 本地文件 远端文件名

# 服务器配置
USER="zbn"
HOST="10.0.0.46"
REMOTE_DIR="/home/zbn/code/npu_lpnpu/testcase/minsys"

LOCAL_DIR="$1"
BASENAME=$(basename "$LOCAL_DIR")
if [ -z "$LOCAL_DIR" ]; then
    echo "用法: $0 本地文件夹"
    exit 1
fi

sftp ${USER}@${HOST} <<EOF
cd ${REMOTE_DIR}
mkdir ${BASENAME}
put -r ${LOCAL_DIR}
bye
EOF
