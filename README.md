# Вступительное задание
## Структура проекта
- Код исполняемых файлов расположен в папке `examples`.
- Код разработанных статических библиотек расположен в директориях `src` и `include`.
## Сборка и запуск

```bash
git clone git@github.com:popov-p/sorts-duplicates-hulls.git

cd sorts-duplicates-hulls
mkdir build && cd build
```
```bash
export CC=clang \
export CXX=clang++
cmake .. -DCMAKE_BUILD_TYPE=Debug -DUSE_SANITIZERS=On && make
```
или 
```bash
cmake .. -DCMAKE_BUILD_TYPE=Release && make
```