package main

import (
	"io/ioutil"
	"fmt"
	"net/http"
	"time"
	"bytes"
)

func main() {
	sum := 1
	for sum < 5000{
		c := http.Client{Timeout: time.Duration(1) * time.Second}
		resp, err := c.Get("http://127.0.0.1:16161")
		if err != nil {
			fmt.Printf("Error %s", err)
			return
		}
		defer resp.Body.Close()
		body, err := ioutil.ReadAll(resp.Body)
		fmt.Printf("Body : %s", body)
		sum += 1;
	}
	sum = 1
	file := bytes.NewBuffer([]byte(`{"name":"SubwaySurfer"}`))
	for sum < 5000{
		c := http.Client{Timeout: time.Duration(1) * time.Second}
		resp, err := c.Post("http://127.0.0.1:16161", "okok", file)
		if err != nil {
			fmt.Printf("Error %s", err)
			return
		}
		defer resp.Body.Close()
		body, err := ioutil.ReadAll(resp.Body)
		fmt.Printf("Body : %s", body)
		sum += 1;
	}
}
