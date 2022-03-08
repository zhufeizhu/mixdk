## 测试代码


使用blktrace抓取block io并使用blkparse作图
```shell
blktrace -d /dev/nvme0n1
blkparse -i nvme0n1 -d nvme0n1.blktrace.bin
iowatcher -t nvme0n1.blktrace.bin -o disk.svg
```
生成矢量图


## 测试结果

- block_size: 4k
- size: 4G
- rw: sequential write

是否预热 | 单队列 | 多队列 
--- | --- | --- 
否 | 6.5s  | 3.5s
是 | 3.5s  | 2s

- cmd: fio -filename=/dev/nvme0n1 -direct=1 -thread -rw=write -bs=16k -ioengine=psync -size=4g -iodepth=1 -numjobs=1 -group_reporting -name=test

块大小\线程数 | 1 | 2 | 4 | 8 | 16 | 24
 --- | --- | --- | --- | --- | --- | --- 
  4k | 0.31G | 0.6G | 1.23G | 2.3G | 2.4G | 2.4G
  8k | 0.55G | 1.1G | 2.2G | 2.9G | 3.0G | 3.0G 
  12k | 0.7G | 1.45G | 2.6G | 3G | 3G | 3G
  16k | 0.88G | 1.73G | 3G | 3G | 3G | 3G 
  24k | 1.12G | 2.4G | 3G | 3.1G | 3.1G | 3.1G 

  mixdk测试结果
  块大小 | 4k | 8k | 12k | 16K
   --- | --- | --- | --- | ---
  bw |  0.95G | 1.27G | 2.38G | 
  
  buffer区测结果
  总大小 | 4M | 8M | 12M | 16M(未触发migrate) | 16M(触发migrate)
  --- | --- | --- | ---| --- | --- 
时间 | 7ms | 14ms | 19ms | 25ms | 83ms
带宽 | 570Mb/s | 570Mb/s | 630Mb/s | 640Mb/s | 192Mb/s


功能测试内容
1. 纯nvm小写 
2. 纯ssd