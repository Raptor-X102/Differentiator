# Дифференциатор

## Краткое описание проекта
Проект посвящен программе "Дифференциатор", которая считывает из файла выражение, дифференцирует его, упрощает, выводит поэтапно процесс взятия производной в TeX, а также заменяет длинные выражениями буквами греческого алфавита. Умеет раскладывать в ряд Тейлора от любой точки до нужного порядка и выводит в TeX.

### Пример ввода выражения
```
((x) + (3)) / ((1000) - (7))$
((x) + (3)) / (x)$
ln(3)$
log((3),(x))$
```

### Краткое описание бэкенда
Первый этап - проход по выражению и разбиение символов на отдельные узлы - лексемы. Эти занимается функция Lexeme_separator в файле Diff_read_funcs.cpp. После этого идет проход по массиву лексем и связывание узлов путем рекурсивного спуска. Узлы выражения хранятся в бинарном дереве (подробнее смотри в репозитории [Binary-tree](https://github.com/Raptor-X102/Binary-Tree)).

Корневая функция, которая проходит все этапы - Differentiate_expression в файле Diff_funcs.cpp
Второй этап - дифференцирование. Его осуществляет функция Diff_take_derivative. Инструкции по элементарному дифференцированию узла лежат в файле Elementary_diff_funcs.cpp. Дифференцирует все элементарные функции, в том числе обратные гиперболические.
Третий этап - упрощение выражения. Осуществлено в файле Simplification_funcs.cpp. Основная суть упрощения - если самая глубокая ветка упростилась, то проверяй, можно ли после этого упростить ветку-родителя. Это позволяет делать только 1 проход по дереву.
Разложение в ряд Тейлора происходит за счет многократкого дифференцирования, а также вычисления выражения, которое осуществляется функциями файла Evaluation_funcs.cpp.
Вывод в TeX и замену выражений делают функции в файлах TeX_output.cpp и Replacements_list.cpp.

Пример вывода
![Output_example](other/TeX%20output/Exp_test1.pdf)
