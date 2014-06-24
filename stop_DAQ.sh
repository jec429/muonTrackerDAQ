#/bin/bash
pid_reader=`pgrep -u daquser ReadData`
pid_writer=`pgrep -u daquser WriteRootData`
kill -s SIGINT $pid_reader
sleep 3
kill -s SIGINT $pid_writer

