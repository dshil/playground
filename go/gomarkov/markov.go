package gomarkov

import (
	"bytes"
	"fmt"
	"io"
	"math/rand"
	"strings"
)

// Generate randomly generates text for providing input text.
func Generate(in io.Reader) ([]byte, error) {
	var data bytes.Buffer
	if _, err := io.Copy(&data, in); err != nil {
		return nil, err
	}

	prefixesSuffixes := make(map[string][]string)
	text := strings.Split(data.String(), " ")
	for i := 2; i < len(text); i++ {
		prefix := text[i-2] + " " + text[i-1]
		prefixesSuffixes[prefix] = append(prefixesSuffixes[prefix], text[i])
	}

	for k, v := range prefixesSuffixes {
		fmt.Println(k, v)
	}

	w1 := text[0]
	w2 := text[1]
	fmt.Printf("%v %v ", w1, w2)
	for {
		suff := prefixesSuffixes[w1+" "+w2]
		rnum := rand.Intn(len(suff))
		s := suff[rnum]
		fmt.Printf("%v ", s)
		if s == "(end)" {
			break
		}
		w1 = w2
		w2 = s
	}

	return []byte("mock"), nil
}
