#/bin/bash
./ipc_rm_all.sh  
./ReadData &
sleep 5
pid_reader=$!
./WriteRootData &
pid_writer=$!

