package main

import (
	"fmt"
	"net"
	"runtime"
)

func main() {
	runtime.GOMAXPROCS(1)
	listener, err := net.Listen("tcp", ":8888")
	if err != nil {
		fmt.Println(err)
		return
	}
	for ;; {
		conn, err := listener.Accept()
		if err != nil {
			fmt.Println(err)
			continue
		}
		go func(conn net.Conn) {
			defer conn.Close()
			var buf [100]byte
			_, err := conn.Read(buf[:]);
			if err != nil {
				fmt.Println(err)
				return
			}
			_, err = conn.Write(buf[:]);
			if err != nil {
				fmt.Println(err)
				return
			}
		}(conn)
	}
}
