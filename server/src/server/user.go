package server

import (
	"encoding/json"
	"fmt"
)

type User struct {
}

type InSetName struct {
	Name string
}
type OutSetName struct {
	Success bool
}

func (u *User) SetName(client *UserClient, data string) (string, error) {
	//解析参数
	var param InSetName
	err := json.Unmarshal([]byte(data), &param)
	if nil != err {
		fmt.Println("parse param failed,", err)
		return "", fmt.Errorf("parse param failed,", err)
	}

	client.SetSessionData("Uname", param.Name)

	out := &OutSetName{
		Success: true,
	}

	//编码结果
	ret, err := json.Marshal(out)
	if nil != err {
		fmt.Println("encode ret failed,", err)
		return "", fmt.Errorf("encode out failed, %v", err)
	}

	return string(ret), nil
}
