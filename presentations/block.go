package main

import (
	"math"
	"os"
	"runtime/trace"
	"time"
)

// This example shows blocked state and some CPU usage visualization
// START OMIT
func main() {
	trace.Start(os.Stderr)
	time.Sleep(10 * time.Millisecond)
	ch := make(chan int)
	go func() {
		time.Sleep(10 * time.Millisecond)
		for i := 0; i < 10000000; i++ {
			j := math.Sqrt(float64(i * i))
			j *= j * float64(i)
			_ = j
		}
		ch <- 42
		time.Sleep(10 * time.Millisecond)
	}()
	<-ch
	time.Sleep(100 * time.Millisecond)
	trace.Stop()
}
// END OMIT
