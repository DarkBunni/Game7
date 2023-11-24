#include <iostream>
#include <vector>
#include <string>
#include "Class.h"   // Подключение класса
#include "Victory.h" // Подключение группы функций проверяющих победные комбинации

using std::cout;
using std::cin;
using std::endl;
using std::string;    // Псевдокласс строки
using std::vector;    // Псевдокласс векторов
using std::to_string; // Встроенная в стандартную библиотеку функция-член, преобразования int в string (только для С++ 11 версии и выше)

void CountingVictories();       // функция для подсчета побед и вызова основной игровой функции
int GameFX(lattice& pole);      // Функция реализующая сценарий игры
void PoleFX(lattice& pole);     // функция отображающая игровое поле
char GetChoice();               // функция для получения ответа на запрос начать новую игру
int GetNumberLevel();           // функция обработки и получения выбранного уровня сложности
void ExaminationCell(lattice& pole);    // функция обработки выбранного пользователем номера ячейки
int CellNumber(lattice& pole);          // функция получения номера ячейки выбранной пользователем
bool ComputerChoiceWin(lattice& pole);      // функция ищет ячейку заняв которую компьютер выиграет
bool ComputerChoiceTrick(lattice& pole);    // функция ищет ячейку заняв которую компьютер не даст выиграть пользователю
void ComputerChoice(lattice& pole);         // функция случайным образом выбирает свободную ячейку
bool OverflowFX(lattice& pole);         // функция для проверки поля на переполнения - ничейный результат

int main()
{
    setlocale(LC_ALL, "Rus");       // Русский язык в консоли
    srand(static_cast<unsigned int>(time(0)));  // Запуск генератора случайных чисел (установка зерна)
    cout << "\tДобро пожаловать в Крестики-Нолики\n\nЭто консольная игра крестики-нолики.";
    cout << "\nДля игры используйте цифры на клавиатуре.";
    CountingVictories();            // Вызов функции подсчитывающей результаты партий
    return 0;
}

void CountingVictories()        // функция для подсчета побед и вызова основной игровой функции
{
    int player = 0, computer = 0, draw = 0; // переменные для подсчета побед

    while (true)
    {
        lattice pole(GetNumberLevel(), rand() % 2);

        PoleFX(pole);                              // выводим игровое поле с результатами партии

        if (pole.GetPointerPleer() == 1)
            std::cout << "\n Вы играете крестиками, ваш ход первый\n";
        else
            std::cout << "\n Вы играете ноликами, первым ходит компьютер\n";

        switch (GameFX(pole))                      // вызываем основную игровую функцию
        {
        case 1:player++; break;                // записываем результат игровой партии
        case 2:computer++; break;
        case 3:draw++; break;
        }

        PoleFX(pole);                              // выводим игровое поле с результатами партии

        pole.~lattice();                           // деструктор для класса

        cout << "\n\n\tИгрок:\t\t- " << player << "\n\tКомпьютер:\t- " << computer << "\n\tНичья:\t\t- " << draw;

        if (GetChoice() == 'n')                    // вызываем функцию - запрос продолжить игру
        {
            cout << "\n\t\t Всего доброго !!!" << endl << endl;  // если на запрос отвечают нет - программа завершается
            break;
        }
    }
}

int GameFX(lattice& pole)       // Функция реализующая сценарий игры
{
    if (pole.GetPointerPleer() == 2)  // методом класса выбираем кто ходит первый и устанавливает значения соответствующих переменных
        goto ferst;                   // если первым ходит компьютер, начинаем игру с блока кода где ходит PC

    while (true)
    {
        ExaminationCell(pole);    // через функцию получаем номер ячейки выбранной пользователем
        pole.SetPointerCheck(pole.GetPointerPleer()); // устанавливаем значение для организации проверки

        if (VictoryFX(pole)) // завершение игрового цикла если выиграл пользователь
        {
            cout << "\n\n\tПоздравляем выиграл пользователь";
            return 1; // возвращаем результат работы GameFX в функцию CountingVictories
        }

        if (OverflowFX(pole)) // завершение игрового цикла если пользователь занял последнюю свободную ячейку
            return 3; // возвращаем результат работы GameFX в функцию CountingVictories

        PoleFX(pole); // вывод функции отображающей игровое поле с символом введенным пользователем

    ferst:

        if (ComputerChoiceWin(pole)) // вызываем функцию которая ищет ячейку заняв которую РС победит
            return 2; // возвращаем результат работы GameFX в функцию CountingVictories

        if (ComputerChoiceTrick(pole))  // вызываем функцию которая занимает ячейки не давая выиграть пользователю
            ComputerChoice(pole);  // если подстава от РС не получилась, вызываем функцию случайного выбора не занятой ячейки

        if (OverflowFX(pole)) // завершение игрового цикла если компьютер занял последнюю свободную ячейку
            return 3; // возвращаем результат работы GameFX в функцию CountingVictories

        cout << endl; // для красоты добавляем разделение строк
        PoleFX(pole);  // вывод функции, отображающей игровое поле с символом введенным компьютером
    }
}

void PoleFX(lattice& pole)      // функция отображающая игровое поле
{
    vector<string> clearing;  // создание вектора со строками
    int line = 0, CellNumber; // определение переменных

    /* цикл заполняющий вектор значениями построчно,
        каждая строка новая итерация, количество строк - размерность поля*/
    for (int i = 0; i < pole.GetDuration(); i++)
    {
        clearing.push_back("\t"); // создание новой переменной string в векторе, а за одно отступ с лева для красоты
        for (int z = 0; z < pole.GetDuration(); z++) // заполняем строку номерами адресов (для красоты, адреса цифрами с лева, поле с крестиками и нолика с права)
        {
            CellNumber = z + line * pole.GetDuration() + 1; // высчитываем номер ячейки, которую сейчас записываем
            std::string CellText;
            if (pole.GetArr()[CellNumber - 1] == 0)
            {
                CellText = to_string(CellNumber); // преобразуем int в string
                if (CellNumber < 10) // добавляем пробел для красивого ряда (если нужен)
                    CellText = ' ' + CellText;
            }
            else
            {
                CellText = " -";
            }
            clearing[i] = clearing[i] + CellText + ' '; // добавляем текст со значением номера ячейки в строку вектора
        }

        clearing[i] = clearing[i] + "\t"; // разделяем пустым пространством поле номеров ячеек и поле с крестиками и ноликами
        for (int z = 0; z < pole.GetDuration(); z++) // добавляем в строку значения ячеек игрового поля
        {
            switch (pole.GetArr()[(z + line * pole.GetDuration())])   // проверяем значение в массиве игрового поля
            {
            case 0:clearing[i] = clearing[i] + "| |"; break;   // добавляем соответствующие символы в строку
            case 1:clearing[i] = clearing[i] + " X "; break;
            case 2:clearing[i] = clearing[i] + " O "; break;
            }
        }
        line++; // увеличиваем значения линии для перехода на новую строку
    }

    cout << endl; // разделитель строк для красоты

    for (int i = 0; i < pole.GetDuration(); i++) // выводим циклом все строки в консоль
    {
        cout << clearing[i] << endl;
    }
}

char GetChoice()                // Функция для получения ответа на запрос продолжить(начать) игру
{
    while (true) // цикл продолжается до тех пор, пока пользователь не введет корректное значение
    {
        cout << "\n\n\tВы хотите сыграть еще? (y/n)?: ";
        char key;
        cin >> key;
        cin.ignore(32767, '\n'); // Лишнее убираем

        if (key == 'y' || key == 'n')  // Выполняем проверку значений
        {
            return key; // возвращаем данные
        }
        else // сообщаем пользователю, что что-то пошло не так
        {
            cout << "\n\n\tВведите английские буквы (Yes/No)\n";
        }
    }
}

int GetNumberLevel()            // Функция обработки и получения выбранного уровня сложности
{
    while (true)     // цикл продолжается до тех пор, пока пользователь не введет корректное значение
    {
        cout << "\n\nВыберете размер поля\n\tТри Х три\t- 3\n\tЧетыре Х четыре\t- 4\n\t...\n\tДевять Х девять\t- 9";
        cout << "\n\n\tКакой уровень сложности вы выберете: ";
        char key;
        cin >> key;
        cin.ignore(32767, '\n');        // удаляем лишние символы введенные с клавиатуры
        if ('3' <= key && key <= '9')   // Выполняем проверку значений
        {
            return key - '0';           // возвращаем данные в функцию main()
        }
        cout << "\tВаш выбор не соответствует условиям, введите корректное значение\n";
    }
}

void ExaminationCell(lattice& pole)     // функция обработки выбранного пользователем номера ячейки
{
    while (true)
    {
        int a = CellNumber(pole);   // через функцию запрашиваем от пользователя число
        if (a > pole.GetField())    // проверяем, что число в заданном диапазон
        {
            cout << "\n\tЗначение за пределами поля, максимум \" " << pole.GetField() << " \"" << endl;
            continue;
        }
        if (pole.GetArr()[a - 1] == 0)                // проверяем, что ячейка выбранная пользователем не занята
        {
            pole.GetArr()[a - 1] = pole.GetPointerPleer();
            break;
        }
        cout << "\n\tЯчейка \" " << a << " \" занята." << endl;
    }
}

int CellNumber(lattice& pole)           // функция получения номера ячейки выбранной пользователем
{
    while (true)    // цикл продолжается до тех пор, пока пользователь не введет корректное значение
    {
        cout << "\n Для установки \"";
        switch (pole.GetPointerPleer())
        {
        case 1: cout << " X "; break;
        case 2: cout << " O "; break;
        }
        cout << "\" введите номер пустой ячейки: ";

        char key[10] = "";          // объявляем Cи строку в 10 значения (чтобы перекрыть некорректный ввод)
        cin.getline(key, 10);       // получаем до 9 символов введенных пользователем
        cin.clear();
        if (key[1] == '\0' && '0' <= key[0] && key[0] <= '9') // проверяем, что первый символ - число, второй пустое место)
        {
            return key[0] - '0'; // если условие соблюдено, возвращаем значение int
        }
        if ('0' <= key[0] && key[0] <= '9' && '0' <= key[1] && key[1] <= '9') // если оба символа числа
        {
            return ((key[0] - '0') * 10 + (key[1] - '0')); // рассчитываем из значение, возвращаем значение int
        }
        cout << "\n\tЗначение не корректно!" << endl;
    }
}

bool ComputerChoiceWin(lattice& pole)      // функция ищет ячейку заняв которую компьютер выиграет
{
    pole.SetPointerCheck(pole.GetPointerPC());      // устанавливаем значение проверяемого символа равному символу которым играет компьютер
    for (int i = 0; i < pole.GetField(); i++)       // обходим все игровое поле
    {
        if (pole.GetArr()[i] == 0)                      // если ячейка пустая
        {
            pole.GetArr()[i] = pole.GetPointerPC();     // устанавливаем в нее символ которым играет компьютер
            if (VictoryFX(pole))                        // проверяем даст ли такое поле выигрышную комбинацию
            {
                cout << "\n\tКомпьютер занимает ячейку " << i + 1 << " и выигрывает.\n";
                return true;
            }
            pole.GetArr()[i] = 0;                       // если нет, сбрасываем ячейку на ноль
        }
    }
    return false;
}

bool ComputerChoiceTrick(lattice& pole)    // функция ищет ячейку заняв которую компьютер не даст выиграть пользователю
{
    pole.SetPointerCheck(pole.GetPointerPleer());      // устанавливаем значение проверяемого символа равному символу которым играет пользователь
    for (int i = 0; i < pole.GetField(); i++)          // обходим циклом игровое поле
    {
        if (pole.GetArr()[i] == 0)                     // если ячейка пустая
        {
            pole.GetArr()[i] = pole.GetPointerPleer(); // ставим в нее символ которым играет пользователь
            if (VictoryFX(pole))                       // проверяем даст ли такая комбинация выигрыш
            {
                pole.GetArr()[i] = pole.GetPointerPC(); // если да, занимаем ячейку символом которым играет компьютер
                cout << "\n\tКомпьютер решает занять ячейку " << i + 1;
                return false;
            }
            pole.GetArr()[i] = 0;                      // если нет, сбрасываем ячейку на ноль
        }
    }
    return true;
}

void ComputerChoice(lattice& pole)         // функция случайным образом выбирает свободную ячейку
{
    int сhose;
    do
    {
        сhose = rand() % (pole.GetField());         // получаем случайное число которое будет выбором компьютера
    } while (pole.GetArr()[сhose] != 0);            // если клетка выбранная компьютером занята, повторяем выбор случайного числа
    pole.GetArr()[сhose] = pole.GetPointerPC();     // присваиваем ячейки значение
    cout << "\n\tКомпьютер наугад занимает ячейку " << сhose + 1;
}

bool OverflowFX(lattice& pole)             // функция для проверки поля на переполнения - ничейный результат
{
    for (int i = 0; i < pole.GetField(); i++)
    {
        if (pole.GetArr()[i] == 0)         // ищем первую свободную ячеку
            return false;
    }
    return true;
}
#include "Class.h"

lattice::lattice(int value, int random)
{
    duration = value;
    field = value * value;
    arr = new int[field];
    for (int i = 0; i < field; i++)
    {
        arr[i] = 0;
    }

    if (random == 0)               // выбираем какой блок кода реализовать
    {
        pointerPleer = 1;          // устанавливаем значение переменных 1 - крестик, 2 - нолик
        pointerPC = 2;             // для пользователя и компьютера
    }
    else {
        pointerPleer = 2;
        pointerPC = 1;
    }

    if (value < 5)
        CountNumberSeries = 3;
    else if (value < 7)
        CountNumberSeries = 4;
    else CountNumberSeries = 5;
}

lattice::~lattice()
{
    delete[] arr;
}

int* lattice::GetArr()
{
    return arr;
}

int lattice::GetField()
{
    return field;
}

int lattice::GetDuration()
{
    return duration;
}

int lattice::GetPointerPleer()
{
    return pointerPleer;
}

int lattice::GetPointerPC()
{
    return pointerPC;
}

int lattice::GetCountNumberSeries()
{
    return CountNumberSeries;
}

int lattice::GetPointerCheck()
{
    return pointerCheck;
}

void lattice::SetPointerCheck(int pointer)
{
    this->pointerCheck = pointer;
}
#ifndef VICTORY_H_INCLUDED
#define VICTORY_H_INCLUDED
#include "Class.h"   // Подключение класса

bool VictoryFX(lattice& pole);    // объединение всех функций проверяющих выигрышную комбинацию в одну
bool VictoryFXa(lattice& pole);   // проверка поля на выигрышную комбинацию по строкам
bool VictoryFXb(lattice& pole);   // проверка поля на выигрышную комбинацию по столбцам
bool VictoryFXc(lattice& pole);   // проверка поля на выигрышную комбинацию по диагоналям с лева на право с началом линии проверки с верхнего ряда
bool VictoryFXc1(lattice& pole);  // проверка поля на выигрышную комбинацию по диагоналям с лева на право с началом линии проверки на левой боковой грани
bool VictoryFXd(lattice& pole);   // проверка поля на выигрышную комбинацию по диагоналям с право на лево с началом линии проверки в верхнем ряду
bool VictoryFXd1(lattice& pole);  // проверка поля на выигрышную комбинацию по диагоналям с право на лево с началом линии проверки на правой боковой грани

#endif // VICTORY_H_INCLUDED
#include "Victory.h"
#include "Class.h"

bool VictoryFXa(lattice& pole) // функция для проверки значений поля на выигрышную комбинацию
{
    /* двумя вложенными циклами обходим массив со значениями поля, проверяем каждую
   строчку на заполнение переданным в функцию символом (" Х " или " О "), если находим символ
   увеличиваем значение CounterRow на единицу, проверяем если CounterRow достиг
   значения NumberSeries выходим из функции и возвращаем TRUE.
   Когда строка дошла до конца CounterRow опять присваивается значение ноль
   и повторяется внутренний цикл проверяющий строку*/

    for (int i = 0; i < pole.GetField(); i += pole.GetDuration()) // цикл для перехода по строкам, сдвигаемся каждый раз на размерность поля (duration)
    {
        int CounterRow = 0; // обнуление счетчика
        for (int z = 0; z < pole.GetDuration(); z++) // внутренний цикл обходит строку
        {
            if (pole.GetArr()[i + z] == pole.GetPointerCheck()) // проверяем текущее значение вектора
                CounterRow++; // увеличиваем счетчик если пройдена проверка
            else
                CounterRow = 0; // обнуляем счетчик если цепочка нужных символов прервалась
            if (CounterRow == pole.GetCountNumberSeries()) // если счетчик достиг значения
                return true; // завершаем функцию - передаем TRUE
        }
    }
    return false;
}

bool VictoryFXb(lattice& pole)
{
    int CounterRow;

    for (int z = 0; z < pole.GetDuration(); z++) // аналогично проверяем каждый столбец, сдвигая начало внутреннего цикла на единицу
    {
        CounterRow = 0;                  // обнуляем счетчик
        for (int i = 0; i < pole.GetField(); i += pole.GetDuration()) // переходя на размерность поля проверяем столбец вниз
        {
            if (pole.GetArr()[i + z] == pole.GetPointerCheck())     // если находим нужный символ увеличиваем счетчик
                CounterRow++;
            else
                CounterRow = 0; // обнуляем счетчик если цепочка нужных символов прервалась
            if (CounterRow == pole.GetCountNumberSeries())  // если счетчик достиг значения равного нужной комбинации
                return true;   // завершаем функцию - передаем TRUE
        }
    }
    return false;
}

bool VictoryFXc(lattice& pole)
{
    /* проверяем диагональ из правого верхнего угла в левый нижний*/
    for (int i = 0; i <= pole.GetDuration() - pole.GetCountNumberSeries(); i++)
    {
        int CounterRow = 0;
        int stroka = 0;
        for (int z = i; z < pole.GetField(); z = z + pole.GetDuration() + 1)
        {
            if (stroka > z / pole.GetDuration())
            {
                stroka++;
                continue;
            }
            if (pole.GetArr()[z] == pole.GetPointerCheck()) // проверяем значение вектора по индексу
            {
                CounterRow++;      // если находим нужный символ увеличиваем счетчик
            }
            else
            {
                CounterRow = 0;    // обнуляем счетчик если цепочка нужных символов прервалась
            }
            if (CounterRow == pole.GetCountNumberSeries())  // если счетчик достиг значения равного нужной комбинации
            {
                return true;   // завершаем функцию - передаем TRUE
            }
            stroka++;
        }
    }
    return false;
}

bool VictoryFXc1(lattice& pole)
{
    /* в зеркально проверяем диагональ из правого верхнего угла в левый нижний*/
    for (int i = pole.GetDuration(); i < pole.GetDuration() * (pole.GetDuration() - pole.GetCountNumberSeries() + 1); i = i + pole.GetDuration())
    {
        int CounterRow = 0;
        int stroka = 0;
        for (int z = i; z < pole.GetField(); z = z + pole.GetDuration() + 1)
        {
            if (stroka > z / pole.GetDuration())
            {
                stroka++;
                continue;
            }
            if (pole.GetArr()[z] == pole.GetPointerCheck()) // проверяем значение вектора по индексу
            {
                CounterRow++;      // если находим нужный символ увеличиваем счетчик
            }
            else
            {
                CounterRow = 0;    // обнуляем счетчик если цепочка нужных символов прервалась
            }
            if (CounterRow == pole.GetCountNumberSeries())  // если счетчик достиг значения равного нужной комбинации
            {
                return true;   // завершаем функцию - передаем TRUE
            }
            stroka++;
        }
    }
    return false;
}

bool VictoryFXd(lattice& pole)
{
    /* в зеркально проверяем диагональ из правого верхнего угла в левый нижний*/
    for (int i = pole.GetCountNumberSeries() - 1; i < pole.GetDuration(); i++)
    {
        int CounterRow = 0;
        int stroka = 0;
        for (int z = i; z < pole.GetField(); z = z + pole.GetDuration() - 1)
        {
            if (stroka > z / pole.GetDuration())
            {
                stroka++;
                continue;
            }
            if (pole.GetArr()[z] == pole.GetPointerCheck()) // проверяем значение вектора по индексу
            {
                CounterRow++;      // если находим нужный символ увеличиваем счетчик
            }
            else
            {
                CounterRow = 0;    // обнуляем счетчик если цепочка нужных символов прервалась
            }
            if (CounterRow == pole.GetCountNumberSeries())  // если счетчик достиг значения равного нужной комбинации
            {
                return true;   // завершаем функцию - передаем TRUE
            }
            stroka++;
        }
    }
    return false;
}

bool VictoryFXd1(lattice& pole)
{
    for (int i = pole.GetDuration() * 2 - 1; i < pole.GetDuration() * (pole.GetDuration() - pole.GetCountNumberSeries() + 1); i = i + pole.GetDuration())
    {
        int CounterRow = 0;
        int stroka = 1;
        for (int z = i; z < pole.GetField(); z = z + pole.GetDuration() - 1)
        {
            if (stroka > z / pole.GetDuration())
            {
                stroka++;
                continue;
            }
            if (pole.GetArr()[z] == pole.GetPointerCheck()) // проверяем значение вектора по индексу
            {
                CounterRow++;      // если находим нужный символ увеличиваем счетчик
            }
            else
            {
                CounterRow = 0;    // обнуляем счетчик если цепочка нужных символов прервалась
            }
            if (CounterRow == pole.GetCountNumberSeries())  // если счетчик достиг значения равного нужной комбинации
            {
                return true;   // завершаем функцию - передаем TRUE
            }
            stroka++;
        }
    }
    return false;
}

bool VictoryFX(lattice& pole)    // объединение всех функций проверяющих выигрышную комбинацию в одну
{
    return (VictoryFXa(pole) || VictoryFXb(pole) || VictoryFXc(pole) || VictoryFXc1(pole) || VictoryFXd(pole) || VictoryFXd1(pole));
}