package dht

import (
	"bytes"
	"errors"
	"fmt"
	"strconv"
	"strings"
	"unicode"
	"unicode/utf8"
)

// find returns the index of first target in data starting from `start`.
// It returns -1 if target not found.
func find(data []byte, start int, target rune) (index int) {
	index = bytes.IndexRune(data[start:], target)
	if index != -1 {
		return index + start
	}
	return index
}

func DecodeString(data []byte, start int) (result interface{}, index int, err error) {
	i := find(data, start, ':')
	if i == -1 {
		err = errors.New("':' not found when decode string")
		return
	}

	length, err := strconv.Atoi(string(data[start:i]))
	if err != nil {
		return
	}

	if length < 0 {
		err = errors.New("invalid length of string")
		return
	}

	index = i + 1 + length

	if index > len(data) || index < i+1 {
		err = errors.New("out of range")
		return
	}

	result = string(data[i+1 : index])
	return
}

func DecodeInt(data []byte, start int) (result interface{}, index int, err error) {
	index = find(data, start+1, 'e')
	if index == -1 {
		err = errors.New("':' not found when decode int")
		return
	}

	result, err = strconv.Atoi(string(data[start+1 : index]))
	if err != nil {
		return
	}
	index++

	return
}

func decodeItem(data []byte, start int) (result interface{}, index int, err error) {
	if start >= len(data) {
		err = errors.New("start out of range")
		return
	}

	var (
		decodeFunc     func([]byte, int) (interface{}, int, error)
		startCharacter = data[start]
	)
	switch startCharacter {
	case 'i':
		decodeFunc = DecodeInt
	case 'l':
		decodeFunc = DecodeList
	case 'd':
		decodeFunc = DecodeDict
	case '0', '1', '2', '3', '4', '5', '6', '7', '8', '9':
		decodeFunc = DecodeString
	default:
		err = errors.New("invalid bencode when decode item")
		return
	}

	result, index, err = decodeFunc(data, start)
	return
}

func DecodeList(data []byte, start int) (result interface{}, index int, err error) {
	var item interface{}
	r := make([]interface{}, 0, 8)

	index = start + 1
	for index < len(data) {
		char, _ := utf8.DecodeRune(data[index:])
		if char == 'e' {
			break
		}

		item, index, err = decodeItem(data, index)
		if err != nil {
			return
		}
		r = append(r, item)
	}

	if index == len(data) {
		err = errors.New("'e' not found when decode list")
		return
	}
	index++

	result = r
	return
}

func DecodeDict(data []byte, start int) (result interface{}, index int, err error) {
	var item, key interface{}
	r := make(map[string]interface{})

	index = start + 1
	for index < len(data) {
		char, _ := utf8.DecodeRune(data[index:])
		if char == 'e' {
			break
		}

		if !unicode.IsDigit(char) {
			err = errors.New("invalid dict bencode")
			return
		}

		key, index, err = DecodeString(data, index)
		if err != nil {
			return
		}

		if index >= len(data) {
			err = errors.New("out of range")
			return
		}

		item, index, err = decodeItem(data, index)
		if err != nil {
			return
		}

		r[key.(string)] = item
	}

	if index == len(data) {
		err = errors.New("'e' not found when decode dict")
		return
	}
	index++

	result = r
	return
}

func Decode(data []byte) (result interface{}, err error) {
	if data == nil || len(data) == 0 {
		err = errors.New("decode data empty")
		return
	}

	result, _, err = decodeItem(data, 0)
	return
}

func EncodeString(data string) string {
	return fmt.Sprintf("%d:%s", len(data), data)
}

func EncodeInt(data int) string {
	return fmt.Sprintf("i%de", data)
}

func encodeItem(data interface{}) (result string, err error) {
	switch v := data.(type) {
	case string:
		result = EncodeString(v)
	case int:
		result = EncodeInt(v)
	case []interface{}:
		result, err = EncodeList(v)
	case map[string]interface{}:
		result, err = EncodeDict(v)
	default:
		err = errors.New("invalid type when encode result")
	}
	return
}

func EncodeList(data []interface{}) (result string, err error) {
	items := make([]string, len(data))

	for i, item := range data {
		items[i], err = encodeItem(item)
		if err != nil {
			return
		}
	}

	result = fmt.Sprintf("l%se", strings.Join(items, ""))
	return
}

func EncodeDict(data map[string]interface{}) (result string, err error) {
	items, i := make([]string, len(data)), 0

	for key, val := range data {
		item, e := encodeItem(val)
		if e != nil {
			err = e
			return
		}
		items[i] = fmt.Sprintf("%s%s", EncodeString(key), item)
		i++
	}

	result = fmt.Sprintf("d%se", strings.Join(items, ""))
	return
}

func Encode(data interface{}) (result string, err error) {
	result, err = encodeItem(data)
	return
}
