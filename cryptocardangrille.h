#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <utility>
#include <string>
#include <clocale>

class GrilleKey4x4;
class GrilleKey5x5;
class GrilleKey6x6;

template <typename KeyArray>
class CryptoCardanGrille
{
    /* Класс CryptoCardanGrille позволяет осуществлять шифрование/дешифрование текстовых данных,
	 * используя алгоритм квадрата Кардано.*/

public:

	CryptoCardanGrille()
	{
	    /* Устанавливаем локаль
         * Russian_Russia.1251 - идентификатор в формате POSIX*/
        setlocale(LC_ALL, "Russian_Russia.1251");
	}

	/* Шифруем файл.
	 * Для шифрования файла разбиваем считанные строки на болки
	 * по размеру равному квадрату стороны решетки (4*4, 5*5, ...),
	 * строки (блоки) размер который меньше квадрата стороны решетки
	 * дополняем не печатаемым символом с кодом Ascii равным нулю.
	 * Вызываем функцию std::string encrypt(const std::string& input),
	 * которая шифрует сформерованную подстраку и возвращает шифротекст,
	 * далее собираем строку из шифротекста и пишем в файл. */
    void encryptFile(const std::string& input, const std::string& output)
    {
        std::ifstream file1(input.data(), std::ifstream::in);
        std::ofstream file2(output.data(), std::ofstream::out);
		const int size = KeyArray::size * KeyArray::size;
		for(std::string line; getline(file1, line);) {
			std::string crypt;
			for(std::size_t i = 0; i < line.length(); i+=size){
				std::string sub = line.substr(i, size);
				//sub.assign(line, i, size);
			    while(size > sub.length())
					  sub.push_back(0);
				crypt += this->encrypt(sub);
			}
			file2<<crypt<<std::endl;
        }
        file1.close(); file2.close();
    }

	/* Дешифруем файл
	 * для дешифровки считанные строки разбиваем на болки
	 * по размеру равному квадрату стороны решетки (4*4, 5*5, ...), далее
	 * вызываем функцию std::string decrypt(const std::string& input),
	 * которая дешифрует сформерованную подстраку и возвращает исходный текст,
	 * далее собираем строку из боков исходного теста и пишем в файл. */
    void decryptFile(const std::string& input, const std::string& output)
    {
        std::ifstream file1(input.data(), std::ifstream::in);
        std::ofstream file2(output.data(), std::ofstream::out);
		const int size = KeyArray::size * KeyArray::size;
		for(std::string line; getline(file1, line);) {
			std::string crypt;
			for(std::size_t i = 0; i < line.length(); i+=size) {
				std::string sub = line.substr(i, size);
				crypt.append(this->decrypt(sub));
			}
			file2<<crypt<<std::endl;
        }
        file1.close(); file2.close();
    }

	/* Функция шифрует строку размером равным
	 * квадрату стороны решетки (4*4, 5*5, ...).
	 * Последовательно перемещаемся по шаблону решетки (квадрату),
	 * предварительно определив длину результирующей строки (resize).
	 * Вслучае если элемент решетки равен еденицы записываем в
	 * результирующую строку элемнт входного параметра, индекс элемента
	 * результирующей строки вычисляем по формуле (строка * размер + столбец)
	 * Пройдя всю решетку (квадрат) поворачиваем ее вокруг оси на 90 градусов
	 * повторяем операцию rotateCount раз */
	std::string encrypt(const std::string& input) {
		std::string result;
		if(KeyArray::size * KeyArray::size == input.length()) {
			int index = 0;
			result.resize(KeyArray::size * KeyArray::size);
			for (int x = 0; x < rotateCount; ++x) {
				for (int y = 0; y < KeyArray::size; ++y)
					for (int z = 0; z < KeyArray::size; ++z)
						if(key(y, z) == 1)
							result[y * KeyArray::size + z] = input[index++];
				this->rotate();
			}
		/*Если размер решетки не четный заполняем центральный элемент массива (строки)*/
		/*Так как не существует решетки при которой центральный элемент заполнялся бы всего одни раз*/
		if(KeyArray::size % 2 != 0)
			result[(int)(KeyArray::size * KeyArray::size) / 2] = input[index];
		}
		return result;
	}

	/* Функция дешифрует строку размером равным
	 * квадрату стороны решетки (4*4, 5*5, ...).
	 * Последовательно перемещаемся по шаблону решетки (квадрату).
	 * Вслучае если элемент решетки равен еденицы помещаем в
	 * результирующую строку элемнт входного параметра, предварительно
	 * проверив элемент на равенство нулевому коду табоицы Ascii, индекс элемента
	 * входного параметра вычисляем по формуле (строка * размер + столбец)
	 * Пройдя всю решетку (квадрат) поворачиваем ее вокруг оси на 90 градусов
	 * повторяем операцию rotateCount раз */
    std::string decrypt(const std::string& input) {
		std::string src;
		if(KeyArray::size * KeyArray::size == input.length()) {
			int index = 0;
			for (int x = 0; x < rotateCount; ++x) {
				for (int y = 0; y < KeyArray::size; ++y)
					for (int z = 0; z < KeyArray::size; ++z)
						if(key(y, z) == 1){
							unsigned char ch = input[y * KeyArray::size + z];
							if (ch != 0)
								src.push_back(ch);
						}
				this->rotate();
			}

	    /*Если размер решетки не четный центральный элемент массива помещаем в конец строки*/
		if(KeyArray::size % 2 != 0){
			int center = (int)(KeyArray::size * KeyArray::size) / 2;
			unsigned char ch = input[center];
			if (ch != 0) src.push_back(ch);
		}

		}
		return src;
	}

    virtual ~CryptoCardanGrille(){}

private:

	/* Поворот решетки на 90 градусов */
	void rotate()  {
		for(int i = 0; i < KeyArray::size; ++i)
		    for(int j = i; j < KeyArray::size; ++j)
			    std::swap(key(i, j), key(j, i));

		for(int i = 0; i < KeyArray::size; ++i)
			for(int j = 0; j < KeyArray::size / 2; ++j)
			  std::swap(key(i, j), key(i,  KeyArray::size - j - 1));
	}

	static const int rotateCount = 4;
	KeyArray key;
};


class GrilleKey4x4
{
	/*
	 * Класс решетки (квадрата) 4 X 4
	 * Выступает в роли ключа шифрования для алгоритма Кардано */
public:

	GrilleKey4x4() {
		key[0][0] = 0; key[0][1] = 0; key[0][2] = 1; key[0][3] = 0;
	    key[1][0] = 0; key[1][1] = 0; key[1][2] = 0; key[1][3] = 1;
        key[2][0] = 0; key[2][1] = 1; key[2][2] = 0; key[2][3] = 0;
        key[3][0] = 1; key[3][1] = 0; key[3][2] = 0; key[3][3] = 0;
	}

	~GrilleKey4x4() { }

	int& operator()(int row, int col) {
		return key[row][col];
	}

	const int& operator()(int row, int col) const {
		return key[row][col];
	}

	static const std::size_t size = 4;

private:

	int key[size][size];
};

class GrilleKey5x5
{
	/*
	 * Класс решетки (квадрата) 5 X 5
	 * Выступает в роли ключа шифрования для алгоритма Кардано */
public:

	GrilleKey5x5() {
		key[0][0] = 0; key[0][1] = 1; key[0][2] = 0; key[0][3] = 1; key[0][4] = 0;
	    key[1][0] = 0; key[1][1] = 0; key[1][2] = 0; key[1][3] = 0; key[1][4] = 0;
        key[2][0] = 0; key[2][1] = 0; key[2][2] = 0; key[2][3] = 1; key[2][4] = 0;
        key[3][0] = 0; key[3][1] = 1; key[3][2] = 0; key[3][3] = 0; key[3][4] = 0;
		key[4][0] = 1; key[4][1] = 0; key[4][2] = 1; key[4][3] = 0; key[4][4] = 0;
	}

	~GrilleKey5x5() { }

	int& operator()(int row, int col) {
		return key[row][col];
	}

	const int& operator()(int row, int col) const {
		return key[row][col];
	}

	static const std::size_t size = 5;

private:

	int key[size][size];
};

class GrilleKey6x6
{
	/*
	 * Класс решетки (квадрата) 6 X 6
	 * Выступает в роли ключа шифрования для алгоритма Кардано */
public:

	GrilleKey6x6() {
	    int keyA[size][size] = {{0, 1, 0, 0, 0, 0},
						        {0, 0, 0, 1, 0, 0},
                                {1, 0, 0, 0, 1, 0},
                                {1, 0, 0, 1, 0, 0},
                                {0, 0, 0, 0, 1, 0},
						        {1, 1, 0, 0, 0, 0}};

		for (int i = 0; i < size; ++i)
			for (int j = 0; j < size; ++j)
				key[i][j] = keyA[i][j];

	}

	~GrilleKey6x6() { }

	int& operator()(int row, int col) {
		return key[row][col];
	}

	const int& operator()(int row, int col) const {
		return key[row][col];
	}

	static const std::size_t size = 6;

private:

	int key[size][size];
};

// GrilleKey4x4;
// GrilleKey5x5;
// GrilleKey6x6;

	/*Инстанцированием параметра шаблона получаем необходимый размер решетки*/
/*
	CryptoCardanGrille<GrilleKey4x4> obj;
	std::cout<<"Шифрование и дешифрование файла используя алгоритм квадрата Кардано..."<<std::endl;
	obj.encryptFile("data.txt","encrypt.txt");
	obj.decryptFile("encrypt.txt", "decrypt.txt");
	std::cout<<"Работа завершина ..."<<std::endl;
*/


