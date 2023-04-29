package main

import (
	"fmt"
	"net/http"
	"time"
	"bytes"
)

func main() {
	sum := 1
	for sum < 5000{
		c := http.Client{Timeout: time.Duration(1) * time.Second * 100}
		resp, err := c.Get("http://127.0.0.1:6969")
		if err != nil {
			fmt.Printf("Error %s", err)
			return
		}
		defer resp.Body.Close()
		fmt.Printf("GET package: %d\n", sum)
		sum += 1;
	}
	sum = 1
	file := bytes.NewBuffer([]byte(`{"name":"SubwaySurfer"}`))
	for sum < 5000{
		c := http.Client{Timeout: time.Duration(1) * time.Second * 100}
		resp, err := c.Post("http://127.0.0.1:6969", "okok", file)
		if err != nil {
			fmt.Printf("Error %s", err)
			return
		}
		defer resp.Body.Close()
		fmt.Printf("POST package: %d\n", sum)
		sum += 1;
	}
}
