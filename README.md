# ptz-utils
ptz camera control utils   


## 编译方法 


## 层次架构

``` sequence
title: 三个臭皮匠的故事
participant 小王
participant 小李
participant 小异常

note left of 小王: 我是小王
note over 小李: 我是小李
note right of 小异常: 大家好！\n我是小异常

小王->小王: 小王想：今天要去见两个好朋友咯~
小王->小李: 嘿，小李好久不见啊~ 
小李-->>小王: 是啊
小李->小异常: 小异常，你好啊
小异常-->小王: 哈，小王！\n最近身体怎么样了？
小王->>小异常: 还可以吧
```


``` flow
sta=>start: 开始
e=>end: 结束
op=>operation: 操作（处理块）
sub=>subroutine: 子程序
cond=>condition: 是或者不是（条件判断）?
cond2=>condition: 第二个判断（条件判断）?
io=>inputoutput: 输出

sta->op->cond
cond(yes)->e
cond(no)->cond2
cond2(yes,right)->sub(left)-op
cond2(no)->io(lef)->e
```


