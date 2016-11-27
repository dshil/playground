package gomarkov

import (
	"bytes"
	"testing"

	"github.com/stretchr/testify/assert"
	"github.com/stretchr/testify/require"
)

var testText = `Show your flowcharts and conceal your tables and I will be mystified. Show your tables and your flowcharts will be obvious. (end)`

func TestGenerate(t *testing.T) {
	r := bytes.NewReader([]byte(testText))
	out, err := Generate(r)
	require.Nil(t, err)
	assert.NotEmpty(t, out)
}
