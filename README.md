# О проекте
Современные инструменты для обработки генома вне лаборатории секвенирования не позволяют получить прирост производительности при работе на кластере. В связи с этим, требуется разработать инструмент для сортировки генома, эффективно работающий на кластерных системах.

После секвенирования генома производится его обработка, в которой определяется местоположение фрагментов генома, ридов (read, с англ. прочтение), относительно эталонного (референсного) генома (далее – референс). Обработанные данные поставляются в файлах SAM или BAM (архивированный SAM) формата. Требуется отсортировать риды по координатам относительно референса на кластерной системе для последующего выравнивания.

# Установка и запуск проекта (Ubuntu 16.x.x)
1. Установить https://www.virtualbox.org/wiki/Downloads
2. Скачать образ виртуальной машины (64-bit 16.x.x) https://www.osboxes.org/ubuntu/
3. В приложении VBox: "Создать" -> [Задать имя], [Использовать существующий жёсткий диск] -> [Выбрать *.vdi из предыдущего пункта]
4. После создания можно настроить машину: выделить пару ядер (если есть) и 1-2 GB оперативной памяти
5. Запустить машину
6. Войти. Пароль по умолчанию: **osboxes.org**
7. Открыть терминал
8. Выполнить следующие команды (https://askubuntu.com/questions/15433/unable-to-lock-the-administration-directory-var-lib-dpkg-is-another-process)
```
sudo rm /var/cache/apt/archives/lock
sudo rm /var/lib/dpkg/lock
```
9. Установить следующие пакеты:
```
sudo apt update
sudo apt install git autoconf zlib1g-dev libbz2-dev liblzma-dev libncurses5-dev
```
10. Склонировать проект (https://stackoverflow.com/questions/3796927/how-to-git-clone-including-submodules):
```
cd ~/Downloads
git clone --recurse-submodules https://gitlab.com/sort-dev/gray-team/gray-sort.git
cd gray-sort/
```
11. Установить библиотеку htslib (ignore warnings) (https://github.com/samtools/htslib):
```
cd htslib/
autoheader
autoconf
./configure
make
sudo make install
cd ..
```
12. Установить библиотеку samtools (ignore warnings) (https://github.com/samtools/samtools)
```
cd samtools/
autoheader
autoconf
./configure
make
sudo make install
cd ..
```
13. Подключить собранные библиотеки для динамического связывания
```
sudo ldconfig
```
14. Предварительно из папки проекта (1 раз):
```
autoconf
./configure
```
15. Можно собирать (сортировка чисел):
```
make meta
```

# Тесты
Для тестирования больше всего подходят Bash скрипты, по скольку важнее всего 
получить на выходете корректно обработанный файл. Так же важно провести на корость и
нагрузку системы.
Все скрипты для тестирования можно найти в /scripts/sh
