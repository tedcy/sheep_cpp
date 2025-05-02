package main

import (
	"net"
	"fmt"
	"runtime"
)

func main() {
	runtime.GOMAXPROCS(1)
	for ;; {
		var buf [100]byte
		conn, err := net.Dial("tcp", "127.0.0.1:8888")
		if err != nil {
			fmt.Println(err)
			continue
		}
		conn.Write(buf[:]);
		conn.Read(buf[:]);
		err = conn.Close()
		if err != nil {
			fmt.Println(err)
		}
		return;
	}
}
