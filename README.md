# fincore

あるファイルがページキャッシュにのっているかどうかを調べたい

## how to build
```
g++ -std=c++11 fincore.cpp -o fincore
```

## how to check
```
# clear page cache
sync; sudo sh -c "echo 1 > /proc/sys/vm/drop_caches"

./fincore $FILEPATH
```

## NOTE
* `open()`や`fstat()`をしただけでは、キャッシュされず、`read()`されるとキャッシュされる
* 特定のファイルキャッシュを解放したい場合は、`posix_fadvice()`の`POSIX_FADV_DONTNEED`を利用する

## 公式ツール
[Google Code Archive \- Long\-term storage for Google Code Project Hosting\.]( https://code.google.com/archive/p/linux-ftools/ )

ただし、うまくビルドできず

Ubuntu16.04の下記に含まれているはずだが、含まれておらず
```
sudo apt-get install -y util-linux
```
