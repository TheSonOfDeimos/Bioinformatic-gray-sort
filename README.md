# О проекте
Современные инструменты для обработки генома вне лаборатории секвенирования не позволяют получить прирост производительности при работе на кластере. В связи с этим, требуется разработать инструмент для сортировки генома, эффективно работающий на кластерных системах.

После секвенирования генома производится его обработка, в которой определяется местоположение фрагментов генома, ридов (read, с англ. прочтение), относительно эталонного (референсного) генома (далее – референс). Обработанные данные поставляются в файлах SAM или BAM (архивированный SAM) формата. Требуется отсортировать риды по координатам относительно референса на кластерной системе для последующего выравнивания.

# Установка и запуск проекта (Ubuntu 16.x.x)
1. Установить следующие пакеты:
```
sudo apt update
sudo apt install git autoconf zlib1g-dev libbz2-dev liblzma-dev libncurses5-dev libopenmpi-dev
```
2. Склонировать проект (https://github.com/TheSonOfDeimos/Bioinformatic-gray-sort.git):
```
cd ~/Downloads
git clone --recurse-submodules https://github.com/TheSonOfDeimos/Bioinformatic-gray-sort.git
cd gray-sort/
```
3. Установить библиотеку htslib (ignore warnings) (https://github.com/samtools/htslib):
```
cd htslib/
autoheader
autoconf
./configure
make
sudo make install
cd ..
```
4. Установить библиотеку samtools (ignore warnings) (https://github.com/samtools/samtools)
```
cd samtools/
autoheader
autoconf
./configure
make
sudo make install
cd ..
```
5. Подключить собранные библиотеки для динамического связывания
```
sudo ldconfig
```
6. Предварительно из папки проекта (1 раз):
```
autoconf
./configure
```
7. Можно собирать (сортировка чисел):
```
make mpiio
```

# Тесты
Для тестирования больше всего подходят Bash скрипты, по скольку важнее всего 
получить на выходете корректно обработанный файл. Так же важно провести на корость и
нагрузку системы.
Все скрипты для тестирования можно найти в /scripts/sh
